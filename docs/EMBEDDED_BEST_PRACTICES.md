# 임베디드 개발 베스트 프랙티스 가이드

임베디드 시스템 개발에서 효율적으로 개발하고 관리하기 위한 종합 가이드입니다.

## 목차

1. [프로젝트 구조](#1-프로젝트-구조)
2. [Git 버전 관리](#2-git-버전-관리)
3. [빌드 시스템](#3-빌드-시스템)
4. [디버깅 전략](#4-디버깅-전략)
5. [메모리 관리](#5-메모리-관리)
6. [테스팅](#6-테스팅)
7. [문서화](#7-문서화)
8. [코드 품질](#8-코드-품질)
9. [CI/CD](#9-cicd)
10. [성능 최적화](#10-성능-최적화)

---

## 1. 프로젝트 구조

### 권장 디렉토리 구조

```
ToyProject/
├── Core/                   # MCU 코어 관련
│   ├── Inc/               # 헤더 파일
│   │   ├── main.h
│   │   ├── version.h      # 자동 생성
│   │   └── config.h       # 설정
│   └── Src/               # 소스 파일
│       └── main.c
├── Drivers/               # HAL, 드라이버
│   ├── STM32F4xx_HAL_Driver/
│   └── CMSIS/
├── Middlewares/           # 미들웨어 (USB, FreeRTOS 등)
├── App/                   # 애플리케이션 로직
│   ├── sensors/          # 센서 모듈
│   ├── communication/    # 통신 모듈
│   └── utilities/        # 유틸리티
├── Tests/                # 단위 테스트
├── docs/                 # 문서
├── scripts/              # 빌드/배포 스크립트
├── EWARM/                # IAR 프로젝트
├── .github/workflows/    # CI/CD
└── README.md
```

### 모듈화 원칙

```c
// 각 모듈은 독립적으로 동작
// 예: sensors/temperature.h

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stdint.h>
#include <stdbool.h>

// Public API
typedef struct {
    float celsius;
    float fahrenheit;
    bool valid;
} Temperature_t;

// 초기화
bool Temperature_Init(void);

// 온도 읽기
Temperature_t Temperature_Read(void);

// 에러 처리
const char* Temperature_GetLastError(void);

#endif // TEMPERATURE_H
```

---

## 2. Git 버전 관리

### 브랜치 전략 (Git Flow 간소화)

```
main            → 릴리즈 (v1.0.0, v1.1.0...)
develop         → 개발 중
feature/xyz     → 새 기능
bugfix/issue-42 → 버그 수정
hotfix/v1.0.1   → 긴급 패치
```

### .gitignore 설정 (필수)

```gitignore
# 빌드 출력
Debug/
Release/
*.hex
*.bin
*.out
*.elf

# IAR 임시 파일
settings/
*.dep
*.pbd
*.bak
Backup*/

# 자동 생성 파일
Core/Inc/version.h

# IDE
.vscode/settings.json  # 개인 설정
.idea/

# 하지만 이것들은 포함:
# - .vscode/launch.json    (디버그 설정)
# - .vscode/tasks.json     (빌드 작업)
# - EWARM/*.ewp            (프로젝트 파일)
```

### 커밋 메시지 규칙

```bash
# 형식: <타입>: <제목>
#
# 타입:
#   feat:     새 기능
#   fix:      버그 수정
#   refactor: 리팩토링
#   test:     테스트 추가
#   docs:     문서 수정
#   perf:     성능 개선
#   chore:    빌드/설정 변경

# 예시:
git commit -m "feat: Add I2C temperature sensor driver

- Implement BME280 sensor support
- Add temperature/humidity reading functions
- Include error handling for I2C failures"

git commit -m "fix: Resolve UART buffer overflow issue

The UART RX buffer was overflowing when receiving
large data packets. Increased buffer size from 64 to 256 bytes
and added overflow detection."
```

### 태그 기반 릴리즈 관리

```bash
# 릴리즈 체크리스트
# 1. 테스트 완료
# 2. 문서 업데이트
# 3. CHANGELOG 작성

# 태그 생성
git tag -a v1.2.0 -m "Release v1.2.0

Features:
- Add CAN bus communication
- Implement power saving mode

Bug Fixes:
- Fix timer overflow issue
- Resolve memory leak in USB handler

Performance:
- Reduce power consumption by 20%"

# 푸시
git push origin v1.2.0
```

---

## 3. 빌드 시스템

### 빌드 설정 분리

```
Debug/      → 디버깅 심볼, 최적화 OFF, assert 활성화
Release/    → 최적화 ON, assert 비활성화, 크기 최소화
Test/       → 호스트에서 단위 테스트 실행
```

### Prebuild 자동화 (이미 구현됨 ✅)

```powershell
# EWARM/prebuild_version.ps1
# 빌드 전 자동으로 버전 정보 생성
```

### 빌드 후 검증

```bash
# Postbuild 스크립트 예시
# 빌드 후 메모리 사용량 확인

#!/bin/bash
MAP_FILE="EWARM/Release/ToyProject.map"

# Flash 사용량
FLASH_USED=$(grep ".text" $MAP_FILE | awk '{print $2}')
FLASH_MAX=524288  # 512KB

# RAM 사용량
RAM_USED=$(grep ".data\|.bss" $MAP_FILE | awk '{sum+=$2} END {print sum}')
RAM_MAX=131072    # 128KB

echo "=== Build Summary ==="
echo "Flash: $FLASH_USED / $FLASH_MAX bytes"
echo "RAM:   $RAM_USED / $RAM_MAX bytes"

if [ $FLASH_USED -gt $FLASH_MAX ]; then
    echo "ERROR: Flash overflow!"
    exit 1
fi
```

---

## 4. 디버깅 전략

### 로깅 시스템 구축

```c
// Core/Inc/debug_log.h

#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <stdio.h>

// 로그 레벨
typedef enum {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARN  = 1,
    LOG_LEVEL_INFO  = 2,
    LOG_LEVEL_DEBUG = 3
} LogLevel_t;

// 전역 로그 레벨 (릴리즈에서는 INFO로 설정)
#ifdef DEBUG
    #define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG
#else
    #define CURRENT_LOG_LEVEL LOG_LEVEL_INFO
#endif

// 로그 매크로
#define LOG_ERROR(fmt, ...) \
    if (CURRENT_LOG_LEVEL >= LOG_LEVEL_ERROR) \
        printf("[ERROR][%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    if (CURRENT_LOG_LEVEL >= LOG_LEVEL_WARN) \
        printf("[WARN ][%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    if (CURRENT_LOG_LEVEL >= LOG_LEVEL_INFO) \
        printf("[INFO ] " fmt "\n", ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    if (CURRENT_LOG_LEVEL >= LOG_LEVEL_DEBUG) \
        printf("[DEBUG][%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif // DEBUG_LOG_H
```

### Assert 사용

```c
// Core/Inc/assert_custom.h

#ifdef DEBUG
    #define ASSERT(expr) \
        do { \
            if (!(expr)) { \
                printf("ASSERT FAILED: %s\n", #expr); \
                printf("File: %s, Line: %d\n", __FILE__, __LINE__); \
                while(1);  /* 무한 루프로 멈춤 */ \
            } \
        } while(0)
#else
    #define ASSERT(expr) ((void)0)
#endif

// 사용 예시
void ProcessData(uint8_t *data, size_t len) {
    ASSERT(data != NULL);
    ASSERT(len > 0 && len <= 256);

    // 실제 처리...
}
```

### 하드웨어 디버깅

```c
// GPIO 토글로 타이밍 측정
#define DEBUG_PIN_HIGH()  HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET)
#define DEBUG_PIN_LOW()   HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET)

void CriticalFunction(void) {
    DEBUG_PIN_HIGH();  // 오실로스코프로 측정 시작

    // 시간이 중요한 코드...

    DEBUG_PIN_LOW();   // 측정 종료
}
```

---

## 5. 메모리 관리

### 스택 사용량 모니터링

```c
// Core/Src/memory_monitor.c

#include <stdint.h>

// 스택 영역을 특정 패턴으로 채우기
#define STACK_FILL_PATTERN 0xA5A5A5A5

extern uint32_t _estack;  // 링커 스크립트에서 정의
extern uint32_t _sstack;

void Stack_FillPattern(void) {
    uint32_t *stack_ptr = &_sstack;
    while (stack_ptr < &_estack) {
        *stack_ptr++ = STACK_FILL_PATTERN;
    }
}

uint32_t Stack_GetUsage(void) {
    uint32_t *stack_ptr = &_sstack;
    uint32_t unused = 0;

    while (*stack_ptr == STACK_FILL_PATTERN && stack_ptr < &_estack) {
        unused += 4;
        stack_ptr++;
    }

    uint32_t total = (uint32_t)(&_estack - &_sstack) * 4;
    return total - unused;
}

float Stack_GetUsagePercent(void) {
    uint32_t total = (uint32_t)(&_estack - &_sstack) * 4;
    uint32_t used = Stack_GetUsage();
    return ((float)used / (float)total) * 100.0f;
}
```

### 힙 사용량 모니터링

```c
// 커스텀 malloc/free 래퍼
typedef struct {
    uint32_t total_allocated;
    uint32_t peak_usage;
    uint32_t current_usage;
    uint32_t allocation_count;
} HeapStats_t;

static HeapStats_t heap_stats = {0};

void* tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr != NULL) {
        heap_stats.current_usage += size;
        heap_stats.total_allocated += size;
        heap_stats.allocation_count++;

        if (heap_stats.current_usage > heap_stats.peak_usage) {
            heap_stats.peak_usage = heap_stats.current_usage;
        }
    }
    return ptr;
}

void tracked_free(void *ptr, size_t size) {
    if (ptr != NULL) {
        heap_stats.current_usage -= size;
        free(ptr);
    }
}

// 통계 출력
void Heap_PrintStats(void) {
    LOG_INFO("Heap Statistics:");
    LOG_INFO("  Current: %u bytes", heap_stats.current_usage);
    LOG_INFO("  Peak:    %u bytes", heap_stats.peak_usage);
    LOG_INFO("  Total:   %u bytes", heap_stats.total_allocated);
    LOG_INFO("  Allocs:  %u", heap_stats.allocation_count);
}
```

### 메모리 풀 사용 (동적 할당 대신)

```c
// 고정 크기 메모리 풀
#define POOL_SIZE 10
#define BUFFER_SIZE 128

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    bool in_use;
} PoolBlock_t;

static PoolBlock_t memory_pool[POOL_SIZE];

void* MemoryPool_Alloc(void) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!memory_pool[i].in_use) {
            memory_pool[i].in_use = true;
            return memory_pool[i].buffer;
        }
    }
    return NULL;  // 풀이 가득 참
}

void MemoryPool_Free(void *ptr) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (memory_pool[i].buffer == ptr) {
            memory_pool[i].in_use = false;
            return;
        }
    }
}
```

---

## 6. 테스팅

### 단위 테스트 (Unity 프레임워크)

```c
// Tests/test_temperature.c

#include "unity.h"
#include "temperature.h"
#include "mock_i2c.h"  // I2C 모킹

void setUp(void) {
    Temperature_Init();
}

void tearDown(void) {
    // 정리 작업
}

void test_temperature_read_success(void) {
    // Given: I2C가 정상적인 데이터 반환
    mock_i2c_expect_read(0x76, 0xFA, 0x12, 0x34);

    // When: 온도 읽기
    Temperature_t temp = Temperature_Read();

    // Then: 올바른 값 반환
    TEST_ASSERT_TRUE(temp.valid);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 25.5, temp.celsius);
}

void test_temperature_read_i2c_error(void) {
    // Given: I2C 에러 발생
    mock_i2c_inject_error();

    // When: 온도 읽기
    Temperature_t temp = Temperature_Read();

    // Then: 에러 처리
    TEST_ASSERT_FALSE(temp.valid);
    TEST_ASSERT_NOT_NULL(Temperature_GetLastError());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_read_success);
    RUN_TEST(test_temperature_read_i2c_error);
    return UNITY_END();
}
```

### 하드웨어 독립 테스트

```c
// HAL을 추상화하여 PC에서 테스트 가능
// hardware_abstraction.h

#ifdef TARGET_HOST
    // PC에서 실행 (테스트용)
    #define HAL_GPIO_WritePin(port, pin, state) mock_gpio_write(pin, state)
    #define HAL_Delay(ms) usleep(ms * 1000)
#else
    // 실제 하드웨어
    #include "stm32f4xx_hal.h"
#endif
```

---

## 7. 문서화

### README.md 구조

```markdown
# Project Name

## 하드웨어
- MCU: STM32F407VET6
- 클럭: 168MHz
- Flash: 512KB
- RAM: 128KB

## 주요 기능
- [ ] UART 통신
- [ ] I2C 센서 (BME280)
- [ ] CAN 버스
- [x] USB CDC

## 빌드 방법
\`\`\`bash
# IAR에서 빌드
# 또는
python scripts/build.py --target release
\`\`\`

## 핀 배치
| Pin | Function | Description |
|-----|----------|-------------|
| PA9 | UART1_TX | Debug UART  |
| PA10| UART1_RX | Debug UART  |
| PB6 | I2C1_SCL | Sensor Bus  |
| PB7 | I2C1_SDA | Sensor Bus  |

## 메모리 맵
| Address    | Size  | Description |
|------------|-------|-------------|
| 0x08000000 | 32KB  | Bootloader  |
| 0x08008000 | 480KB | Application |
```

### 코드 주석 규칙 (Doxygen)

```c
/**
 * @file    temperature.h
 * @brief   Temperature sensor driver
 * @author  Your Name
 * @date    2026-02-10
 * @version 1.0.0
 */

/**
 * @brief Initialize temperature sensor
 *
 * Initializes the I2C interface and configures the BME280 sensor
 * with the following settings:
 * - Oversampling: x16
 * - Mode: Normal
 * - Standby: 0.5ms
 *
 * @return true if initialization successful, false otherwise
 *
 * @note Must be called before Temperature_Read()
 * @warning Blocks for up to 100ms during initialization
 */
bool Temperature_Init(void);
```

---

## 8. 코드 품질

### 코딩 스타일 가이드

```c
// 1. 네이밍 규칙
typedef struct {
    uint32_t value;
} MyStruct_t;              // 타입: PascalCase_t

#define MAX_BUFFER_SIZE 256  // 상수: UPPER_CASE
static uint8_t g_buffer[MAX_BUFFER_SIZE];  // 전역: g_ prefix

void Function_DoSomething(void);  // 함수: PascalCase
void module_internal_func(void);  // 내부 함수: snake_case

// 2. 들여쓰기: 4 spaces (탭 아님)
void example(void) {
    if (condition) {
        // 코드...
    }
}

// 3. 중괄호: K&R 스타일
if (condition) {
    // same line
}

// 4. 매직 넘버 금지
#define TIMEOUT_MS 1000
HAL_Delay(TIMEOUT_MS);  // Good
// HAL_Delay(1000);     // Bad
```

### 정적 분석 (PC-Lint, Cppcheck)

```bash
# Cppcheck 실행
cppcheck --enable=all --inconclusive Core/Src/

# 일반적인 경고
# - 사용하지 않는 변수
# - Null 포인터 역참조
# - 메모리 누수
# - 버퍼 오버플로우
```

### 코드 리뷰 체크리스트

```markdown
## Code Review Checklist

### 기능
- [ ] 요구사항을 충족하는가?
- [ ] 에지 케이스를 처리하는가?
- [ ] 에러 처리가 적절한가?

### 안전성
- [ ] Null 포인터 체크가 있는가?
- [ ] 버퍼 오버플로우 가능성은 없는가?
- [ ] 정수 오버플로우 가능성은 없는가?

### 성능
- [ ] 불필요한 메모리 할당은 없는가?
- [ ] 최적화가 필요한 부분은 없는가?

### 가독성
- [ ] 코드가 이해하기 쉬운가?
- [ ] 주석이 충분한가?
- [ ] 네이밍이 명확한가?
```

---

## 9. CI/CD

### GitHub Actions 워크플로우

```yaml
# .github/workflows/ci.yml
name: Continuous Integration

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Setup Python
        uses: actions/setup-python@v5
        with:
          python-version: '3.x'

      - name: Generate version.h
        run: python scripts/update_version.py

      - name: Run static analysis
        run: |
          sudo apt-get install -y cppcheck
          cppcheck --enable=all --error-exitcode=1 Core/Src/

      - name: Run unit tests
        run: |
          cd Tests
          make test

      # IAR 빌드 (라이선스 필요)
      # - name: Build firmware
      #   run: iarbuild EWARM/Project.eww -build Release

  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Check code style
        run: |
          # clang-format 체크
          find Core -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror
```

---

## 10. 성능 최적화

### 프로파일링

```c
// 실행 시간 측정
#include "core_cm4.h"  // ARM Cortex-M4

#define PROFILE_START() \
    uint32_t start_cycles = DWT->CYCCNT

#define PROFILE_END(name) \
    do { \
        uint32_t end_cycles = DWT->CYCCNT; \
        uint32_t elapsed = end_cycles - start_cycles; \
        float time_us = (float)elapsed / (SystemCoreClock / 1000000.0f); \
        LOG_INFO("%s: %.2f us (%u cycles)", name, time_us, elapsed); \
    } while(0)

// 사용 예시
void MyFunction(void) {
    PROFILE_START();

    // 측정할 코드...

    PROFILE_END("MyFunction");
}
```

### 전력 관리

```c
// 저전력 모드 진입
void EnterLowPowerMode(void) {
    // 1. 사용하지 않는 페리페럴 비활성화
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_TIM2_CLK_DISABLE();

    // 2. 저전력 모드 설정
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    // 3. 깨어난 후 재활성화
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
}
```

### 최적화 팁

```c
// 1. const 사용으로 ROM에 배치
const uint8_t lookup_table[256] = { /* ... */ };

// 2. inline 함수 (작은 함수)
static inline uint32_t min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

// 3. 비트 연산 활용
// 나눗셈 대신 시프트
uint32_t div_by_8 = value >> 3;  // value / 8

// 4. 루프 언롤링 (컴파일러가 자동으로 하기도 함)
for (int i = 0; i < 4; i++) {
    buffer[i] = 0;
}
// vs
buffer[0] = 0;
buffer[1] = 0;
buffer[2] = 0;
buffer[3] = 0;
```

---

## 추가 리소스

### 유용한 도구

| 도구 | 용도 | 링크 |
|------|------|------|
| STM32CubeMX | 초기 설정 생성 | https://www.st.com/stm32cubemx |
| Segger Ozone | 디버거 | https://www.segger.com/ozone |
| PuTTY | 시리얼 터미널 | https://www.putty.org/ |
| Wireshark | 네트워크 분석 | https://www.wireshark.org/ |

### 추천 라이브러리

- **FreeRTOS**: RTOS
- **lwIP**: TCP/IP 스택
- **FatFs**: 파일 시스템
- **Unity**: 단위 테스트
- **Embedded Artistry**: 유틸리티

### 학습 자료

- [Embedded Artistry Blog](https://embeddedartistry.com/)
- [Interrupt Blog](https://interrupt.memfault.com/)
- [STM32 Community](https://community.st.com/)

---

## 프로젝트별 체크리스트

### 새 프로젝트 시작 시

- [ ] Git 저장소 초기화
- [ ] .gitignore 설정
- [ ] 디렉토리 구조 생성
- [ ] README.md 작성
- [ ] 버전 자동화 스크립트 설정
- [ ] CI/CD 파이프라인 구축
- [ ] 코딩 스타일 가이드 정의

### 릴리즈 전

- [ ] 모든 테스트 통과
- [ ] 메모리 사용량 확인
- [ ] 전력 소비 측정
- [ ] 문서 업데이트
- [ ] CHANGELOG 작성
- [ ] Git 태그 생성
- [ ] 릴리즈 노트 작성

---

**작성일**: 2026-02-10
**버전**: 1.0.0
**작성자**: Claude (with user guidance)
