# dropbear_firmware

[![Firmware CI](https://github.com/Hyperspawn/dropbear_firmware/actions/workflows/ci.yml/badge.svg)](https://github.com/Hyperspawn/dropbear_firmware/actions/workflows/ci.yml)

ESP32 firmware for real-time actuator control over CAN bus, with modular configs per limb.

---

## Overview

Each of Dropbear's 6 limbs runs a dedicated **ESP32-DEVKITC-V4** node that:

- Receives position / torque commands from the Jetson Orin brain over a shared **1 Mbit/s CAN bus**
- Reports joint states back to the brain at 500 Hz
- Sends 1 Hz heartbeats for liveness monitoring

The firmware is built with **PlatformIO** (Arduino framework).  
A separate per-limb build environment injects the correct `CAN_NODE_ID` at compile time — no source edits required.

---

## Quick start — flashing a limb

### Prerequisites

| Tool | Install |
|------|---------|
| Python ≥ 3.8 | [python.org](https://python.org) |
| PlatformIO Core | `pip install platformio` |
| Git | [git-scm.com](https://git-scm.com) |

### Steps

```bash
# 1. Clone
git clone https://github.com/Hyperspawn/dropbear_firmware.git
cd dropbear_firmware

# 2. Connect the ESP32 board via USB

# 3. Flash — replace <limb> with your target (see table below)
pio run -e <limb> --target upload

# 4. Verify
pio device monitor -b 115200
# Expected: [dropbear] limb=<limb>  node_id=0xNN  joints=N
#           [dropbear] CAN bus online
```

### Limb environments

| Limb       | PlatformIO env | CAN node ID | Joints |
|------------|----------------|-------------|--------|
| Left arm   | `left_arm`     | `0x10`      | 7      |
| Right arm  | `right_arm`    | `0x11`      | 7      |
| Left leg   | `left_leg`     | `0x12`      | 6      |
| Right leg  | `right_leg`    | `0x13`      | 6      |
| Head       | `head`         | `0x14`      | 3      |
| Torso      | `torso`        | `0x15`      | 2      |

For detailed flashing instructions and troubleshooting (including the esptool workflow), see **[docs/flashing.md](docs/flashing.md)**.

---

## Hardware

### Bill of materials (per node)

| Component | Part | Notes |
|-----------|------|-------|
| MCU | ESP32-DEVKITC-V4 (WROOM-32) | 3.3 V I/O |
| CAN transceiver | SN65HVD230 | 3.3 V native; preferred over TJA1050 |
| Termination | 120 Ω 0.25 W resistor × 2 | One at each physical bus end only |
| Bus cable | 120 Ω twisted pair | e.g. one pair of CAT5 |

### Wiring (per node)

```
ESP32 GPIO4  ──►  SN65HVD230 CTX
ESP32 GPIO5  ◄──  SN65HVD230 CRX
ESP32 3V3    ───  SN65HVD230 VCC
ESP32 GND    ───  SN65HVD230 GND
```

See **[docs/can_config.md](docs/can_config.md)** for the full CAN network topology, message format, and per-limb configuration details.

---

## Repository structure

```
dropbear_firmware/
├── platformio.ini          # PlatformIO project — one env per limb
├── src/
│   └── main.cpp            # Firmware entry point
├── include/
│   ├── can_config.h        # CAN pin mapping, node IDs, message IDs
│   └── limb_config.h       # Per-limb joint count and torque limits
├── docs/
│   ├── flashing.md         # Step-by-step flash instructions (PlatformIO + esptool)
│   └── can_config.md       # CAN bus topology and message format
└── .github/
    └── workflows/
        └── ci.yml          # GitHub Actions — builds all 6 limb envs on push
```

---

## CI

GitHub Actions builds all 6 limb environments on every push to `main` / `develop` and on PRs targeting `main`.

Status badge at the top of this file reflects the latest `main` build.

To run CI locally:

```bash
pio run   # builds all environments defined in platformio.ini
```

---

## Contributing

1. Fork the repo and create a feature branch.
2. Make your changes; ensure `pio run` succeeds for all environments.
3. Open a pull request against `main`.

Please keep per-limb constants in `include/limb_config.h` and CAN parameters in `include/can_config.h` — avoid hardcoding values in `src/main.cpp`.

---

## License

MIT — see [LICENSE](LICENSE).
