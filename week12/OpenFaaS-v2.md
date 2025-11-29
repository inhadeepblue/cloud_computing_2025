# OpenFaaS 실습 가이드

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

출력 예시:

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

생성되는 네임스페이스:
- openfaas - OpenFaaS 핵심 서비스용
- openfaas-fn - 함수(Functions) 배포용

---

## 4단계: Helm 설치

```bash
# Helm 설치 (이미 설치되어 있는지 확인)
helm version

# Helm이 없다면 설치
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash
```

---

## 5단계: Helm으로 OpenFaaS 설치 (Community Edition)

중요: Arkade는 라이선스 문제가 있어 Helm으로 직접 설치합니다.

### 5-1: Helm Repository 추가

```bash
# OpenFaaS Helm Repository 추가
helm repo add openfaas https://openfaas.github.io/faas-netes/
helm repo update

# Repository 확인
helm search repo openfaas
```

### 5-2: OpenFaaS 설치

```bash
# 네임스페이스 생성 (이미 생성했다면 스킵 가능)
kubectl create namespace openfaas
kubectl create namespace openfaas-fn

# Helm으로 OpenFaaS Community Edition 설치
helm install openfaas openfaas/openfaas \
  --namespace openfaas \
  --set functionNamespace=openfaas-fn \
  --set generateBasicAuth=true \
  --set serviceType=NodePort \
  --set openfaasImagePullPolicy=IfNotPresent
```

설치 파라미터 설명:
- functionNamespace: 배포할 함수들이 실행될 네임스페이스
- generateBasicAuth: 기본 인증 정보 자동 생성
- serviceType: NodePort로 설정 (Minikube에서 접근 가능)
- openfaasImagePullPolicy: IfNotPresent로 설정 (로컬 이미지 우선 사용)

### 5-3: 설치 확인 및 대기

```bash
# 모든 Pod가 Running 상태일 때까지 대기
kubectl get pods -n openfaas

# 실시간 모니터링 (모두 Running이 될 때까지 기다림)
kubectl get pods -n openfaas -w
# Ctrl+C로 종료
```

예상 출력 (5-10분 소요):

```
NAME                            READY   STATUS    RESTARTS   AGE
gateway-6c5b4d8f4b-xxxxx       2/2     Running   0          5m
nats-75f84f8d7c-xxxxx          1/1     Running   0          5m
queue-worker-5d4c8f9b9-xxxxx   1/1     Running   0          5m
prometheus-855c7f5c9c-xxxxx    1/1     Running   0          5m
autoscaler-7dd6c4d7c5-xxxxx    1/1     Running   0          5m
dashboard-85c6668b64-xxxxx     1/1     Running   0          5m
```

### 5-4: 설치 문제 해결

만약 Pod가 ContainerCreating에서 멈춘다면:

```bash
# 문제 진단
kubectl describe pod <pod-name> -n openfaas

# 이벤트 확인
kubectl get events -n openfaas --sort-by='.lastTimestamp' | tail -10

# 라이선스 Secret 에러가 보인다면 완전히 다시 설치
helm uninstall openfaas -n openfaas
kubectl delete namespace openfaas openfaas-fn
kubectl delete crd functions.openfaas.com profiles.openfaas.com

# 잠시 대기
sleep 5

# 처음부터 재설치
kubectl create namespace openfaas
kubectl create namespace openfaas-fn

helm install openfaas openfaas/openfaas \
  --namespace openfaas \
  --set functionNamespace=openfaas-fn \
  --set generateBasicAuth=true \
  --set serviceType=NodePort \
  --set openfaasImagePullPolicy=IfNotPresent

kubectl get pods -n openfaas -w
```

---

## 6단계: 포트 포워딩 설정

```bash
# Gateway 포트 포워딩 (백그라운드에서 실행)
kubectl port-forward -n openfaas svc/gateway 8080:8080 &

# UI 접근 (선택사항)
kubectl port-forward -n openfaas svc/prometheus 9090:9090 &
```

---

## 7단계: 기본 인증 정보 확인

