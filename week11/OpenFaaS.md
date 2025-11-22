# OpenFaaS 실습 가이드

> **주의**: Community Edition 라이센스 문제 해결

---

## 준비 상황 확인

이 가이드는 다음이 이미 설치되어 있음을 가정합니다:
- Docker
- Docker Compose
- Minikube
- kubectl

```bash
# 설치 상황 확인
docker --version
minikube version
kubectl version --client
```

---

## 1단계: OpenFaaS CLI 설치

```bash
# OpenFaaS CLI 설치
curl -sSL https://cli.openfaas.com | sudo sh

# 설치 확인
faas-cli version
```

---

## 2단계: Minikube 시작

```bash
# Minikube 시작 (충분한 리소스 할당)
minikube start --memory 4096 --cpus 2

# Minikube 상태 확인
minikube status
```

**출력 예시:**
```
minikube
type: Control Plane
host: Running
kubelet: Running
apiserver: Running
kubeconfig: Configured
```

---

## 3단계: OpenFaaS 네임스페이스 생성

```bash
# OpenFaaS용 네임스페이스 생성
kubectl apply -f https://raw.githubusercontent.com/openfaas/faas-netes/master/namespaces.yml

# 네임스페이스 확인
kubectl get namespaces

# openfaas와 openfaas-fn이 생성되었는지 확인
kubectl get ns | grep openfaas
```

**생성되는 네임스페이스:**
- `openfaas` - OpenFaaS 핵심 서비스용
- `openfaas-fn` - 함수(Functions) 배포용

---

## 4단계: Helm 설치

```bash
# Helm 설치 스크립트 다운로드 및 실행
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash

# Helm 버전 확인
helm version
```

**출력 예시:**
```
version.BuildInfo{Version:"v3.13.0", GitCommit:"...", GitTreeState:"clean", GoVersion:"go1.21.0"}
```

---

## 5단계: Arkade로 OpenFaaS 설치 (CE 라이센스 문제 해결)

> **중요**: Helm 직접 설치 대신 **arkade**를 사용합니다.
> 이렇게 하면 로컬 이미지를 사용할 수 있어 라이센스 문제가 없습니다.

### 5-1: Arkade 설치

```bash
# Arkade 설치
curl -sLS https://get.arkade.dev | sudo sh

# Arkade 버전 확인
arkade version
```

### 5-2: OpenFaaS 설치 (arkade 사용)

```bash
# OpenFaaS 설치 (로컬 이미지 정책 설정)
arkade install openfaas \
  --set openfaasImagePullPolicy=IfNotPresent

# 설치 확인 - 모든 Pod가 Running 상태일 때까지 대기
kubectl get pods -n openfaas

# 실시간 모니터링 (모두 Running이 될 때까지 기다림)
kubectl get pods -n openfaas -w
# (Ctrl+C로 종료)
```

**예상 출력:**
```
NAME                            READY   STATUS    RESTARTS   AGE
gateway-6c5b4d8f4b-xxxxx       1/1     Running   0          2m
nats-75f84f8d7c-xxxxx          1/1     Running   0          2m
queue-worker-5d4c8f9b9-xxxxx   1/1     Running   0          2m
prometheus-855c7f5c9c-xxxxx    1/1     Running   0          2m
```

---

## 6단계: OpenFaaS Gateway 접속 설정

### 6-1: 포트 포워딩

```bash
# 포트 포워딩 (별도 터미널에서 실행하거나 백그라운드 실행)
kubectl port-forward -n openfaas svc/gateway 8080:8080 &
```

### 6-2: 비밀번호 확인

```bash
# OpenFaaS 비밀번호 확인
PASSWORD=$(kubectl get secret -n openfaas basic-auth -o jsonpath="{.data.basic-auth-password}" | base64 --decode; echo)

echo "OpenFaaS 비밀번호: $PASSWORD"
```

### 6-3: OpenFaaS CLI 로그인

