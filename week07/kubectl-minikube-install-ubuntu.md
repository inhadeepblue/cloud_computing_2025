# kubectl 및 MiniKube 설치 가이드 (Ubuntu)

이 가이드는 Ubuntu VM 환경에서 kubectl과 MiniKube를 설치하는 방법을 단계별로 설명합니다.

## 사전 준비사항

### 시스템 요구사항
- **Ubuntu:** 20.04 LTS 이상
- **RAM:** 최소 4GB (권장 8GB)
- **CPU:** 최소 2 cores
- **디스크 여유 공간:** 20GB 이상
- **Docker:** 사전 설치 필요 (6주차에서 설치 완료)

### Docker 설치 확인
```bash
# Docker 버전 확인
docker --version

# Docker 서비스 상태 확인
sudo systemctl status docker
```

---

## kubectl 설치

kubectl은 Kubernetes 클러스터와 상호작용하기 위한 명령행 도구입니다.

```bash
# 최신 버전 다운로드
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"

# 실행 권한 부여
chmod +x kubectl

# 시스템 PATH에 설치
sudo mv kubectl /usr/local/bin/

# 설치 확인
kubectl version --client
```

**예상 출력:**
```
Client Version: v1.28.x
Kustomize Version: v5.x.x
```

---

## MiniKube 설치

MiniKube는 로컬 환경에서 Kubernetes 클러스터를 실행하는 도구입니다.

```bash
# MiniKube 바이너리 다운로드
curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64

# 실행 권한 부여
chmod +x minikube

# 시스템 PATH에 설치
sudo mv minikube /usr/local/bin/

# 설치 확인
minikube version
```

**예상 출력:**
```
minikube version: v1.32.x
commit: xxxxx
```

---

## MiniKube 클러스터 시작

```bash
# MiniKube 클러스터 시작 (Docker 드라이버 사용)
minikube start --driver=docker

# 클러스터 상태 확인
minikube status
```

**예상 출력:**
```
minikube
type: Control Plane
host: Running
kubelet: Running
apiserver: Running
kubeconfig: Configured
```

---

## 설치 확인

```bash
# 클러스터 정보 확인
kubectl cluster-info

# 노드 목록 확인
kubectl get nodes
```

**예상 출력:**
```
NAME       STATUS   ROLES           AGE   VERSION
minikube   Ready    control-plane   1m    v1.28.3
```

---

## 첫 번째 애플리케이션 배포 테스트

```bash
# 간단한 Nginx Pod 생성
kubectl run test-pod --image=nginx:1.21

# Pod 상태 확인
kubectl get pods

# 정리
kubectl delete pod test-pod
```

---

## 유용한 명령어

```bash
# MiniKube 중지
minikube stop

# MiniKube 재시작
minikube start

# MiniKube 삭제 (완전 초기화)
minikube delete

# MiniKube 대시보드 실행
minikube dashboard
```

---

## 문제 해결

### Docker 권한 오류가 발생하는 경우

```bash
# 현재 사용자를 docker 그룹에 추가
sudo usermod -aG docker $USER

# 그룹 변경 적용
newgrp docker

# MiniKube 재시작
minikube delete
minikube start
```

### MiniKube 시작 실패 시

```bash
# Docker 서비스 재시작
sudo systemctl restart docker

# 기존 MiniKube 완전 삭제 후 재시작
minikube delete --all --purge
minikube start --driver=docker
```

---

## 설치 완료 체크리스트

- [ ] kubectl 설치 완료 (`kubectl version --client`)
- [ ] MiniKube 설치 완료 (`minikube version`)
- [ ] MiniKube 클러스터 시작 성공 (`minikube status`)
- [ ] kubectl로 클러스터 접근 확인 (`kubectl get nodes`)
- [ ] 테스트 Pod 생성 및 삭제 성공

