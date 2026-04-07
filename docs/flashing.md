# Flashing Instructions

Two supported workflows: **PlatformIO** (recommended) and **esptool** (manual).

---

## Hardware requirements

| Component                     | Specification / Part number                           |
|-------------------------------|-------------------------------------------------------|
| MCU board                     | ESP32-DEVKITC-V4 (WROOM-32 module)                    |
| CAN transceiver               | SN65HVD230 (3.3 V) — one per ESP32 node               |
| USB-Serial cable              | USB Type-A to Micro-USB (already on DevKit)           |
| Termination resistors         | 120 Ω, 0.25 W (2 × per bus segment)                  |
| Bus cable                     | 120 Ω twisted-pair (e.g. CAT5 pair)                  |

---

## Option A — PlatformIO (recommended)

### 1. Prerequisites

```bash
pip install --upgrade platformio
```

Verify:

```bash
pio --version   # should print PlatformIO Core 6.x
```

### 2. Clone and enter the repo

```bash
git clone https://github.com/Hyperspawn/dropbear_firmware.git
cd dropbear_firmware
```

### 3. Connect the ESP32

Connect the DevKit to your computer via USB.  
On Linux, add yourself to the `dialout` group if you haven't already:

```bash
sudo usermod -aG dialout $USER   # log out and back in after this
```

On macOS, the port appears as `/dev/tty.usbserial-*`.  
On Windows, check Device Manager for `COMx`.

### 4. Flash a specific limb

Replace `<ENV>` with the target limb environment (see table below):

```bash
pio run -e <ENV> --target upload
```

| Limb       | `<ENV>`     |
|------------|-------------|
| Left arm   | `left_arm`  |
| Right arm  | `right_arm` |
| Left leg   | `left_leg`  |
| Right leg  | `right_leg` |
| Head       | `head`      |
| Torso      | `torso`     |

Example — flash the left leg:

```bash
pio run -e left_leg --target upload
```

PlatformIO will automatically:
- Download the ESP32 Arduino core and libraries on first run
- Compile with the correct `CAN_NODE_ID` for the selected limb
- Upload via USB at 921600 baud

### 5. Monitor serial output

```bash
pio device monitor -b 115200
```

Expected output after boot:

```
[dropbear] limb=left_leg  node_id=0x12  joints=6
[dropbear] CAN bus online
```

---

## Option B — esptool (manual)

Use this approach when PlatformIO is not available or you want to flash a pre-built binary.

### 1. Build the binary first

```bash
pio run -e left_leg     # compiles only, no upload
```

The firmware binary is written to:

```
.pio/build/left_leg/firmware.bin
```

### 2. Install esptool

```bash
pip install esptool
```

### 3. Erase flash (first-time only)

```bash
esptool.py --port /dev/ttyUSB0 erase_flash
```

Replace `/dev/ttyUSB0` with your actual port (`COMx` on Windows).

### 4. Flash the binary

```bash
esptool.py \
  --chip esp32 \
  --port /dev/ttyUSB0 \
  --baud 921600 \
  --before default_reset \
  --after hard_reset \
  write_flash \
    --flash_mode dio \
    --flash_freq 40m \
    --flash_size detect \
    0x1000  .pio/build/left_leg/bootloader.bin \
    0x8000  .pio/build/left_leg/partitions.bin \
    0x10000 .pio/build/left_leg/firmware.bin
```

### 5. Monitor serial

```bash
esptool.py --port /dev/ttyUSB0 --baud 115200 \
  | cat    # or use any serial terminal (minicom, PuTTY, etc.)
```

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| `No serial data` / upload fails | Hold BOOT button while upload starts; release after "Connecting…" |
| `Permission denied /dev/ttyUSB0` | `sudo chmod a+rw /dev/ttyUSB0` or add user to `dialout` group |
| `CAN init FAILED` | Check wiring: GPIO4 → CTX, GPIO5 → CRX; verify 3.3 V on transceiver VCC |
| `Wrong node ID on bus` | Ensure you flashed the correct PlatformIO environment for this board |
| Build fails on first run | Run `pio pkg install` to pre-fetch all packages, then retry |