```bash
# OpenFaaS CLI 로그인
echo -n $PASSWORD | faas-cli login --username admin --password-stdin

# 로그인 확인
faas-cli list
```

### 6-4: 웹 UI 접속

웹 브라우저를 열고 다음 URL에 접속하세요:

```
http://localhost:8080
```

**로그인 정보:**
- Username: `admin`
- Password: `$PASSWORD` (위에서 확인한 비밀번호)

---

## 7단계: 첫 번째 함수 생성 (Node.js)

### 7-1: 작업 디렉토리 생성

```bash
# 작업 디렉토리 생성
mkdir -p ~/openfaas-lab
cd ~/openfaas-lab
```

### 7-2: 템플릿 다운로드

```bash
# 사용 가능한 Node.js 템플릿 다운로드
faas-cli template store pull node18
```

### 7-3: 함수 생성

```bash
# Node.js 함수 생성
faas-cli new --lang node18 hello-node

# 생성된 파일 확인
ls -la
```

**생성되는 파일:**
```
hello-node.yml          (함수 정의 파일)
hello-node/             (함수 코드 디렉토리)
hello-node/handler.js   (함수 코드)
hello-node/package.json (의존성 정의)
```

---

## 8단계: 함수 코드 작성

### 8-1: handler.js 편집

```bash
nano hello-node/handler.js
```

### 8-2: 다음 코드를 입력하세요

```javascript
'use strict'

module.exports = async (event, context) => {
  const name = event.body || 'World';
  
  const result = {
    status: "success",
    message: `안녕하세요, ${name}님!`,
    timestamp: new Date().toISOString(),
    serverless: "OpenFaaS"
  };
  
  return context
    .status(200)
    .headers({"Content-Type": "application/json"})
    .succeed(JSON.stringify(result, null, 2));
}
```

저장: `Ctrl+X` → `Y` → `Enter`

---

## 9단계: 함수 배포 설정 수정 (라이센스 문제 해결)

### 9-1: YAML 파일 편집

```bash
nano hello-node.yml
```

### 9-2: 다음과 같이 수정하세요

**Option A: Docker Hub 사용**

Docker Hub 계정이 있다면 username을 입력하세요:

```yaml
version: 1.0
provider:
  name: openfaas
  gateway: http://127.0.0.1:8080

functions:
  hello-node:
    lang: node18
    handler: ./hello-node
    image: [your-dockerhub-username]/hello-node:latest
```

예: Docker Hub username이 `inhadeepblue`이면:
```yaml
    image: inhadeepblue/hello-node:latest
```

**Option B: Private Registry 사용**

```yaml
version: 1.0
provider:
  name: openfaas
  gateway: http://127.0.0.1:8080

functions:
  hello-node:
    lang: node18
    handler: ./hello-node
    image: hello-node:latest
    imagePullPolicy: IfNotPresent
```

---

## 10단계: 함수 빌드 및 배포

### 10-1: Option A 선택한 경우 (Docker Hub)

```bash
# Docker Hub 로그인
docker login

# Minikube Docker 환경 해제 (Docker Hub에 푸시하기 위해)
eval $(minikube docker-env -u)

# 빌드, 푸시, 배포를 한 번에
faas-cli up -f hello-node.yml
```

또는 단계별로:

```bash
# 빌드
faas-cli build -f hello-node.yml

# 푸시
faas-cli push -f hello-node.yml

# 배포
faas-cli deploy -f hello-node.yml
```

### 10-2: Option B 선택한 경우 (Private Registry)

```bash
# Minikube Docker 환경 사용 (이미지를 로컬에서 빌드)
eval $(minikube docker-env)

# 빌드
faas-cli build -f hello-node.yml

# 배포
faas-cli deploy -f hello-node.yml
```

### 10-3: 배포 확인

```bash
# 배포된 함수 목록 확인
faas-cli list

# Pod 상태 확인
kubectl get pods -n openfaas-fn

# 모두 Running 상태가 될 때까지 대기
kubectl get pods -n openfaas-fn -w
# (Ctrl+C로 종료)
```

