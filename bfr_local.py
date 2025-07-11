#!/usr/bin/env python3
import os
import subprocess
import sys
import time

# Paths
BINFILE = './BareBones.bin'
ELFFILE = './BareBones.elf'
GDBSCRIPT = os.path.join(os.environ.get("MLIBS_ROOT", "."), "gdbscript")

def run_cmd(cmd, cwd=None, wait=True):
    proc = subprocess.Popen(cmd, cwd=cwd)
    if wait:
        proc.wait()
    return proc

def clean():
    run_cmd(['make', 'clean'])

def build():
    run_cmd(['make'])

def flash():
    run_cmd(['st-flash', 'write', BINFILE, '0x08000000'])

def flash_and_run():
    # Flash firmware
    flash()
    time.sleep(0.3)  # give st-flash some time

    # Start OpenOCD (in background)
    openocd = subprocess.Popen(
        ['openocd', '-f', 'interface/stlink.cfg', '-f', 'target/stm32f4x.cfg']
    )

    # Wait for OpenOCD to be ready (you can replace with log-based check)
    time.sleep(1.0)

    # Run GDB with script
    if not os.path.exists(GDBSCRIPT):
        print(f"Error: GDB script '{GDBSCRIPT}' not found.")
        openocd.kill()
        return

    try:
        gdb = subprocess.Popen([
            'gdb-multiarch', ELFFILE,
            '--batch', '--command=' + GDBSCRIPT
        ])
        gdb.wait()
    finally:
        time.sleep(0.5)  # Give UART time to flush
        openocd.terminate()
        openocd.wait()

def print_usage():
    print("Usage: ./bfr_local.py [clean] [build] [flash] [flashrun]")

# ---------------------- Main --------------------------------
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_usage()
        sys.exit(1)

    for cmd in sys.argv[1:]:
        if cmd == 'clean':
            clean()
        elif cmd == 'build':
            build()
        elif cmd == 'flash':
            flash()
        elif cmd == 'flashrun':
            flash_and_run()
        else:
            print(f"Unknown command: {cmd}")

