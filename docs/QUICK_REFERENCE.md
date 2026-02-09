# 임베디드 개발 빠른 참조 가이드

1분 안에 찾을 수 있는 자주 사용하는 패턴과 명령어 모음

## Git 명령어

```bash
# 태그 생성 및 푸시
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# 브랜치 생성 및 전환
git checkout -b feature/new-sensor

# 커밋 메시지
git commit -m "feat: Add temperature sensor driver"

# 태그 확인
git describe --tags --always

# 특정 태그로 체크아웃
git checkout v1.0.0
```

## IAR 빌드 자동화

```powershell
# PowerShell에서 버전 생성
powershell.exe -ExecutionPolicy Bypass -File "EWARM\prebuild_version.ps1"

# Python으로 버전 생성 (CI/CD용)
python scripts/update_version.py

# 명령줄에서 IAR 빌드
iarbuild EWARM/Project.eww -build Release
```

## 디버깅 매크로

```c
// 빠른 로깅
#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  printf("[INFO ] " fmt "\n", ##__VA_ARGS__)

// Assert
#define ASSERT(x) if(!(x)) { printf("ASSERT: %s:%d\n", __FILE__, __LINE__); while(1); }

// 타이밍 측정
#define PROFILE_START() uint32_t _t = DWT->CYCCNT
#define PROFILE_END()   printf("Cycles: %u\n", DWT->CYCCNT - _t)

// GPIO 토글 (오실로스코프)
#define PULSE() do { DEBUG_PIN_HIGH(); DEBUG_PIN_LOW(); } while(0)
```

## 메모리 최적화

```c
// ROM에 배치 (Flash)
const uint8_t table[256] = { /* ... */ };

// 섹션 지정
__attribute__((section(".ccmram"))) uint8_t fast_buffer[1024];

// 정렬
__attribute__((aligned(4))) uint32_t aligned_var;

// 패킹 (구조체 크기 최소화)
typedef struct __attribute__((packed)) {
    uint8_t  a;
    uint32_t b;
} PackedStruct_t;
```

## 자주 사용하는 HAL

```c
// GPIO
HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

// UART
HAL_UART_Transmit(&huart1, data, len, HAL_MAX_DELAY);
HAL_UART_Receive(&huart1, data, len, 1000);  // 1초 타임아웃

// I2C
HAL_I2C_Mem_Read(&hi2c1, 0x76<<1, 0xD0, 1, data, 1, 1000);
HAL_I2C_Mem_Write(&hi2c1, 0x76<<1, 0xF4, 1, data, 1, 1000);

// Timer
HAL_TIM_Base_Start_IT(&htim2);
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

// DMA
HAL_UART_Receive_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
```

## 일반적인 버그 패턴

```c
// ❌ 버퍼 오버플로우
char buffer[10];
sprintf(buffer, "Long string here");  // 위험!

// ✅ 안전한 버전
snprintf(buffer, sizeof(buffer), "Safe string");

// ❌ Null 체크 누락
void process(uint8_t *data) {
    data[0] = 0;  // 크래시 가능!
}

// ✅ Null 체크
void process(uint8_t *data) {
    if (data == NULL) return;
    data[0] = 0;
}

// ❌ 정수 오버플로우
uint8_t a = 200;
uint8_t b = 100;
uint8_t c = a + b;  // 오버플로우! c = 44

// ✅ 타입 확장
uint16_t c = (uint16_t)a + (uint16_t)b;  // c = 300
```

## 전력 관리

```c
// Sleep 모드
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

// Stop 모드
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

// Standby 모드 (최저 전력)
HAL_PWR_EnterSTANDBYMode();

// 클럭 게이팅
__HAL_RCC_GPIOC_CLK_DISABLE();  // 사용 안 하는 GPIO 비활성화
__HAL_RCC_TIM2_CLK_DISABLE();   // 사용 안 하는 Timer 비활성화
```

## 성능 측정

```c
// DWT 초기화 (한 번만)
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
DWT->CYCCNT = 0;
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

// 사이클 카운트 측정
uint32_t start = DWT->CYCCNT;
MyFunction();
uint32_t cycles = DWT->CYCCNT - start;
float time_us = cycles / (SystemCoreClock / 1000000.0f);
printf("Time: %.2f us (%u cycles)\n", time_us, cycles);
```

## 인터럽트

```c
// 인터럽트 우선순위 설정
HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);  // 최고 우선순위
HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);

// 인터럽트 활성화/비활성화
HAL_NVIC_EnableIRQ(TIM2_IRQn);
HAL_NVIC_DisableIRQ(TIM2_IRQn);

// 크리티컬 섹션
__disable_irq();
// critical code
__enable_irq();

// 또는 HAL 버전
uint32_t primask = __get_PRIMASK();
__disable_irq();
// critical code
__set_PRIMASK(primask);
```

## Makefile 빌드

```bash
# 빌드
make

# 정리
make clean

# 플래시
make flash

# 디버그 정보 포함 빌드
make DEBUG=1

# 특정 타겟
make TARGET=STM32F407
```

## 유용한 온라인 도구

- **Hex to Binary**: https://www.rapidtables.com/convert/number/hex-to-binary.html
- **CRC Calculator**: https://crccalc.com/
- **ASCII Table**: https://www.asciitable.com/
- **Bit Calculator**: http://www.ee.unb.ca/cgi-bin/tervo/calc2.pl

## 단위 변환

```
1 MHz = 1,000,000 Hz
1 ms  = 1,000 us
1 KB  = 1,024 bytes
1 MB  = 1,048,576 bytes

SystemCoreClock = 168 MHz
→ 1 cycle = 5.95 ns
→ 1 us = 168 cycles
→ 1 ms = 168,000 cycles
```

## 일반적인 I2C 주소

```
BME280:  0x76 or 0x77
MPU6050: 0x68 or 0x69
EEPROM:  0x50 ~ 0x57
RTC:     0x68 (DS1307)
```

## 시리얼 디버그 설정

```
Baud Rate:  115200
Data Bits:  8
Parity:     None
Stop Bits:  1
Flow Ctrl:  None
```

## 링커 스크립트 주요 섹션

```
.text     → Flash (코드)
.rodata   → Flash (상수)
.data     → RAM (초기화된 변수)
.bss      → RAM (초기화되지 않은 변수)
.heap     → RAM (동적 할당)
.stack    → RAM (스택)
```

## Watchdog 타이머

```c
// IWDG 초기화 (독립 워치독)
MX_IWDG_Init();

// 주기적으로 리프레시 (타임아웃 전에 호출 필요)
HAL_IWDG_Refresh(&hiwdg);

// 전형적인 타임아웃: 1초 ~ 30초
```

## 부트로더 점프

```c
// 애플리케이션에서 부트로더로 점프
void JumpToBootloader(void) {
    void (*bootloader)(void) = (void (*)(void))(*((uint32_t *)0x1FFF0004));
    __set_MSP(*((uint32_t *)0x1FFF0000));
    bootloader();
}
```

## printf 리다이렉션 (UART)

```c
// retarget.c
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

// 이제 printf() 사용 가능
printf("Hello, World!\n");
```

---

**팁**: 이 가이드를 북마크하고 자주 참조하세요!
