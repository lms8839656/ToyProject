# Code Review Fixes (2026-02-15)

## Purpose
Apply fixes found during the review based on `.claude/project_summary.md`.

## Changes

1. SPI5 DMA alignment fix
- File: `Core/Src/spi.c`
- Change: `DMA_PDATAALIGN_WORD`/`DMA_MDATAALIGN_WORD` -> `DMA_PDATAALIGN_BYTE`/`DMA_MDATAALIGN_BYTE`
- Reason: SPI5 uses 8-bit data, so word alignment can cause incorrect transfers.

2. TM1638 DMA transfer hardening
- File: `System/Bsp/bsp_TM1638.c`
- Changes:
- Validate DMA input (`NULL`, `size == 0`, buffer overflow, semaphore not created).
- Clear stale semaphore state before new transfer.
- Check `HAL_SPI_Transmit_DMA()` return status.
- Abort SPI on DMA timeout via `HAL_SPI_Abort()`.
- Validate callback context (`hspi`, SPI instance, semaphore).
- Validate display API bounds (`position <= 7`, `number <= 9`).
- File: `System/Bsp/bsp_TM1638.h`
- Change: `TM1638_ReceiveByte(uint8_t data)` -> `TM1638_ReceiveByte(void)`.

3. RTC reboot reset fix
- File: `Core/Src/rtc.c`
- Changes:
- Added `RTC_BKP_MAGIC`.
- Check init state with `HAL_RTCEx_BKUPRead()`.
- Only initialize date/time on first boot, then mark with `HAL_RTCEx_BKUPWrite()`.
- Reason: Prevent resetting RTC to 00:00 at every boot.

4. CI version dirty detection fix
- File: `scripts/update_version.py`
- Changes:
- Added `run_git_returncode()`.
- Dirty status now uses exit codes from `git diff --quiet` and `git diff --cached --quiet`.
- Reason: stdout-based check did not detect dirty states reliably.

5. GitLab CI order fix
- File: `.gitlab-ci.yml`
- Changes:
- Install `git` in `.common_setup.before_script` before `git --version`.
- Removed duplicate git install from `build_firmware.script`.
- Reason: avoid failures on `python:3.11-slim`.

6. Documentation and helper script consistency
- File: `README.md`
- Change: MCU line updated to `STM32H743ZIT6 (STM32H7 series)`.
- File: `generate_version.py`
- Change: aligned generated macros with current version schema (`GIT_COMMIT_HASH`, `GIT_BRANCH`, `GIT_TAG`, `FW_VERSION_INFO`).

## Validation
- Ran: `python -m py_compile scripts/update_version.py generate_version.py`
- Ran static checks for updated symbols and settings.

## Note
- Hardware validation (IAR build + board flashing/runtime) was not executed in this session.
