### 사전 준비사항
- **Docker Desktop/Engine** 설치 (6주차에서 완료)
- **웹 브라우저** (Chrome, Firefox 등)
- **작업 디렉토리:** `~/week09`

***

## 실습 환경 설정

### 작업 디렉토리 생성 및 이동
```bash
# week09 디렉토리 생성
mkdir -p ~/week09
cd ~/week09

# MinIO 데이터 저장용 디렉토리 생성
mkdir -p ~/week09/minio_data

# 실습 파일 저장용 디렉토리 생성
mkdir -p ~/week09/test_files
mkdir -p ~/week09/downloaded_files
```

### MinIO 서버 시작
```bash
# week09 디렉토리에서 실행
cd ~/week09

# MinIO 서버 실행 (데이터는 ~/week09/minio_data에 저장)
docker run -d \
  -p 9000:9000 \
  -p 9090:9090 \
  --name minio \
  -v ~/week09/minio_data:/data \
  -e "MINIO_ROOT_USER=admin" \
  -e "MINIO_ROOT_PASSWORD=password123" \
  quay.io/minio/minio server /data --console-address ":9090"

# MinIO 서버 상태 확인
docker ps | grep minio

# 로그 확인
docker logs minio
```

**MinIO 콘솔 접근:**
- URL: http://localhost:9090
- 사용자명: admin
- 암호: password123

***

## 실습 1: MinIO 콘솔을 이용한 오브젝트 스토리지 체험

### 목표
로컬 MinIO 환경에서 S3 호환 오브젝트 스토리지의 기본 기능을 체험한다.

### 1-1. MinIO 콘솔 로그인 및 버킷 생성

**Step 1: MinIO 콘솔 접근**
```bash
# 웹 브라우저에서 접속
http://localhost:9090

# 로그인 정보
사용자명: admin
암호: password123
```

**Step 2: 버킷 생성**
```
1. MinIO 콘솔 로그인 후 좌측 메뉴에서 "Buckets" 클릭
2. "Create Bucket" 버튼 클릭
3. 버킷 이름 입력: "storage-lab" 또는 "storage-lab-[학번]"
4. 기본 설정 그대로 "Create Bucket" 완료
5. 생성된 버킷이 목록에 나타났는지 확인
```

### 1-2. 파일 업로드 및 관리

**Step 1: 테스트 파일 준비**
```bash
# week09 디렉토리로 이동
cd ~/week09/test_files

# 텍스트 파일 생성
cat > test.txt << 'EOF'
This is a test file for MinIO object storage.
Timestamp: $(date)
Student Name: [이름]
EOF

# HTML 파일 생성
cat > sample.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>MinIO Test</title>
    <style>
        body { font-family: Arial; padding: 20px; }
        h1 { color: #4285f4; }
    </style>
</head>
<body>
    <h1>Hello from MinIO</h1>
    <p>This file is stored in local object storage.</p>
    <p>Week 09 Lab - Cloud Storage</p>
</body>
</html>
EOF

# JSON 파일 생성
cat > data.json << 'EOF'
{
  "course": "Cloud Computing",
  "week": 9,
  "topic": "Cloud Storage and Data Management",
  "lab": "MinIO Local Object Storage"
}
EOF

# 생성된 파일 확인
ls -la
```

**Step 2: 파일 업로드**
```
1. MinIO 콘솔에서 생성한 "storage-lab" 버킷 클릭
2. "Upload" 버튼 클릭
3. 다음 파일들 업로드:
   - test.txt
   - sample.html
   - data.json
4. 업로드 진행률 확인
5. 업로드 완료 후 파일 목록 확인
```

**Step 3: 객체 상세 정보 확인**
```
1. 업로드된 파일 중 test.txt 클릭
2. 파일 정보 확인:
   - 파일 이름
   - 파일 크기 (Bytes)
   - 수정 날짜 및 시간
   - ETag (해시값)
3. "Preview" 탭에서 파일 내용 미리보기
4. "Metadata" 탭에서 객체 메타데이터 확인
```

