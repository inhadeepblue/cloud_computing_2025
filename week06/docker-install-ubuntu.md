# Docker 설치 가이드 (Ubuntu)

이 문서는 Ubuntu (22.04 이상 추천)에서 Docker Engine을 설치하고 간단히 테스트하는 과정을 안내합니다. 공식 Docker 문서를 기반으로 하며, 64비트 아키텍처(x86_64, arm64 등)를 지원합니다. Ubuntu 파생 배포판(예: Linux Mint)에서는 작동할 수 있지만 공식 지원되지 않을 수 있습니다.

## 사전 요구사항
- **OS**: Ubuntu 22.04 (Jammy LTS), 24.04 (Noble LTS), 25.04 (Plucky), 또는 25.10 (Questing) 이상의 64비트 버전.
- **방화벽**: ufw나 firewalld 사용 시 Docker와 ufw 문서를 확인하세요. Docker는 iptables-nft 또는 iptables-legacy만 지원합니다.
- **권한**: 터미널에서 sudo 권한으로 명령어 실행.

## 1. 기존 Docker 버전 제거
비공식 패키지와 충돌을 피하기 위해 제거합니다. (이미 설치되지 않은 경우 무시됨)
```bash
for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; do sudo apt-get remove $pkg; done
```
- **주의**: 이미지, 컨테이너, 볼륨 등 데이터는 제거되지 않음. 필요 시 `/var/lib/docker/` 수동 삭제.

## 2. Docker 저장소 설정 및 설치
### 2.1. GPG 키 및 저장소 추가
```bash
# 업데이트 및 필수 패키지 설치
sudo apt-get update
sudo apt-get install ca-certificates curl

# GPG 키 디렉터리 생성 및 키 다운로드
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc

# 저장소 추가
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
```

### 2.2. Docker 패키지 설치
최신 버전 설치:
```bash
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

특정 버전 설치 시:
1. 사용 가능 버전 확인:
   ```bash
   apt-cache madison docker-ce | awk '{ print $3 }'
   ```
2. 원하는 버전으로 설치 (예시):
   ```bash
   VERSION_STRING=5:28.5.1-1~ubuntu.24.04~noble  # 실제 버전으로 변경
   sudo apt-get install docker-ce=$VERSION_STRING docker-ce-cli=$VERSION_STRING containerd.io docker-buildx-plugin docker-compose-plugin
   ```

### 2.3. 서비스 상태 확인 및 시작
```bash
sudo systemctl status docker
```
- 시작되지 않았다면:
  ```bash
  sudo systemctl start docker
  sudo systemctl enable docker  # 부팅 시 자동 시작
  ```

## 3. 설치 후 설정 (비루트 사용자 권한 부여, 선택적)
기본적으로 sudo 필요. 비루트 실행 시:
```bash
sudo groupadd docker  # 이미 존재할 수 있음
sudo usermod -aG docker $USER
newgrp docker
```
- 로그아웃 후 재로그인하거나 `newgrp docker`로 적용 확인.

## 4. 간단 테스트
```bash
docker run hello-world
```
- (비루트 설정 후 sudo 생략 가능)
- 출력 예시:
  ```
  Hello from Docker!
  This message shows that your installation appears to be working correctly.
  ...
  ```

## 주의사항
- **인터넷 스크립트**: 이번 가이드는 apt 저장소 사용으로 안전. 다운로드 스크립트 실행 시 내용 검토.
- **문제 해결**: `sudo journalctl -u docker`로 로그 확인.
- **디스크 관리**: `/var/lib/docker`가 공간을 차지할 수 있음. `docker system prune`으로 정기 정리.
- **참고 문서**: [Docker 공식 설치 가이드](https://docs.docker.com/engine/install/ubuntu/).

이 가이드를 따라 설치하면 Docker를 사용할 수 있습니다. 추가 질문 시 문의하세요!