**예상 출력:**
```
Function                      Invocations    Replicas
hello-node                    0              1/1
```

---

## 11단계: 함수 테스트

### 11-1: CLI로 함수 호출

```bash
# 기본 요청 (빈 요청)
echo "" | faas-cli invoke hello-node

# 이름을 전달해서 호출
echo "홍길동" | faas-cli invoke hello-node

# 다른 이름으로 호출
echo "김철수" | faas-cli invoke hello-node
```

**예상 출력:**
```json
{
  "status": "success",
  "message": "안녕하세요, 김철수님!",
  "timestamp": "2025-11-22T03:10:00.000Z",
  "serverless": "OpenFaaS"
}
```

### 11-2: curl로 함수 호출

```bash
# 기본 요청
curl http://127.0.0.1:8080/function/hello-node

# 이름을 전달
curl http://127.0.0.1:8080/function/hello-node \
  -H "Content-Type: text/plain" \
  -d "개발자"

# JSON 형태로 보기 좋게 출력 (jq 필요)
curl -s http://127.0.0.1:8080/function/hello-node \
  -d "OpenFaaS" | jq .
```

### 11-3: 웹 UI에서 테스트

1. 브라우저에서 `http://localhost:8080` 접속
2. `hello-node` 함수 클릭
3. Request body에 "테스트" 입력
4. "Invoke" 버튼 클릭

---

## 12단계: 보너스 - 두 번째 함수 만들기 (계산기)

### 12-1: 함수 생성

```bash
# 계산기 함수 생성
faas-cli new --lang node18 calculator
```

### 12-2: handler.js 작성

```bash
nano calculator/handler.js
```

```javascript
'use strict'

module.exports = async (event, context) => {
  try {
    const input = JSON.parse(event.body);
    const { operation, a, b } = input;
    
    let result;
    switch(operation) {
      case 'add':
        result = a + b;
        break;
      case 'subtract':
        result = a - b;
        break;
      case 'multiply':
        result = a * b;
        break;
      case 'divide':
        result = b !== 0 ? a / b : 'Error: Division by zero';
        break;
      default:
        result = 'Error: Unknown operation';
    }
    
    return context
      .status(200)
      .succeed(JSON.stringify({ 
        operation, 
        a, 
        b, 
        result 
      }));
  } catch (error) {
    return context
      .status(400)
      .succeed(JSON.stringify({ 
        error: error.message 
      }));
  }
}
```

### 12-3: YAML 파일 수정

```bash
nano calculator.yml
```

**Option A 선택한 경우:**
```yaml
    image: [your-dockerhub-username]/calculator:latest
```

**Option B 선택한 경우:**
```yaml
    image: calculator:latest
    imagePullPolicy: IfNotPresent
```

### 12-4: 배포

```bash
# Option A (Docker Hub)
eval $(minikube docker-env -u)
faas-cli up -f calculator.yml

# 또는 Option B (Private Registry)
eval $(minikube docker-env)
faas-cli build -f calculator.yml
faas-cli deploy -f calculator.yml
```

### 12-5: 테스트

```bash
# 덧셈
echo '{"operation":"add","a":10,"b":5}' | faas-cli invoke calculator

# 곱셈
echo '{"operation":"multiply","a":7,"b":8}' | faas-cli invoke calculator

# 나눗셈
echo '{"operation":"divide","a":100,"b":4}' | faas-cli invoke calculator

# curl로 테스트
curl -X POST http://127.0.0.1:8080/function/calculator \
  -H "Content-Type: application/json" \
  -d '{"operation":"subtract","a":50,"b":20}'
```

---

## 유용한 명령어 모음

