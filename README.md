# ToyProject
Real Time Clock Embedded System

## 📋 프로젝트 개요

STM32 기반 Real Time Clock 임베디드 시스템 프로젝트

## 🔧 하드웨어

- **MCU**: STM32H743ZIT6 (STM32H7 series)
- **개발 환경**: IAR Embedded Workbench
- **디버거**: ST-Link

## 🚀 빌드 및 실행

### IAR에서 빌드
1. `EWARM/Project.eww` 열기
2. F7 키로 빌드
3. Ctrl+D로 디버그 시작

### 명령줄에서 빌드
```bash
# PowerShell에서 버전 생성
powershell -ExecutionPolicy Bypass -File EWARM\prebuild_version.ps1

# IAR 빌드
iarbuild EWARM/Project.eww -build Release
```

## 📚 문서

- **[임베디드 개발 베스트 프랙티스](docs/EMBEDDED_BEST_PRACTICES.md)** - 종합 개발 가이드
- **[빠른 참조 가이드](docs/QUICK_REFERENCE.md)** - 자주 사용하는 패턴과 명령어
- **[빌드 스크립트 가이드](scripts/README.md)** - 버전 자동화 및 빌드 설정

## 🏷️ 버전 관리

이 프로젝트는 Git 태그 기반 자동 버전 관리를 사용합니다.

```bash
# 릴리즈 태그 생성
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# 빌드 시 version.h 자동 생성
```

## 📂 프로젝트 구조

```
ToyProject/
├── Core/               # MCU 코어 코드
│   ├── Inc/           # 헤더 파일
│   └── Src/           # 소스 파일
├── Drivers/           # HAL 드라이버
├── EWARM/             # IAR 프로젝트
├── scripts/           # 빌드 자동화 스크립트
├── docs/              # 문서
└── .github/workflows/ # CI/CD
```

## 🛠️ 주요 기능

- [ ] Real Time Clock
- [ ] 버전 자동 관리 ✅
- [ ] CI/CD 파이프라인 ✅

## 📝 개발 가이드

### 새 기능 추가
```bash
git checkout -b feature/new-feature
# 개발...
git commit -m "feat: Add new feature"
git push origin feature/new-feature
```

### 버그 수정
```bash
git checkout -b bugfix/issue-42
# 수정...
git commit -m "fix: Resolve issue #42"
```

## 🤝 기여

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'feat: Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 라이선스

[라이선스 정보 추가 필요]

## 📞 연락처

프로젝트 관리자 - [이메일 주소]