```bash
# 관리자 비밀번호 확인
PASSWORD=$(kubectl get secret -n openfaas basic-auth -o jsonpath="{.data.basic-auth-password}" | base64 --decode; echo)
echo "Password: $PASSWORD"

# OpenFaaS CLI에 로그인
echo -n $PASSWORD | faas-cli login --username admin --password-stdin --gateway http://127.0.0.1:8080
```

---

## 8단계: 함수 프로젝트 생성

```bash
# 디렉토리 생성
mkdir openfaas-functions
cd openfaas-functions

# 새로운 함수 생성 (Node.js 템플릿 사용)
faas-cli new hello-node --lang node18

# 생성된 파일 확인
ls -la
```

생성되는 파일:
- stack.yaml - 함수 정의 파일 (이전 버전에서는 hello-node.yml)
- hello-node/ - 함수 소스 코드 디렉토리
- template/ - 함수 템플릿 디렉토리

함수 핸들러 (hello-node/handler.js) 수정 예:

```javascript
'use strict'

module.exports = async (context) => {
    return {
        "message": "Hello from OpenFaaS!",
        "university": "Inha",
        "time": new Date()
    }
}
```

---

## 9단계: 함수 배포 (로컬 이미지 사용)

로컬 이미지를 사용하는 세 가지 방법 중 선택.

### 9-1: Option A - Docker Hub 사용 (공개 이미지, 프로덕션 스타일)

stack.yaml 파일 수정:

```yaml
version: 1.0
provider:
  name: openfaas
  gateway: http://127.0.0.1:8080

functions:
  hello-node:
    lang: node18
    handler: ./hello-node
    image: inhadeepblue/hello-node:latest
```

배포:

```bash
# Docker Hub에 로그인
docker login

# 이미지 빌드 및 Push
export OPENFAAS_PREFIX=inhadeepblue  # export OPENFAAS_PREFIX=[your-dockerhub-username] 

faas-cli up -f stack.yaml

# 완료 확인
faas-cli list
```

### 9-2: Option B - Minikube Docker 환경 사용

stack.yaml 파일 수정:

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
    skip_build: false
```

배포:

```bash
# Minikube Docker 환경으로 전환
eval $(minikube docker-env)

# 이미지 빌드
faas-cli build -f stack.yaml

# 배포
faas-cli deploy -f stack.yaml

# 확인
faas-cli list
```

주의: 매 터미널 세션마다 eval $(minikube docker-env)를 실행해야 합니다.

### 9-3: Option C - Minikube Image Load 사용 (가장 빠름)

stack.yaml 파일 수정:

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
    skip_build: true
```

배포:

```bash
# 1단계: 이미지 빌드
faas-cli build -f stack.yaml

# 2단계: 생성된 Dockerfile 확인
ls -la build/hello-node/

# 3단계: 일반 Docker로 이미지 빌드 (Minikube 환경 밖에서)
docker build -t hello-node:latest ./build/hello-node

# 4단계: Minikube에 이미지 로드 (1-2초 소요)
minikube image load hello-node:latest

# 5단계: 이미지 확인
minikube image ls | grep hello-node

# 6단계: 배포 (push 없이)
faas-cli deploy -f stack.yaml

# 7단계: 확인
faas-cli list
```

코드 수정 후 재배포:

```bash
# 1단계: 이미지 빌드
faas-cli build -f stack.yaml

# 2단계: Dockerfile 기반 빌드
docker build -t hello-node:latest ./build/hello-node

# 3단계: 로드 (매우 빠름)
minikube image load hello-node:latest

# 4단계: 재배포
faas-cli deploy -f stack.yaml
```

---

## 10단계: 함수 테스트

### 10-1: 함수 호출

```bash
# CLI로 호출
faas-cli invoke hello-node

# 또는 HTTP 요청으로 호출
curl http://127.0.0.1:8080/function/hello-node

# POST 요청으로 데이터 전송
curl -X POST http://127.0.0.1:8080/function/hello-node -d "test data"
```

### 10-2: 함수 상태 확인

```bash
# 함수 목록 조회
faas-cli list

# 함수 상세 정보
faas-cli describe hello-node
```

### 10-3: 함수 로그 확인