### 1-3. 객체 접근 및 공유

**Step 1: 객체 URL 생성**
```
1. sample.html 파일 클릭
2. "Share" 또는 "Get URL" 옵션 클릭
3. 생성된 URL 복사:
   예: http://localhost:9000/storage-lab/sample.html
```

**Step 2: 브라우저에서 직접 접근**
```bash
# 새 브라우저 탭에서 URL 입력
http://localhost:9000/storage-lab/sample.html

# 결과: HTML 파일이 브라우저에서 렌더링되어 표시됨
```

**Step 3: 객체 다운로드**
```
1. MinIO 콘솔에서 파일 선택
2. "Download" 버튼 클릭
3. ~/week09/downloaded_files/ 디렉토리에 저장
```

```bash
# 다운로드 확인
ls -la ~/week09/downloaded_files/
```

***

## 실습 2: CLI를 이용한 고급 저장소 관리

### 목표
MinIO CLI(mc 명령어)를 사용하여 프로그래매틱한 방식으로 오브젝트 관리 및 메타데이터 작업을 수행한다.

### 2-1. MinIO CLI 설치 및 설정

**Step 1: MinIO CLI 설치**
```bash
# week09 디렉토리로 이동
cd ~/week09

# MinIO CLI (mc) 다운로드
curl -o mc https://dl.min.io/client/mc/release/linux-amd64/mc

# 실행 권한 부여
chmod +x mc

# PATH에 추가 (시스템 전역)
sudo mv mc /usr/local/bin/

# 설치 확인
mc --version
```

**Step 2: MinIO 서버 연결 설정**
```bash
# MinIO 서버 별칭 등록 (로컬 호스트)
mc alias set minio http://localhost:9000 admin password123

# 연결 테스트
mc ls minio

# 예상 출력:
# [2025-11-01 12:15:34 UTC]     0B storage-lab/
```

### 2-2. CLI를 통한 객체 관리

**Step 1: 버킷 내 객체 목록 확인**
```bash
# 버킷의 모든 객체 나열
mc ls minio/storage-lab

# 상세 정보와 함께 나열
mc ls --recursive minio/storage-lab

# 예상 출력:
# [2025-11-01 12:10:20 UTC]   120B test.txt
# [2025-11-01 12:11:05 UTC]   320B sample.html
# [2025-11-01 12:11:25 UTC]   180B data.json
```

**Step 2: 새로운 객체 업로드**
```bash
# week09/test_files 디렉토리로 이동
cd ~/week09/test_files

# 추가 테스트 파일 생성
cat > large-file.txt << 'EOF'
This is a test file with more content.
Line 1: Introduction to Cloud Storage
Line 2: Object Storage Concepts
Line 3: MinIO S3-Compatible Storage
Line 4: Docker-based Local Environment
Line 5: Week 09 Lab Exercise
EOF

# 버킷에 업로드
mc cp large-file.txt minio/storage-lab/

# 폴더 구조로 업로드 (backup 폴더 생성)
mc cp test.txt minio/storage-lab/backup/
mc cp sample.html minio/storage-lab/backup/
mc cp data.json minio/storage-lab/backup/

# 업로드 확인
mc ls -r minio/storage-lab/
```

**Step 3: 객체 메타데이터 확인**
```bash
# 객체의 상세 정보 확인
mc stat minio/storage-lab/test.txt

# 예상 출력:
# Name      : test.txt
# Date      : 2025-11-01 12:10:20 UTC
# Size      : 120B
# ETag      : a4b3c5d6e7f8...
# Type      : file
# Metadata  : Content-Type: text/plain
```

**Step 4: 객체 다운로드**
```bash
# week09/downloaded_files 디렉토리로 이동
cd ~/week09/downloaded_files

# 특정 객체 다운로드
mc cp minio/storage-lab/test.txt ./downloaded-test.txt

# 폴더 전체 다운로드
mc cp --recursive minio/storage-lab/backup/ ./backup/

# 다운로드 확인
ls -la
cat downloaded-test.txt
ls -la backup/
```