```bash
# 배포된 함수 목록
faas-cli list

# 특정 함수 상세 정보
faas-cli describe hello-node

# 함수 로그 확인
faas-cli logs hello-node

# 함수 로그 실시간 확인
faas-cli logs hello-node --tail

# 함수 삭제
faas-cli remove hello-node

# 모든 함수 삭제
faas-cli remove --all

# OpenFaaS Store에서 미리 만들어진 함수 확인
faas-cli store list

# Store 함수 배포
faas-cli store deploy figlet
faas-cli store deploy nodeinfo

# Store 함수 테스트
echo "OpenFaaS" | faas-cli invoke figlet
faas-cli invoke nodeinfo

# 사용 가능한 언어 템플릿 확인
faas-cli template store list

# 새로운 템플릿 다운로드
faas-cli template store pull go

# Kubernetes 상태 확인
kubectl get all -n openfaas
kubectl get pods -n openfaas-fn
kubectl get svc -n openfaas

# 특정 Pod의 상세 정보
kubectl describe pod hello-node-xxxxx -n openfaas-fn
```

---

## 문제 해결

### 문제 1: Pod가 Pending 상태

```bash
# Pod 상세 정보 확인
kubectl describe pod hello-node-xxxxx -n openfaas-fn

# Minikube 리소스 상태 확인
minikube ssh -- docker ps
```

**해결방법:**
- Minikube 메모리를 더 할당: `minikube start --memory 6144 --cpus 4`

### 문제 2: 함수 호출 실패

```bash
# 함수 로그 확인
faas-cli logs hello-node

# Pod 로그 직접 확인
kubectl logs -n openfaas-fn -l faas_function=hello-node
```

### 문제 3: 포트 포워딩 문제

```bash
# 기존 포트 포워딩 프로세스 종료
pkill -f "port-forward.*gateway"

# 포트 포워딩 다시 설정
kubectl port-forward -n openfaas svc/gateway 8080:8080
```

### 문제 4: Gateway 접속 불가

```bash
# Gateway 상태 확인
kubectl get pod -n openfaas -l app=gateway

# Gateway 로그 확인
kubectl logs -n openfaas -l app=gateway

# Gateway 서비스 확인
kubectl get svc -n openfaas gateway
```

### 문제 5: 로그인 실패

```bash
# 비밀번호 재확인
kubectl get secret -n openfaas basic-auth -o jsonpath="{.data.basic-auth-password}" | base64 --decode; echo

# 수동 로그인
faas-cli login --username admin --password [비밀번호]
```

---

## 정리 및 리소스 해제

### 모든 함수 제거

```bash
# 배포된 모든 함수 제거
faas-cli remove --all

# 또는 특정 함수 제거
faas-cli remove hello-node
faas-cli remove calculator
```

### OpenFaaS 완전 제거

```bash
# Arkade로 설치한 경우
arkade uninstall openfaas

# 또는 수동으로
helm uninstall openfaas -n openfaas

# 네임스페이스 삭제
kubectl delete namespace openfaas openfaas-fn
```

### Minikube 중지

```bash
# Minikube 중지 (데이터 유지)
minikube stop

# Minikube 완전 삭제
minikube delete
```

---

## 실습 요약

### 배운 내용
- Arkade를 사용한 OpenFaaS 설치 (라이센스 문제 해결)
- Node.js 함수 생성 및 배포
- CLI 및 curl을 사용한 함수 호출
- OpenFaaS 웹 UI 사용
- 로그 확인 및 모니터링
- OpenFaaS의 전체 워크플로우 이해

### 다음 단계
- 외부 API 호출하는 함수 만들기
- 데이터베이스와 연동하는 함수 만들기
- Python, Go, Java 등 다른 언어로 함수 만들기
- 함수 성능 최적화하기
- 마이크로서비스 아키텍처 구축하기

---

## 참고 자료

- [OpenFaaS 공식 문서](https://docs.openfaas.com/)
- [Arkade 공식 문서](https://arkade.dev/)
- [OpenFaaS GitHub](https://github.com/openfaas/faas)
- [OpenFaaS 커뮤니티 포럼](https://github.com/openfaas/faas/discussions)

---

**마지막 업데이트:** 2025-11-22
**작성자:** OpenFaaS 실습 가이드
**라이센스:** MIT