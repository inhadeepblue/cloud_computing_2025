# Week 14: 클라우드 컴퓨팅 Lab

## 실습 준비물

Docker, Minikube, kubectl 설치 완료

---

## 사전 준비

### 환경 확인

```bash
# Docker 버전 확인
docker --version

# Minikube 상태 확인
minikube status

# kubectl 버전 확인
kubectl version --client
```

### 작업 디렉토리 생성

```bash
mkdir -p ~/week14-final-lab
cd ~/week14-final-lab
```

---

## Phase 1: Docker Compose로 애플리케이션 구성

### 1-1. Flask 애플리케이션 작성

```bash
# 애플리케이션 디렉토리 생성
mkdir -p app
cd app
```

**app.py 파일 생성:**

```bash
cat > app.py << 'EOF'
from flask import Flask, jsonify, request
from prometheus_client import Counter, Histogram, Gauge, generate_latest
import time

app = Flask(__name__)

# Prometheus 메트릭 정의
requests_total = Counter(
    'flask_requests_total',
    'Total requests',
    ['method', 'endpoint', 'status']
)

request_duration = Histogram(
    'flask_request_duration_seconds',
    'Request duration',
    ['method', 'endpoint']
)

deployment_total = Gauge('deployment_total', 'Total deployments')
active_requests = Gauge('flask_active_requests', 'Active requests')

@app.before_request
def before_request():
    request.start_time = time.time()
    active_requests.inc()

@app.after_request
def after_request(response):
    duration = time.time() - request.start_time
    requests_total.labels(
        method=request.method,
        endpoint=request.endpoint or 'unknown',
        status=response.status_code
    ).inc()
    request_duration.labels(
        method=request.method,
        endpoint=request.endpoint or 'unknown'
    ).observe(duration)
    active_requests.dec()
    return response

@app.route('/api/hello', methods=['GET'])
def hello():
    return jsonify({'message': 'Hello, Cloud Computing!', 'timestamp': time.time()})

@app.route('/api/process', methods=['POST'])
def process():
    data = request.get_json()
    time.sleep(0.5)
    return jsonify({'result': 'processed', 'data': data})

@app.route('/health', methods=['GET'])
def health():
    return jsonify({'status': 'healthy'}), 200

@app.route('/ready', methods=['GET'])
def ready():
    return jsonify({'status': 'ready'}), 200

@app.route('/metrics', methods=['GET'])
def metrics():
    return generate_latest(), 200, {'Content-Type': 'text/plain; charset=utf-8'}

@app.route('/error', methods=['GET'])
def error():
    return jsonify({'error': 'Simulated error'}), 500

if __name__ == '__main__':
    deployment_total.set(1)
    app.run(host='0.0.0.0', port=5000, debug=True)
EOF
```

**Dockerfile 생성:**

```bash
cat > Dockerfile << 'EOF'
FROM python:3.9-slim

WORKDIR /app

RUN pip install flask prometheus-client psycopg2-binary redis

COPY app.py .

EXPOSE 5000

CMD ["python", "app.py"]
EOF
```

### 1-2. Prometheus 설정 파일 작성

```bash
cd ~/week14-final-lab

cat > prometheus.yml << 'EOF'
global:
  scrape_interval: 15s
  evaluation_interval: 15s

scrape_configs:
  - job_name: 'prometheus'
    static_configs:
      - targets: ['localhost:9090']

  - job_name: 'flask-app'
    static_configs:
      - targets: ['app:5000']

  - job_name: 'postgres'
    static_configs:
      - targets: ['db:5432']
EOF
```

### 1-3. Docker Compose 파일 작성

```bash
cat > docker-compose.yml << 'EOF'
version: '3.8'

services:
  app:
    build: ./app
    ports:
      - "5000:5000"
    environment:
      FLASK_APP: app.py
      FLASK_ENV: development
    depends_on:
      - db
      - redis
    networks:
      - app-net

  db:
    image: postgres:13-alpine
    environment:
      POSTGRES_DB: myapp
      POSTGRES_USER: admin
      POSTGRES_PASSWORD: secret
    volumes:
      - db_data:/var/lib/postgresql/data
    networks:
      - app-net

  redis:
    image: redis:7-alpine
    networks:
      - app-net

  prometheus:
    image: prom/prometheus:latest
    ports:
      - "9090:9090"
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml
      - prometheus_data:/prometheus
    command:
      - '--config.file=/etc/prometheus/prometheus.yml'
    networks:
      - app-net

  grafana:
    image: grafana/grafana:latest
    ports:
      - "3000:3000"
    environment:
      GF_SECURITY_ADMIN_PASSWORD: admin
    volumes:
      - grafana_data:/var/lib/grafana
    depends_on:
      - prometheus
    networks:
      - app-net

volumes:
  db_data:
  prometheus_data:
  grafana_data:

networks:
  app-net:
    driver: bridge
EOF
```

### 1-4. 애플리케이션 시작

```bash
# Docker Compose로 모든 서비스 시작
docker-compose up -d

# 서비스 상태 확인
docker-compose ps

# 로그 확인
docker-compose logs app
```

### 1-5. 애플리케이션 테스트

```bash
# 헬스 체크
curl http://localhost:5000/health

# API 테스트
curl http://localhost:5000/api/hello

# 메트릭 확인
curl http://localhost:5000/metrics
```

---

## Phase 2: Kubernetes 배포

### 2-1. Minikube 시작

```bash
# Minikube 시작 (이미 실행 중이면 스킵)
minikube start --memory=4096 --cpus=2

# 상태 확인
minikube status
```

### 2-2. Docker 이미지 빌드