### 2-3. 버킷 정책 및 대량 작업

**Step 1: 대량 파일 생성 및 업로드**
```bash
# week09/test_files 디렉토리로 이동
cd ~/week09/test_files

# archive 폴더 생성
mkdir -p archive

# 여러 개의 테스트 파일 생성
for i in {1..5}; do
  cat > archive/archive-test-$i.txt << EOF
Archive test file number $i
Created at: $(date)
Content: Week 09 Lab - Batch Upload Test
EOF
done

# 생성 확인
ls -la archive/

# 폴더 전체 업로드
mc cp --recursive archive/ minio/storage-lab/archive/

# 업로드 결과 확인
mc ls -r minio/storage-lab/archive/
```

**Step 2: 버킷 정보 확인**
```bash
# 버킷 전체 크기 확인
mc du minio/storage-lab

# 예상 출력:
# 2.5KB	minio/storage-lab

# 객체 수 포함 상세 정보
mc ls -r --summarize minio/storage-lab

# 예상 출력:
# Total: 2.5KB
# Objects: 12
```

**Step 3: 객체 검색 및 필터링**
```bash
# 특정 패턴으로 검색
mc find minio/storage-lab --name "*.txt"

# 특정 크기 이상 파일 검색
mc find minio/storage-lab --larger 100B

# 특정 날짜 이후 생성된 파일 검색
mc find minio/storage-lab --newer-than 1h
```

***

## 실습 3: Docker Compose를 이용한 완전한 환경 구성 (선택)

### 목표
MinIO와 웹 애플리케이션을 함께 구성하여 실제 클라우드 환경 시뮬레이션

### 3-1. Docker Compose 설정

**Step 1: docker-compose.yml 생성**
```bash
# week09 디렉토리로 이동
cd ~/week09

# 기존 MinIO 컨테이너 중지 (이미 실행 중인 경우)
docker stop minio
docker rm minio

# docker-compose.yml 파일 생성
cat > docker-compose.yml << 'EOF'
version: '3.8'

services:
  minio:
    image: quay.io/minio/minio:latest
    container_name: minio-server
    ports:
      - "9000:9000"
      - "9090:9090"
    volumes:
      - ./minio_data:/data
    environment:
      MINIO_ROOT_USER: admin
      MINIO_ROOT_PASSWORD: password123
    command: server /data --console-address ":9090"
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:9000/minio/health/live"]
      interval: 30s
      timeout: 20s
      retries: 3
    networks:
      - storage-network

  # 웹 애플리케이션 예제 (Nginx)
  webapp:
    image: nginx:alpine
    container_name: webapp
    ports:
      - "8080:80"
    volumes:
      - ./test_files:/usr/share/nginx/html:ro
    depends_on:
      - minio
    networks:
      - storage-network

networks:
  storage-network:
    driver: bridge

volumes:
  minio_data:
EOF
```

**Step 2: Docker Compose 실행**
```bash
# 서비스 시작
docker-compose up -d

# 로그 확인
docker-compose logs -f

# 실행 중인 컨테이너 확인
docker-compose ps

# 예상 출력:
# NAME          IMAGE                       STATUS
# minio-server  quay.io/minio/minio:latest  Up
# webapp        nginx:alpine                Up
```

**Step 3: 서비스 접근 테스트**
```bash
# MinIO 콘솔
http://localhost:9090

# 웹 애플리케이션
http://localhost:8080
```

***

## 실습 4: 파일 동기화 및 백업 시뮬레이션

### 목표
실제 백업 시나리오를 시뮬레이션하여 데이터 관리 전략 이해

### 4-1. 로컬과 MinIO 간 동기화

