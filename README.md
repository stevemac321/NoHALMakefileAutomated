
```markdown
# 🚫 No HAL, No Libc: True STM32 BareBones Project

This project is a true **bare-metal STM32** firmware development setup. It targets the **STM32F401RE**, but can be adapted to other STM32 boards (see below). The goal: **zero HAL**, **no libc**, and minimal dependencies — just Make, a cross-compiler, OpenOCD, and a Python script to automate everything.

---

## ⚙️ Tools Required

Install these on your system:

- `arm-none-eabi-gcc` — ARM cross-compiler
- `make`
- `openocd` — for flashing and debug bridge
- `gdb-multiarch` — GDB with ARM support
- `screen` — for monitoring UART (`/dev/ttyACM0`)
- `python3` — for automation script
- `st-flash` (optional — alternative to OpenOCD flashing)

---

## 🗂️ Directory Structure

```

BareBones/
├── Makefile           # Build BareBones.elf and BareBones.bin
├── BareBones.elf      # Output ELF (debuggable)
├── BareBones.bin      # Raw binary for flashing
├── gdbscript          # Optional GDB script: break main, continue, etc.
├── automate.py        # Automates build, flash, GDB debug session
├── main.c             # Your bare-metal main (no HAL, no stdlib)
├── startup\_stm32f401xe.s
├── linker.ld
└── syscalls.c         # Provides minimal syscall stubs (\_write, \_sbrk, etc.)

````

---

## 🚀 One-Step Workflow

In one terminal:

```bash
./automate.py clean build flashrun
````

What it does:

1. Runs `make clean`
2. Builds with `make` (produces ELF + BIN)
3. Flashes with OpenOCD
4. Starts OpenOCD + GDB in one shell (using `gdbscript`)
5. Cleanly exits GDB and kills OpenOCD

---

## 📡 Monitor UART

Open a second terminal:

```bash
screen /dev/ttyACM0 115200
```

This shows the output from your MCU via UART.

---

## 🔁 Workflow Summary

**Terminal 1**:

```bash
./automate.py clean build flashrun
```

**Terminal 2**:

```bash
screen /dev/ttyACM0 115200
```

Repeat the automation script after each code change. Reconnect screen after flashing.

---

## ❌ How to Kill `screen`

**Inside screen**:
Press `Ctrl-A`, then `K`, then `y` to confirm.

**Outside screen**:

```bash
screen -ls         # Get session name
screen -X -S <name> quit
```

---

## 🧩 Porting to a Different STM32

You can reuse this setup with any STM32 chip — here's how.

### ✅ Steps

1. **Generate Initial Project with STM32CubeIDE**

   * Select your board or chip (e.g. STM32F411, STM32F103).
   * Accept default settings or increase stack/heap if needed.

2. **Generate the Project**

   * STM32Cube will create:

     * CMSIS startup and headers
     * `stm32fxxx.h`, `system_stm32fxxx.c`
     * A linker script
     * HAL and driver code (you’ll delete this)

3. **Strip It Down**

   * Remove all `HAL_*` calls from `main.c`
   * Delete HAL folders:

     ```
     Drivers/STM32F4xx_HAL_Driver/
     ```

4. **Add Bare Essentials**

   * Copy `syscalls.c` from this repo (for `_write`, `_sbrk`, etc.)
   * Add `sysmem.c` if needed by your build system
   * Confirm it builds **without any HAL**

5. **Build with Makefile**

   * Extract from CubeIDE:

     * Linker script (`.ld`)
     * CMSIS startup file
     * `stm32fxxx.h`
     * Preprocessor defines (from build log or `.cproject`)
   * Adapt the Makefile to your chip and source layout

6. **Remove newlib-nano (optional)**

   * Remove `--specs=nano.specs`
   * Add `-nostdlib` to `CFLAGS` or `LDFLAGS`
   * Provide stubs:

     * `__libc_init_array()`
     * `_exit()`, `_sbrk()`, etc.

---

## 🧱 Minimal File Set for Any STM32 Board

You'll need:

* `startup_stm32fxxx.s` — vector table and reset handler
* `stm32fxxx.h` — CMSIS device header
* `system_stm32fxxx.c` — sets up clocks (optional)
* `linker.ld` — chip-specific memory layout
* `main.c` — your logic, no HAL
* `syscalls.c` — for `_write`, `_sbrk`, etc.
* `Makefile` — adapted to your build
* `automate.py` — for flashing/debugging automation

---

## 📝 Notes

* GDB is started with `gdb-multiarch` and connects to OpenOCD.
* Output is printed via `USART2` on `/dev/ttyACM0` at 115200 baud.
* `syscalls.c` can be stripped further if you're not using `printf()` or malloc.
* Stubbing out `__libc_init_array()` is required if you disable all libc.

---

## ⏳ To-Do

* [ ] Optional: Integrate UART monitoring into `automate.py`
* [ ] Optional: Parse UART output for test results
* [ ] Handle OpenOCD/GDB timeouts or failures gracefully

---

## 📄 License

GPL v2