```bash
# 함수 Pod 로그 확인
kubectl logs -n openfaas-fn -l app=hello-node

# 실시간 로그 모니터링
kubectl logs -n openfaas-fn -l app=hello-node -f
```

---

## 11단계: 웹 대시보드 접근

### 11-1: 포트 포워딩 (아직 설정하지 않았다면)

```bash
# 대시보드 포트 포워딩
kubectl port-forward -n openfaas svc/gateway 8080:8080 &
```

### 11-2: 브라우저에서 접근

```
http://127.0.0.1:8080
```

로그인:
- 사용자명: admin
- 비밀번호: 7단계에서 확인한 비밀번호

---

## 12단계: OpenFaaS CLI 주요 명령어

```bash
# 함수 목록 조회
faas-cli list

# 함수 생성
faas-cli new <function-name> --lang <language>

# 함수 빌드
faas-cli build -f stack.yaml

# 함수 배포
faas-cli deploy -f stack.yaml

# 함수 호출
faas-cli invoke <function-name>

# 함수 정보 조회
faas-cli describe <function-name>

# 함수 제거
faas-cli remove <function-name>

# Gateway 로그인
faas-cli login --username <username> --password <password>

# Gateway 로그아웃
faas-cli logout
```

---

## 트러블슈팅

### 문제: Pod이 ContainerCreating 상태에서 멈춤

해결책:

```bash
# 문제 원인 확인
kubectl describe pod <pod-name> -n openfaas

# 이미지 Pull 오류 확인
kubectl get events -n openfaas | grep -i pull

# 노드 리소스 확인
kubectl top nodes

# Minikube 재시작
minikube stop
minikube start
```

### 문제: FailedMount 에러 (라이선스 관련)

해결책:

```bash
# 완전히 제거 후 재설치
helm uninstall openfaas -n openfaas
kubectl delete namespace openfaas openfaas-fn
kubectl delete crd functions.openfaas.com profiles.openfaas.com

# 재설치 (5단계 참고)
kubectl create namespace openfaas
kubectl create namespace openfaas-fn

helm install openfaas openfaas/openfaas \
  --namespace openfaas \
  --set functionNamespace=openfaas-fn \
  --set generateBasicAuth=true \
  --set serviceType=NodePort \
  --set openfaasImagePullPolicy=IfNotPresent
```

### 문제: 함수 배포 실패

해결책:

```bash
# 네임스페이스 확인
kubectl get namespaces | grep openfaas

# 함수 Pod 로그 확인
kubectl logs -n openfaas-fn -l app=<function-name>

# 완료된 Pod 정리
kubectl delete pods --field-selector status.phase=Succeeded -n openfaas-fn
kubectl delete pods --field-selector status.phase=Failed -n openfaas-fn
```

### 문제: 포트 포워딩 연결 거부

해결책:

```bash
# 기존 포트 포워딩 프로세스 확인
ps aux | grep "port-forward"

# 기존 프로세스 종료
pkill -f "port-forward"

# 새로운 포트 포워딩 시작
kubectl port-forward -n openfaas svc/gateway 8080:8080
```

### 문제: stack.yaml 파일이 생성되지 않음

해결책:

```bash
# OpenFaaS CLI 버전 확인
faas-cli version

# CLI 업그레이드 (최신 버전으로)
curl -sSL https://cli.openfaas.com | sudo sh

# 다시 함수 생성
faas-cli new hello-node --lang node18
```

---

## 추가 리소스

- 공식 문서: https://docs.openfaas.com/
- GitHub 저장소: https://github.com/openfaas/faas-netes
- 튜토리얼: https://github.com/openfaas/workshop

---

## 체크리스트

배포 전 확인 사항:

- Minikube 실행 중
- OpenFaaS 모든 Pod Running 상태
- 포트 포워딩 설정 완료
- 관리자 비밀번호 확인
- 함수 코드 작성 완료
- stack.yaml 파일 설정 완료
- 이미지 빌드 및 로드/Push 완료

---

**마지막 업데이트:** 2025-11-29
**작성자:** OpenFaaS 실습 가이드
**라이센스:** MIT