**Step 1: 로컬 디렉토리 준비**
```bash
# week09 디렉토리로 이동
cd ~/week09

# 동기화 테스트용 디렉토리 생성
mkdir -p sync_source
cd sync_source

# 여러 파일 생성
for i in {1..3}; do
  cat > file-$i.txt << EOF
Sync test file number $i
Created: $(date)
Week: 09
Topic: Cloud Storage Sync
EOF
done

# 하위 디렉토리 생성
mkdir -p documents images
echo "Document 1" > documents/doc1.txt
echo "Document 2" > documents/doc2.txt
echo "Image metadata" > images/meta.txt

# 생성 확인
tree . || ls -laR
```

**Step 2: 전체 디렉토리 동기화**
```bash
# 로컬 -> MinIO 동기화
mc cp --recursive ~/week09/sync_source/ minio/storage-lab/synced/

# 동기화 결과 확인
mc ls -r minio/storage-lab/synced/

# 예상 출력:
# [date] size file-1.txt
# [date] size file-2.txt
# [date] size file-3.txt
# [date] size documents/doc1.txt
# [date] size documents/doc2.txt
# [date] size images/meta.txt
```

**Step 3: 역방향 동기화 (백업 복원 시뮬레이션)**
```bash
# 복원 디렉토리 생성
mkdir -p ~/week09/sync_restored
cd ~/week09

# MinIO -> 로컬 동기화
mc cp --recursive minio/storage-lab/synced/ ./sync_restored/

# 복원 확인
tree sync_restored/ || ls -laR sync_restored/

# 원본과 복원 파일 비교
diff -r sync_source/ sync_restored/
# 출력 없음 = 완전히 동일
```

### 4-2. 스냅샷과 버전 관리 시뮬레이션

**Step 1: 파일 버전 관리**
```bash
# 버전 관리 테스트 파일 생성
cd ~/week09/test_files

# 버전 1
cat > versioned-file.txt << 'EOF'
Version 1
Initial content
Created: $(date)
EOF

mc cp versioned-file.txt minio/storage-lab/versions/

# 버전 2 (내용 수정)
cat > versioned-file.txt << 'EOF'
Version 2
Updated content - modification 1
Updated: $(date)
EOF

mc cp versioned-file.txt minio/storage-lab/versions/

# 버전 3 (추가 수정)
cat > versioned-file.txt << 'EOF'
Version 3
Updated content - modification 2
Final version
Updated: $(date)
EOF

mc cp versioned-file.txt minio/storage-lab/versions/

# 최종 파일 확인
mc cat minio/storage-lab/versions/versioned-file.txt
```

**Step 2: 백업 시뮬레이션 (타임스탬프 기반)**
```bash
# 백업 디렉토리 생성 (날짜별)
BACKUP_DATE=$(date +%Y%m%d)
echo "백업 날짜: $BACKUP_DATE"

# 전체 버킷 백업
mc cp --recursive minio/storage-lab/ minio/storage-lab-backup-$BACKUP_DATE/

# 백업 확인
mc ls minio/

# 예상 출력:
# [date] storage-lab/
# [date] storage-lab-backup-20251101/
```

***

## 실습 5: S3 API 호환성 테스트 (선택)

### 목표
Python을 이용하여 S3 API의 실제 사용법 학습

### 5-1. Python 환경 설정

**Step 1: Python 스크립트 디렉토리 생성**
```bash
# week09 디렉토리로 이동
cd ~/week09

# Python 스크립트 디렉토리 생성
mkdir -p python_scripts
cd python_scripts

# boto3 설치 (Python S3 클라이언트)
pip install boto3
```

### 5-2. Python 스크립트 작성 및 실행

