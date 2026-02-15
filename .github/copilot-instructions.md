# ToyProject - AI 개발 어시스턴트 컨텍스트

이 문서는 AI 코딩 어시스턴트(GitHub Copilot 등)가 이 프로젝트를 이해하고 효과적으로 지원하기 위한 컨텍스트 정보입니다.

## 📌 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **프로젝트명** | ToyProject |
| **설명** | STM32 기반 Real Time Clock 임베디드 시스템 |
| **MCU** | STM32H743ZIT6 (Cortex-M7, 480MHz) |
| **IDE** | IAR Embedded Workbench for ARM |
| **RTOS** | FreeRTOS v10.3.1 |
| **HAL** | STM32H7xx HAL Driver |

## 🏗️ 프로젝트 구조

```
ToyProject/
├── Core/               # STM32CubeMX 생성 코어 파일
│   ├── Inc/           # main.h, version.h, peripheral headers
│   └── Src/           # main.c, peripheral sources
├── System/            # 사용자 애플리케이션 코드
│   ├── sys_main.c     # 메인 태스크 진입점
│   ├── sys_Init.c     # 초기화 코드
│   ├── Bsp/           # Board Support Package (TM1638 등)
│   ├── Task/          # FreeRTOS 태스크들
│   └── util/          # 유틸리티 (RTC 등)
├── Drivers/           # CMSIS, HAL 드라이버
├── Middlewares/       # FreeRTOS 미들웨어
├── EWARM/             # IAR 프로젝트 파일
├── scripts/           # 빌드 자동화 스크립트
└── docs/              # 문서
```

## 🔧 하드웨어 구성

### MCU 클럭
- System Clock: 480 MHz (Cortex-M7)
- FreeRTOS Tick: 1000 Hz (1ms)

### 활성화된 페리페럴
| 페리페럴 | 용도 | 비고 |
|---------|------|------|
| **SPI4** | LCD 디스플레이 | PE2(CLK), PE5(MOSI), PE6(MISO) |
| **SPI5** | TM1638 LED/Key | PF7(CLK), PF8(MISO), PF9(MOSI) + PF8(STB) |
| **DMA1** | SPI5_TX | Stream0 |
| **USART3** | 디버그/ST-Link | PD8(TX), PD9(RX) |
| **ETH** | 이더넷 | RMII 모드 |
| **USB_OTG_FS** | USB 디바이스 | PA11(DM), PA12(DP) |
| **RTC** | 실시간 클럭 | LSE 32.768kHz |
| **TIM2** | 범용 타이머 | 채널1 |
| **TIM3** | 범용 타이머 | |
| **TIM7** | FreeRTOS 타임베이스 | |
| **CRC** | CRC 계산 | |

### GPIO 핀
| 핀 | 기능 | 라벨 |
|----|------|------|
| PC13 | 입력 | B1 (User Button) |
| PB0 | 출력 | LD1 (Green LED) |
| PB14 | 출력 | LD3 (Red LED) |
| PE1 | 출력 | LD2 (Yellow LED) |
| PG0 | 출력 | LCD_DC |
| PG1 | 출력 | LCD_RESET |
| PF8 | 출력 | SPI_STB (TM1638) |

## 📦 주요 모듈

### 1. TM1638 LED/Key 드라이버 (`System/Bsp/bsp_TM1638.c`)
- 7세그먼트 LED 디스플레이 컨트롤러
- SPI 통신 (DMA 지원)
- 주요 함수:
  - `TM1638_Init()` - 초기화
  - `TM1638_DisplayNumber()` - 숫자 표시
  - `TM1638_SendByte_DMA()` - DMA 전송

### 2. RTC 유틸리티 (`System/util/RTC.c`)
- 주요 함수:
  - `InitRTC()` - RTC 초기화
  - `GetRTC()` - 현재 시간 읽기
  - `DisplayRTC()` - 시간 표시

### 3. 시스템 메인 (`System/sys_main.c`)
- FreeRTOS 메인 태스크 진입점
- `sys_main_task()` - 1초 주기 LED 토글 및 RTC 표시

