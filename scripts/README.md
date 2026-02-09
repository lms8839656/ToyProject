# Build Scripts - 버전 자동화

IAR Embedded Workbench에서 빌드 시 자동으로 버전 정보를 생성하는 스크립트입니다.

## 📁 파일 설명

- `update_version.bat` - Windows 배치 스크립트
- `update_version.py` - Python 스크립트 (크로스 플랫폼)
- **실제 사용 중**: `../EWARM/prebuild_version.ps1` - PowerShell 스크립트 (IAR 로컬 빌드용)
- `README.md` - 이 파일

## 🎯 환경별 선택 가이드

| 환경 | 추천 스크립트 | 이유 |
|------|-------------|------|
| **Windows 로컬 (IAR)** | `EWARM/prebuild_version.ps1` ⭐ | Windows 기본 설치, 빠름, 네이티브 |
| **GitHub Actions** | `scripts/update_version.py` ⭐ | Python 기본 제공, 크로스 플랫폼 |
| **GitLab CI** | `scripts/update_version.py` ⭐ | Docker 이미지에 포함 용이 |
| **Linux/macOS** | `scripts/update_version.py` ⭐ | PowerShell 미설치 환경 |
| **간단한 빌드** | `scripts/update_version.bat` | 레거시 환경, 단순함 |

### PowerShell vs Python 선택 기준

**PowerShell을 사용하세요 (현재 구성):**
- ✅ Windows 로컬에서 IAR로 개발
- ✅ 추가 설치 없이 바로 사용
- ✅ 빠른 실행 속도 필요
- ✅ Windows 네이티브 환경

**Python을 사용하세요:**
- ✅ CI/CD 파이프라인 (GitHub Actions, GitLab CI)
- ✅ 리눅스/맥 환경에서 빌드
- ✅ Docker 컨테이너에서 빌드
- ✅ 크로스 플랫폼 지원 필요

## ✨ 기능

빌드할 때마다 `Core/Inc/version.h` 파일을 자동 생성하여 다음 정보를 포함합니다:

| 항목 | 설명 | 예시 |
|------|------|------|
| 펌웨어 버전 | Git 태그에서 추출 | v1.2.3 |
| 빌드 날짜/시간 | 빌드 실행 시점 | 2026-02-10 14:30:45 |
| Git 커밋 해시 | 현재 커밋 (short) | abc1234 |
| Git 브랜치 | 현재 브랜치 이름 | main |
| Dirty 플래그 | 미커밋 변경사항 여부 | -dirty |

## 🔧 IAR 설정 방법

### 1단계: Prebuild Action 추가

1. IAR Embedded Workbench에서 프로젝트 열기
2. 메뉴: **Project** → **Options** (단축키: `Alt+F7`)
3. 왼쪽 트리: **Build Actions** 선택
4. **Pre-build command line** 섹션에 입력:

**PowerShell 스크립트 사용 (권장 - 현재 구성):**
```
powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\prebuild_version.ps1"
```

**또는 Python (CI/CD 호환):**
```
python $PROJ_DIR$\..\scripts\update_version.py
```

**또는 Batch (레거시):**
```
$PROJ_DIR$\..\scripts\update_version.bat
```

5. **OK** 버튼 클릭

### 2단계: .gitignore 업데이트

`version.h`는 자동 생성되므로 Git에서 제외합니다:

```bash
# .gitignore에 추가
Core/Inc/version.h
```

### 3단계: 코드에서 사용

**main.c 예시:**

```c
#include "version.h"
#include <stdio.h>

void printFirmwareInfo(void) {
    printf("========================================\n");
    printf("  Firmware Information\n");
    printf("========================================\n");
    printf("Version    : %s\n", FW_VERSION_STRING);
    printf("Build Date : %s\n", FW_BUILD_DATE);
    printf("Build Time : %s\n", FW_BUILD_TIME);
    printf("Git Commit : %s\n", GIT_COMMIT_HASH);
    printf("Git Branch : %s\n", GIT_BRANCH);
    printf("Full Info  : %s\n", FW_VERSION_INFO);
    printf("========================================\n");
}

int main(void) {
    // 초기화 코드...

    printFirmwareInfo();  // 펌웨어 정보 출력

    // 메인 루프...
}
```

## 🏷️ Git 태그를 이용한 버전 관리

### 태그 생성 (Semantic Versioning)

```bash
# 1. 개발 완료 후 릴리즈 준비
git checkout main
git pull origin main

# 2. 태그 생성 (annotated tag - 권장)
git tag -a v1.0.0 -m "Release v1.0.0

주요 변경사항:
- 기능: UART 통신 모듈 추가
- 기능: I2C 센서 드라이버 구현
- 수정: Timer 오버플로우 버그 패치
- 개선: 전력 소모 20% 감소"

# 3. 태그 푸시
git push origin v1.0.0
git push origin main
```

### 버전 번호 규칙

```
v1.2.3
│ │ └─ PATCH: 버그 수정
│ └─── MINOR: 기능 추가 (하위 호환)
└───── MAJOR: 주요 변경 (호환성 깨짐)
```

### 태그 관리 명령어

```bash
# 모든 태그 확인
git tag

# 특정 패턴의 태그 확인
git tag -l "v1.*"

# 태그 상세 정보 확인
git show v1.0.0

# 태그로 체크아웃 (이전 버전 확인)
git checkout v1.0.0

# 태그 삭제 (로컬)
git tag -d v1.0.0

# 태그 삭제 (원격)
git push origin --delete v1.0.0

# 특정 태그 이후 변경사항 확인
git log v1.0.0..HEAD --oneline

# 두 태그 사이의 차이 확인
git diff v1.0.0 v1.1.0
```

