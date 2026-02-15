# ToyProject - 프로젝트 요약

## 개요
STM32H743ZIT6 (Cortex-M7) 기반 임베디드 RTC 시스템.
TM1638 7세그먼트 LED 디스플레이에 실시간 시계를 표시하며,
FreeRTOS 멀티태스킹, Git 기반 자동 버전 관리, CI/CD 파이프라인을 갖춘 프로젝트.

---

## MCU & 클럭
- **MCU:** STM32H743ZIT6 (ARM Cortex-M7)
- **시스템 클럭:** 240 MHz (HSE 25MHz → PLL)
- **I/D Cache:** 활성화
- **툴체인:** IAR Embedded Workbench (EWARMv9+)

---

## 디렉토리 구조

```
ToyProject/
├── Core/Inc/          # HAL 설정 헤더 (main.h, FreeRTOSConfig.h, version.h 등)
├── Core/Src/          # HAL 초기화 (main.c, spi.c, dma.c, tim.c 등)
├── System/            # 커스텀 시스템 레이어
│   ├── sys_main.c     # 메인 태스크 (LED 토글 + RTC 표시)
│   ├── sys_Init.c     # 커널 초기화 (Init 태스크 생성)
│   ├── sys_Interrupt.c # SPI5 DMA 완료 콜백
│   ├── Bsp/           # 보드 지원 (bsp_TM1638.c/.h)
│   └── util/          # 유틸리티 (RTC.c/.h)
├── Drivers/           # CMSIS + STM32H7 HAL 드라이버
├── Middlewares/       # FreeRTOS v10.3.1
├── EWARM/             # IAR 빌드 설정 + 프리빌드 스크립트
├── scripts/           # CI/CD용 버전 생성 스크립트 (Python, Batch)
├── docs/              # 개발 가이드 (Best Practices, Quick Reference)
└── .github/workflows/ # GitHub Actions CI/CD
```

---

## FreeRTOS 태스크 구조

| 태스크 | 우선순위 | 역할 |
|--------|---------|------|
| Init Task | 2 (idle+2) | TM1638 초기화, PWM 설정, Main 태스크 생성 후 자기 삭제 |
| Main Task | 1 (idle+1) | LD1 LED 1초 토글 + TM1638에 RTC 시간 표시 |

- Tick Rate: 1000 Hz (1ms)
- Heap: 15,360 bytes
- Max Priorities: 7

---

## 주요 페리페럴

| 페리페럴 | 용도 | 설정 |
|---------|------|------|
| **SPI5** | TM1638 LED 모듈 | 1-line TX, LSB-first, baud /64 |
| **SPI4** | 범용 SPI | 2-line, baud /2 |
| **DMA1 Stream0** | SPI5 TX | Memory→Peripheral, FIFO, 세마포어 동기화 |
| **TIM2 CH1** | LCD 백라이트 PWM | 1 KHz, 듀티 0-100% |
| **TIM7** | FreeRTOS 틱 | 1ms |
| **RTC** | 실시간 시계 | LSE 32.768 kHz, BCD 포맷 |
| **USART3** | ST-Link 디버그 | 115200 baud |
| **ETH** | 이더넷 (RMII) | 설정됨, 미사용 |
| **USB OTG FS** | USB 디바이스 | 설정됨, 미사용 |

### TM1638 드라이버
- SPI5 + DMA1_Stream0으로 7세그먼트 디스플레이 제어
- 바이너리 세마포어로 DMA 완료 동기화
- STB 핀: GPIO F8 (소프트웨어 제어)
- `SCB_CleanDCache_by_Addr()` 캐시 관리

---

## GPIO 핀 매핑

| 핀 | 이름 | 용도 |
|----|------|------|
| B13 | B1 | 사용자 버튼 (입력) |
| B0 | LD1 | LED1 (출력) |
| E1 | LD2 | LED2 (출력) |
| B14 | LD3 | LED3 (출력) |
| G0 | LCD_DC | LCD Data/Command |
| G1 | LCD_RESET | LCD Reset |
| F8 | SPI_STB | TM1638 Chip Select |

---

## 코딩 컨벤션

- **헤더 가드:** `#pragma once`
- **함수명:** `Module_Function()` (PascalCase) — 예: `TM1638_Init()`, `DisplayRTC()`
- **매크로:** `PREFIX_CONSTANT` (UPPERCASE_SNAKE)
- **전역변수:** `g_` 접두사 또는 모듈 스코프
- **지역변수:** lowercase_snake_case
- **타입:** `Type_t` 접미사
- **HAL 패턴:** `HAL_GPIO_WritePin()`, `HAL_SPI_Transmit()` 등 표준 STM32 HAL 사용
- **ISR 패턴:** `xSemaphoreGiveFromISR()` + `portYIELD_FROM_ISR()`
- **USER CODE 섹션:** CubeMX 재생성 시 보존을 위한 주석 블록

---

## 빌드 & CI/CD

### 로컬 빌드
- IAR Embedded Workbench GUI 또는 CLI (`IarBuild.exe`)
- 프리빌드: `EWARM/prebuild_version.ps1` → `Core/Inc/version.h` 자동 생성

### CI/CD
- **GitHub Actions** (`.github/workflows/build.yml`):
  Push(main/develop), PR(main), 태그(v*) 트리거
  → `scripts/update_version.py`로 버전 생성 → 태그 시 Release 생성
- **GitLab CI** (`.gitlab-ci.yml`):
  build → test → release 스테이지

### 버전 관리
- Git 태그 기반 시맨틱 버전 (`v1.2.3`)
- 자동 생성 `Core/Inc/version.h`: MAJOR/MINOR/PATCH, COMMIT_HASH, BRANCH, TAG, BUILD_DATE

---

## 구현 상태

### 완료
- MCU 초기화 (240 MHz, 캐시)
- FreeRTOS 태스크 구조 (Init → Main)
- TM1638 드라이버 (SPI5 + DMA)
- RTC 시간 표시
- 자동 버전 관리 (3개 스크립트)
- CI/CD (GitHub Actions + GitLab CI)
- 문서화 (Best Practices, Quick Reference)

### 미완료 / TODO
- `InitRTC()` 구현 (현재 빈 스텁)
- ADC 통합
- 이더넷 통신 활용
- USB OTG 활용
- 추가 타이머 패턴

---

## 핵심 파일 경로

| 파일 | 역할 |
|------|------|
| `Core/Src/main.c` | MCU 진입점, 페리페럴 초기화 |
| `System/sys_main.c` | 메인 애플리케이션 루프 |
| `System/sys_Init.c` | 커널 초기화, 태스크 생성 |
| `System/sys_Interrupt.c` | ISR 콜백 라우팅 |
| `System/Bsp/bsp_TM1638.c` | TM1638 드라이버 |
| `System/util/RTC.c` | RTC 읽기 및 표시 |
| `Core/Src/spi.c` | SPI4/5 설정 |
| `Core/Src/dma.c` | DMA1 Stream0 설정 |
| `Core/Inc/FreeRTOSConfig.h` | FreeRTOS 설정 |
| `Core/Inc/version.h` | 자동 생성 버전 정보 |
| `.github/workflows/build.yml` | GitHub Actions 파이프라인 |