## 🛠️ 빌드 및 개발

### 빌드 방법
```powershell
# IAR GUI에서
# 1. EWARM/Project.eww 열기
# 2. F7 빌드, Ctrl+D 디버그

# 명령줄에서
powershell -ExecutionPolicy Bypass -File EWARM\prebuild_version.ps1
iarbuild EWARM/Project.eww -build Release
```

### 버전 관리
- Git 태그 기반 자동 버전 생성
- `Core/Inc/version.h` - 자동 생성 (수정 금지)
- 태그 형식: `v1.2.3`

### 코드 규칙
- HAL 함수 사용: `HAL_GPIO_WritePin()`, `HAL_SPI_Transmit()` 등
- FreeRTOS API: `vTaskDelay()`, `xTaskCreate()` 등
- 에러 처리: `HAL_StatusTypeDef` 반환값 확인

## ⚙️ FreeRTOS 설정

```c
// FreeRTOSConfig.h 주요 설정
#define configCPU_CLOCK_HZ          SystemCoreClock
#define configTICK_RATE_HZ          1000
#define configMAX_PRIORITIES        7
#define configMINIMAL_STACK_SIZE    128
#define configTOTAL_HEAP_SIZE       15360
#define configUSE_PREEMPTION        1
#define configUSE_MUTEXES           1
```

## 📋 TODO 항목

현재 구현 예정 기능:
- [ ] SPI 추가 기능
- [ ] DMA 최적화
- [ ] 타이머 기능 확장
- [ ] RTC 알람 기능
- [ ] 이더넷 통신
- [ ] ADC 기능

## 🎯 AI 어시스턴트 지침

### 코드 작성 시 준수사항
1. **HAL 라이브러리 사용**: STM32 HAL 함수 우선 사용
2. **FreeRTOS 호환**: 블로킹 함수는 `vTaskDelay()` 사용, ISR에서는 `FromISR` 버전 사용
3. **메모리 관리**: 동적 할당 최소화, 스택 크기 고려
4. **인터럽트 안전**: Critical section 적절히 사용
5. **코드 스타일**: 기존 코드 스타일 유지

### 자주 사용하는 패턴
```c
// LED 토글
HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

// 딜레이 (FreeRTOS)
vTaskDelay(pdMS_TO_TICKS(1000));  // 1초 대기

// SPI 전송
HAL_SPI_Transmit(&hspi5, data, size, HAL_MAX_DELAY);

// UART 출력
HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

// RTC 시간 읽기
HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
```

### 파일 생성 위치
| 파일 유형 | 위치 |
|----------|------|
| 새로운 태스크 | `System/Task/` |
| BSP 드라이버 | `System/Bsp/` |
| 유틸리티 함수 | `System/util/` |
| HAL 설정 변경 | `ToyProject.ioc` → CubeMX 재생성 |

### 금지 사항
- ❌ `Core/Inc/`, `Core/Src/` 내 `USER CODE` 영역 외 직접 수정 (CubeMX 덮어쓰기됨)
- ❌ `Drivers/` 폴더 직접 수정
- ❌ `Core/Inc/version.h` 직접 수정 (자동 생성 파일)
- ❌ `while(1)` 무한 루프 내에서 태스크 없이 폴링

### 디버깅 지원
```c
// UART 디버그 출력
#include <stdio.h>
printf("Debug: %d\n", value);  // USART3으로 출력

// LED 상태 표시
HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);   // Green
HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);   // Red
HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);   // Yellow
```

## 📚 참고 문서

- [STM32H743 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0433.pdf)
- [STM32H743 Datasheet](https://www.st.com/resource/en/datasheet/stm32h743zi.pdf)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [EWARM Development Guide](https://www.iar.com/products/architectures/arm/iar-embedded-workbench-for-arm/)

---

*이 문서는 프로젝트 변경 시 업데이트가 필요합니다.*
*최종 업데이트: 2026-02-15*