```bash
cd ~/week14-final-lab

# Docker 이미지 빌드
docker build -t flask-app:latest ./app

# Minikube에 이미지 로드
minikube image load flask-app:latest

# 이미지 확인
minikube image ls | grep flask-app
```

### 2-3. Kubernetes Deployment 파일 작성

```bash
cat > k8s-deployment.yaml << 'EOF'
apiVersion: apps/v1
kind: Deployment
metadata:
  name: flask-app
spec:
  replicas: 3
  selector:
    matchLabels:
      app: flask-app
  template:
    metadata:
      labels:
        app: flask-app
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "5000"
        prometheus.io/path: "/metrics"
    spec:
      containers:
      - name: app
        image: flask-app:latest
        imagePullPolicy: Never
        ports:
        - containerPort: 5000
        livenessProbe:
          httpGet:
            path: /health
            port: 5000
          initialDelaySeconds: 10
          periodSeconds: 5
        readinessProbe:
          httpGet:
            path: /ready
            port: 5000
          initialDelaySeconds: 5
          periodSeconds: 5
---
apiVersion: v1
kind: Service
metadata:
  name: flask-app-svc
spec:
  type: NodePort
  ports:
  - port: 5000
    targetPort: 5000
    nodePort: 30500
  selector:
    app: flask-app
EOF
```

### 2-4. Kubernetes에 배포

```bash
# Deployment 적용
kubectl apply -f k8s-deployment.yaml

# Pod 상태 확인
kubectl get pods

# Pod가 Running 상태가 될 때까지 대기
kubectl get pods -w
# Ctrl+C로 종료

# Service 확인
kubectl get svc

# Deployment 확인
kubectl get deployments
```

### 2-5. 애플리케이션 접근

```bash
# Minikube 서비스 URL 확인
minikube service flask-app-svc --url

# 또는 포트 포워딩
kubectl port-forward svc/flask-app-svc 5000:5000 &

# 테스트
curl http://localhost:5000/api/hello
```

---

## Phase 3: Prometheus 메트릭 수집

### 3-1. Prometheus 접속

```bash
# 브라우저에서 접속
http://localhost:9090
```

### 3-2. 메트릭 조회 실습

**쿼리 입력창에 다음 쿼리들을 입력하고 "Execute" 클릭:**

1. **현재 요청 수:**
```
flask_requests_total
```

2. **요청률 (초당):**
```
rate(flask_requests_total[5m])
```

3. **에러율:**
```
rate(flask_requests_total{status=~"5.."}[5m])
```

4. **활성 요청 수:**
```
flask_active_requests
```

### 3-3. 그래프 확인

- "Graph" 탭을 클릭하여 시간별 변화 확인
- 시간 범위 조정: 5분, 15분, 1시간
- 자동 새로고침 설정

### 3-4. 부하 테스트 (별도 터미널)

```bash
# 트래픽 생성
while true; do
  curl http://localhost:5000/api/hello
  sleep 1
done
```

---

## Phase 4: Grafana 대시보드 구축

### 4-1. Grafana 접속

```bash
# 브라우저에서 접속
http://localhost:3000

# 로그인 정보
username: admin
password: admin
```

### 4-2. Prometheus 데이터 소스 추가

1. 좌측 메뉴에서 "Connections" 또는 "Configuration" 클릭
2. "Data Sources" 선택
3. "Add data source" 버튼 클릭
4. "Prometheus" 선택
5. 설정:
   - **Name:** Prometheus
   - **URL:** http://prometheus:9090
6. 하단 "Save & test" 클릭
7. "Data source is working" 메시지 확인

### 4-3. 대시보드 생성

1. 좌측 메뉴 → "Dashboards"
2. "New" → "New Dashboard" 클릭
3. "Add visualization" 클릭
4. 데이터 소스로 "Prometheus" 선택

### 4-4. 패널 추가

#### 패널 1: 요청률 (RPS)

- **쿼리:** `rate(flask_requests_total[5m])`
- **Title:** 요청률 (RPS)
- **Visualization:** Time series
- **Apply** 클릭

#### 패널 2: 에러율

- "Add" → "Visualization" 클릭
- **쿼리:** `rate(flask_requests_total{status=~"5.."}[5m])`
- **Title:** 에러율
- **Visualization:** Gauge
- Threshold 설정:
  - 녹색: 0 - 0.05
  - 주황색: 0.05 - 0.1
  - 빨강색: 0.1+
- **Apply** 클릭

#### 패널 3: 활성 요청 수

- "Add" → "Visualization" 클릭
- **쿼리:** `flask_active_requests`
- **Title:** 활성 요청 수
- **Visualization:** Stat
- **Apply** 클릭

### 4-5. 대시보드 저장

1. 우측 상단 "Save dashboard" 아이콘 클릭
2. 이름 입력: "Week14 모니터링 대시보드"
3. "Save" 클릭

### 4-6. 자동 새로고침 설정

- 우측 상단 시간 범위 옆 드롭다운
- "5s" 또는 "10s" 선택

---

## 실습 정리

### 서비스 중지

```bash
# Docker Compose 중지
cd ~/week14-final-lab
docker-compose down

# Kubernetes 리소스 삭제
kubectl delete -f k8s-deployment.yaml

# Minikube 중지 (선택사항)
minikube stop
```

### 디렉토리 구조 확인

```bash
tree ~/week14-final-lab

# 예상 구조:
# week14-final-lab/
# ├── app/
# │   ├── app.py
# │   └── Dockerfile
# ├── docker-compose.yml
# ├── prometheus.yml
# └── k8s-deployment.yaml
```

---