# CAN Bus Configuration

Dropbear uses a single shared CAN bus running at **1 Mbit/s**.  
Each limb has a dedicated ESP32 node with a fixed 11-bit CAN node ID.

---

## Network topology

```
Jetson Orin (brain, 0x01)
    │
    └── CAN bus (1 Mbit/s, 120 Ω termination at each end)
            ├── left_arm   ESP32  0x10
            ├── right_arm  ESP32  0x11
            ├── left_leg   ESP32  0x12
            ├── right_leg  ESP32  0x13
            ├── head       ESP32  0x14
            └── torso      ESP32  0x15
```

## Node ID table

| Limb       | Node ID | Joints | Max torque | PlatformIO env |
|------------|---------|--------|-----------|----------------|
| Brain      | `0x01`  | —      | —         | —              |
| Left arm   | `0x10`  | 7      | 40 N·m    | `left_arm`     |
| Right arm  | `0x11`  | 7      | 40 N·m    | `right_arm`    |
| Left leg   | `0x12`  | 6      | 80 N·m    | `left_leg`     |
| Right leg  | `0x13`  | 6      | 80 N·m    | `right_leg`    |
| Head       | `0x14`  | 3      | 5 N·m     | `head`         |
| Torso      | `0x15`  | 2      | 60 N·m    | `torso`        |

## Message format

All frames are **standard 11-bit** (not extended).

Message ID = `(node_id << 4) | msg_type`

| msg_type | Name         | Direction       | Payload                          |
|----------|--------------|-----------------|----------------------------------|
| `0x00`   | HEARTBEAT    | limb → brain    | `[limb_id]` (1 byte)             |
| `0x01`   | CMD_POS      | brain → limb    | `[int16 × n_joints]` big-endian  |
| `0x02`   | CMD_TORQUE   | brain → limb    | `[int16 × n_joints]` big-endian  |
| `0x03`   | STATE        | limb → brain    | `[int16 × n_joints]` big-endian  |
| `0x0F`   | FAULT        | limb → brain    | `[uint8 fault_code]`             |

## Hardware wiring

Each ESP32 node connects to one **SN65HVD230** 3.3 V CAN transceiver:

```
ESP32 GPIO4  ──►  SN65HVD230 CTX
ESP32 GPIO5  ◄──  SN65HVD230 CRX
ESP32 3V3    ───  SN65HVD230 VCC
ESP32 GND    ───  SN65HVD230 GND
```

> **Tip:** The TJA1050 is a 5 V transceiver and requires a 5 V→3.3 V level shifter on the CRX line when used with ESP32.  Prefer the SN65HVD230 for a 3.3 V-native design.

Bus cables: twisted-pair (120 Ω characteristic impedance), add 120 Ω termination resistors at both physical ends of the bus.

## Setting the node ID at flash time

The node ID is injected via the PlatformIO build environment — **no code change required**:

```ini
; in platformio.ini
[env:left_leg]
build_flags = -DLIMB_ID=2 -DLIMB_NAME='"left_leg"' -DCAN_NODE_ID=0x12
```

Select the correct environment when flashing (see [flashing.md](flashing.md)).