**Step 1: 기본 S3 작업 스크립트**
```bash
# week09/python_scripts 디렉토리에서
cat > test_minio.py << 'EOF'
import boto3
from datetime import datetime

# MinIO 클라이언트 설정
s3_client = boto3.client(
    's3',
    endpoint_url='http://localhost:9000',
    aws_access_key_id='admin',
    aws_secret_access_key='password123',
    region_name='us-east-1'
)

print("=" * 50)
print("MinIO S3 API Compatibility Test")
print("=" * 50)

# 1. 버킷 목록 조회
print("\n1. Listing all buckets:")
buckets = s3_client.list_buckets()
for bucket in buckets['Buckets']:
    print(f"   - {bucket['Name']} (created: {bucket['CreationDate']})")

# 2. 객체 목록 조회
print("\n2. Listing objects in 'storage-lab' bucket:")
try:
    objects = s3_client.list_objects_v2(Bucket='storage-lab')
    if 'Contents' in objects:
        for obj in objects['Contents']:
            print(f"   - {obj['Key']} ({obj['Size']} bytes)")
    else:
        print("   (No objects found)")
except Exception as e:
    print(f"   Error: {e}")

# 3. 파일 업로드
print("\n3. Uploading a file:")
upload_content = f"Python uploaded file\nTimestamp: {datetime.now()}\nWeek: 09"
try:
    s3_client.put_object(
        Bucket='storage-lab',
        Key='python-uploaded.txt',
        Body=upload_content.encode('utf-8')
    )
    print("   ✓ File uploaded successfully!")
except Exception as e:
    print(f"   Error: {e}")

# 4. 파일 다운로드
print("\n4. Downloading the uploaded file:")
try:
    response = s3_client.get_object(Bucket='storage-lab', Key='python-uploaded.txt')
    content = response['Body'].read().decode('utf-8')
    print(f"   Downloaded content:\n   {content}")
except Exception as e:
    print(f"   Error: {e}")

# 5. 파일 메타데이터 조회
print("\n5. Getting object metadata:")
try:
    metadata = s3_client.head_object(Bucket='storage-lab', Key='python-uploaded.txt')
    print(f"   - Content-Type: {metadata['ContentType']}")
    print(f"   - Content-Length: {metadata['ContentLength']} bytes")
    print(f"   - Last-Modified: {metadata['LastModified']}")
    print(f"   - ETag: {metadata['ETag']}")
except Exception as e:
    print(f"   Error: {e}")

print("\n" + "=" * 50)
print("Test completed!")
print("=" * 50)
EOF
```

**Step 2: 스크립트 실행**
```bash
# Python 스크립트 실행
python test_minio.py

# 예상 출력:
# ==================================================
# MinIO S3 API Compatibility Test
# ==================================================
#
# 1. Listing all buckets:
#    - storage-lab (created: 2025-11-01...)
#
# 2. Listing objects in 'storage-lab' bucket:
#    - test.txt (120 bytes)
#    - sample.html (320 bytes)
#    ...
#
# 3. Uploading a file:
#    ✓ File uploaded successfully!
#
# 4. Downloading the uploaded file:
#    Downloaded content:
#    Python uploaded file
#    Timestamp: 2025-11-01 12:30:45...
#    Week: 09
#
# 5. Getting object metadata:
#    - Content-Type: binary/octet-stream
#    - Content-Length: 85 bytes
#    ...
# ==================================================
```

***

## 실습 정리 및 학습 확인

### 디렉토리 구조 최종 확인
```bash
# week09 디렉토리 구조 확인
cd ~/week09
tree -L 2 . || ls -laR

# 예상 디렉토리 구조:
# ~/week09/
# ├── minio_data/          # MinIO 데이터 저장소
# ├── test_files/          # 업로드용 테스트 파일
# │   ├── test.txt
# │   ├── sample.html
# │   ├── data.json
# │   └── archive/
# ├── downloaded_files/    # 다운로드된 파일
# ├── sync_source/         # 동기화 소스
# ├── sync_restored/       # 복원된 파일
# ├── python_scripts/      # Python 스크립트
# │   └── test_minio.py
# └── docker-compose.yml   # Docker Compose 설정
```

### 실습 후 리소스 정리
```bash
# Docker Compose 사용 시
cd ~/week09
docker-compose down

# 개별 컨테이너 중지
docker stop minio
docker rm minio

# 데이터 보관 (선택사항)
# minio_data 디렉토리는 삭제하지 않고 보관
# 다음 실습 시 재사용 가능

# 완전 정리 (선택사항)
# rm -rf ~/week09/minio_data
```
