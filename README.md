```markdown
# STM32 BareBones Project Automation (this one is specific for STM32F401RE, see below for porting to a different STM)

This project demonstrates a streamlined workflow for building, flashing, debugging, and monitoring a bare-metal STM32 firmware project using a Python script. It targets the STM32F401RE microcontroller and automates the typical development loop. See

---

## 🛠️ Requirements

Install the following tools:

- `arm-none-eabi-gcc` (or other suitable cross-compiler)
- `make`
- `openocd`
- `gdb-multiarch`
- `stlink-tools` (`st-flash`)
- `screen` (for UART monitoring)
- Python 3

Optional but helpful:

- A `.gdb` script to automate breakpoints, stepping, or running
- A Makefile that produces:
  - `BareBones.elf`
  - `BareBones.bin`

---

## 📂 Directory Layout

```

BareBones/
├── Makefile
├── BareBones.elf
├── BareBones.bin
├── gdbscript         # GDB automation commands (e.g., break main, continue, etc.)
├── automate.py       # Main Python script to control build/flash/debug
└── (source files...)

````

---

## 🚀 Usage

Run the automation script:

```bash
./automate.py clean build flashrun
````

This will:

1. Clean the project with `make clean`
2. Build it with `make`
3. Flash `BareBones.bin` to the STM32
4. Start OpenOCD and GDB using `BareBones.elf` and the `gdbscript`
5. Kill OpenOCD when GDB completes

You should run this in one terminal shell.

In **another terminal**, run:

```bash
screen /dev/ttyACM0 115200
```

This allows you to monitor UART output from the STM32.

---

## 🔁 Typical Workflow

In two terminals:

* **Shell 1**: Run `./automate.py clean build flashrun`
* **Shell 2**: Run `screen /dev/ttyACM0 115200` (or reopen after each reset)

You can rerun `automate.py` as needed for each test iteration.

---

## ❌ Killing Screen

To close the UART monitoring session:

### From inside screen:

1. Press `Ctrl-A`, then `K`
2. Confirm with `y`

### From outside screen:

```bash
screen -ls
screen -X -S <session_name> quit
```

---

## 🧪 Automation Goals

This setup supports a testing pipeline where each test:

* Builds and flashes a firmware image
* Runs automatically via GDB control (e.g., breaking on a `dummy()` function)
* Prints results over UART

You can use this pattern for automating embedded test suites like your MLibs framework.

---
---

## 🔧 Porting to a Different STM32 Board

If you're using a board other than the STM32F401RE, you can bootstrap support using STM32CubeIDE to generate the low-level configuration files, then transition to a minimal (non-HAL) setup. Here's a streamlined workflow:

### 🧱 Step-by-Step

1. **Create a New STM32CubeIDE Project**
   - Select your board (e.g. Nucleo-F411RE or STM32F103C8T6).
   - Choose an empty project or basic template.
   - Use default settings, but you may want to tweak heap/stack sizes now or later in the linker script.

2. **Generate the Code**
   - Let STM32Cube generate the initial project files.
   - This gives you:
     - The correct preprocessor defines
     - A linker script
     - The CMSIS startup code and `stm32fxxx.h` headers
     - HAL source files (which we will delete)

3. **Remove HAL**
   - Delete the HAL initialization code from `main.c` (e.g., `HAL_Init()`, `SystemClock_Config()`).
   - Delete all HAL directories:
     ```
     Drivers/STM32F4xx_HAL_Driver/
     ```

4. **Add Minimal Support**
   - Copy `syscalls.c` from this repo into your project. This provides stubs for `_write`, `_sbrk`, etc. for minimal libc support.
   - Optionally copy `sysmem.c` if your IDE setup expects it (depends on how the newlib nano is configured). I have _write in main.c.

5. **Build in STM32CubeIDE**
   - Ensure the project still compiles cleanly with only CMSIS and no HAL.
   - If you see `_write` or `sbrk` undefined errors, check `syscalls.c` is included.

6. **Try Building with Makefile**
   - Use your working IDE build to extract:
     - The correct `*.ld` linker script
     - The `stm32fxxx.h` header
     - The required preprocessor defines (from the IDE build command or `.cproject`)
   - Copy or adapt these into your `BareBones` template project.

7. **Note on `nano` Lib**
   - This template uses **newlib-nano**, which is a minimal C library.
   - If you strip out `nano`, you may need to define stubs like `__libc_init_array()` or `__initialize_hardware_early()` manually.

---

## 📁 Summary: Files You’ll Need

From the generated IDE project:

- ✅ `startup_stm32fxxx.s` — startup file with vector table
- ✅ `stm32fxxx.h` — CMSIS device header for your MCU
- ✅ `system_stm32fxxx.c` — CMSIS system init file
- ✅ `*.ld` — linker script
- ✅ Your own `main.c` or `main.cpp` with all HAL stripped out
- ✅ `syscalls.c` — minimal syscall support
- ✅ `Makefile` — adapted from the BareBones template

You can now use your board with this same `automate.py` workflow for testing and flashing.

---


## 📝 To-Do

* [ ] Integrate UART reading into Python (optional)
* [ ] Add test result parsing over serial
* [ ] Handle GDB timeouts gracefully

---

## 📄 License

GPL v.2
```
