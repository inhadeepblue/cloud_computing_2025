# 실습 가이드 (Lab Guide)

## 준비사항

### 작업 디렉토리 생성

```bash
# week10 디렉토리 생성 및 이동
mkdir -p ~/week10
cd ~/week10

# 실습용 하위 디렉토리 생성
mkdir -p ~/week10/docker_lab
mkdir -p ~/week10/encryption_lab
mkdir -p ~/week10/test_files
```


***

## 실습 1: Docker 네트워크를 이용한 가상 네트워크 시뮬레이션

### 목표

Docker 네트워크를 구성하여 클라우드 VPC와 보안 그룹의 개념을 학습한다.

### 1-1. Docker 네트워크 기본 (10분)

**Step 1: 사용자 정의 네트워크 생성**

```bash
# week10 디렉토리로 이동
cd ~/week10/docker_lab

# 네트워크 생성 (VPC 개념)
docker network create cloud-network

# 네트워크 확인
docker network ls
docker network inspect cloud-network
```

**Step 2: 컨테이너 네트워크 연결**

```bash
# 웹 계층 컨테이너 (공개 서브넷)
docker run -d \
  --name web-server \
  --network cloud-network \
  -p 80:80 \
  nginx:alpine

# 애플리케이션 계층 컨테이너 (프라이빗 서브넷)
docker run -d \
  --name app-server \
  --network cloud-network \
  -e API_PORT=5000 \
  python:3.9-alpine sleep 1000

# 데이터베이스 계층 컨테이너 (프라이빗 서브넷)
docker run -d \
  --name db-server \
  --network cloud-network \
  -e MYSQL_ROOT_PASSWORD=password \
  mysql:5.7
```

**Step 3: 컨테이너 상태 확인**

```bash
# 실행 중인 컨테이너 확인
docker ps

# 네트워크에 연결된 컨테이너 확인
docker network inspect cloud-network | grep -A 20 "Containers"

# 웹 서버 접근 확인
curl http://localhost
```


### 1-2. 컨테이너 간 통신 제어

**Step 1: 컨테이너 간 통신 테스트**

```bash
# 웹 서버에서 앱 서버로 통신 (같은 네트워크)
docker exec web-server ping -c 2 app-server

# 웹 서버에서 데이터베이스로 통신
docker exec web-server ping -c 2 db-server

# 결과: 모두 성공적으로 통신됨 (DNS 기반 이름 해석)
```

**Step 2: 격리된 네트워크 생성**

```bash
# 별도의 격리된 네트워크 생성 (NACL 개념)
docker network create isolated-network

# 격리된 데이터베이스 시작
docker run -d \
  --name isolated-db \
  --network isolated-network \
  -e MYSQL_ROOT_PASSWORD=secret \
  mysql:5.7

# cloud-network에서 isolated-network로의 통신 불가
docker exec web-server ping -c 2 isolated-db
# 결과: 실패 (격리된 네트워크라 접근 불가)
```


### 1-3. 방화벽 규칙 시뮬레이션

**Step 1: 포트 제한 (보안 그룹 개념)**

```bash
# 특정 포트만 노출하는 컨테이너
docker run -d \
  --name restricted-web \
  --network cloud-network \
  -p 8080:80 \
  --expose 80 \
  nginx:alpine

# 특정 포트 접근 불가능하도록 설정
docker run -d \
  --name firewall-test \
  --network cloud-network \
  --cap-drop=NET_RAW \
  alpine:latest sleep 1000

# 접근 제한 확인
docker exec firewall-test wget -O- http://web-server:80
# 결과: 웹 서버에서만 접근 가능
```

**Step 2: 테스트 정리**

```bash
# 모든 컨테이너 중지
docker stop web-server app-server db-server isolated-db restricted-web firewall-test

# 컨테이너 제거
docker rm web-server app-server db-server isolated-db restricted-web firewall-test

# 네트워크 제거
docker network rm cloud-network isolated-network
```


### 클라우드 네트워킹 매핑

```
Docker 실습 ↔ 클라우드 개념
─────────────────────────────
Docker Network  ↔ VPC
Isolated Network ↔ Network Segmentation
Container Links ↔ Security Groups
Port Mapping    ↔ Firewall Rules
```