## 📋 릴리즈 체크리스트

릴리즈 전에 다음 사항을 확인하세요:

```markdown
## Release v1.0.0 체크리스트

### 코드
- [ ] 모든 기능 구현 완료
- [ ] 버그 수정 완료
- [ ] 코드 리뷰 완료
- [ ] 주석 및 문서화 완료

### 테스트
- [ ] 단위 테스트 통과
- [ ] 통합 테스트 통과
- [ ] 하드웨어 실제 테스트 완료
- [ ] 장시간 안정성 테스트

### 빌드
- [ ] IAR Release 모드 빌드 성공
- [ ] 경고(Warning) 없음 확인
- [ ] 메모리 사용량 확인 (Flash/RAM)
- [ ] .hex/.bin 파일 생성 확인

### 버전 관리
- [ ] version.h 자동 생성 확인
- [ ] Git 태그 생성 (v1.0.0)
- [ ] 릴리즈 노트 작성
- [ ] 변경 이력(CHANGELOG) 업데이트

### 배포
- [ ] .hex 파일 백업
- [ ] GitHub Release 생성
- [ ] 팀원들에게 공지
```

## 🚀 수동 실행 (테스트용)

```bash
# Windows 배치 스크립트
cd ToyProject
scripts\update_version.bat

# Python 스크립트
python scripts/update_version.py

# 생성된 파일 확인
cat Core/Inc/version.h
```

## 🐛 문제 해결

### 문제: Git 정보를 찾을 수 없음

```
Error: git is not recognized as an internal or external command
```

**해결:**
1. Git이 설치되어 있는지 확인
2. Git이 시스템 PATH에 추가되어 있는지 확인
3. 명령 프롬프트에서 `git --version` 실행 테스트

### 문제: 빌드 시 스크립트가 실행되지 않음

**해결:**
1. IAR 프로젝트 설정 → Build Actions 확인
2. 스크립트 경로가 올바른지 확인 (`$PROJ_DIR$` 매크로 사용)
3. 스크립트 파일에 실행 권한이 있는지 확인

### 문제: Python을 찾을 수 없음

```
'python' is not recognized as an internal or external command
```

**해결:**
1. Python 3.x 설치 (https://www.python.org/)
2. 설치 시 "Add Python to PATH" 옵션 선택
3. 또는 전체 경로 사용:
   ```
   C:\Python39\python.exe $PROJ_DIR$\..\scripts\update_version.py
   ```
4. 또는 Windows 배치 스크립트 사용

### 문제: 태그가 없어서 v0.0.0으로 표시됨

**해결:**
```bash
# 첫 번째 태그 생성
git tag -a v1.0.0 -m "Initial release"
git push origin v1.0.0
```

## 📄 생성되는 version.h 예시

```c
#ifndef VERSION_H
#define VERSION_H

// Auto-generated by prebuild script - DO NOT EDIT MANUALLY
// Generated on: 2026-02-10 14:35:22

// Firmware Version
#define FW_MAJOR_VERSION    1
#define FW_MINOR_VERSION    2
#define FW_PATCH_VERSION    3

// Version String
#define FW_VERSION_STRING   "v1.2.3"

// Build Information
#define FW_BUILD_DATE       "2026-02-10"
#define FW_BUILD_TIME       "14:35:22"
#define FW_BUILD_TIMESTAMP  1739178922

// Git Information
#define GIT_COMMIT_HASH     "712c9ae"
#define GIT_BRANCH          "main"
#define GIT_TAG             "v1.2.3"

// Helper macros
#define FW_VERSION_FULL     "1.2.3"
#define FW_VERSION_INFO     "v1.2.3 (712c9ae)"

#endif // VERSION_H
```

## 🔄 CI/CD 통합

### 왜 CI/CD에서는 Python을 사용하나요?

CI/CD 환경 (GitHub Actions, GitLab CI, Jenkins 등)에서는:
- ❌ PowerShell이 기본 설치되지 않은 경우가 많음
- ✅ Python은 대부분의 CI 환경에 기본 제공
- ✅ Linux/Docker 컨테이너에서 실행 가능
- ✅ 크로스 플랫폼 호환성

### GitHub Actions 예시

`.github/workflows/build.yml` 파일이 이미 생성되어 있습니다:

```yaml
- name: Setup Python
  uses: actions/setup-python@v5
  with:
    python-version: '3.x'

- name: Generate version.h (Python - CI용)
  run: python scripts/update_version.py

- name: Build with IAR
  run: iarbuild EWARM/Project.eww -build Release
```

### GitLab CI 예시

`.gitlab-ci.yml` 파일이 이미 생성되어 있습니다:

```yaml
build_firmware:
  image: python:3.11-slim
  script:
    - apt-get update && apt-get install -y git
    - python scripts/update_version.py
    - cat Core/Inc/version.h
```

### 로컬 vs CI/CD 전략

```
로컬 개발 (IAR)
    ↓
EWARM/prebuild_version.ps1 (PowerShell) - 빠르고 간단
    ↓
빌드 완료

CI/CD (GitHub/GitLab)
    ↓
scripts/update_version.py (Python) - 크로스 플랫폼
    ↓
자동 빌드 및 릴리즈
```

## 📚 참고 자료

- [Semantic Versioning](https://semver.org/)
- [Git Tagging](https://git-scm.com/book/en/v2/Git-Basics-Tagging)
- [IAR Build Actions](https://www.iar.com/)
