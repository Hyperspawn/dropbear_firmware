#pragma once

/**
 * @file can_config.h
 * @brief CAN bus pin mapping and network constants for Dropbear ESP32 nodes.
 *
 * Hardware:
 *   MCU          : ESP32-DEVKITC-V4 (WROOM-32)
 *   CAN Driver   : SN65HVD230 3.3 V CAN transceiver (or compatible TJA1050 with 5V→3.3V level shifter)
 *   Bus speed    : 1 Mbit/s
 *   Termination  : 120 Ω at each physical bus end
 *
 * Wiring (per board):
 *   ESP32 GPIO4  → SN65HVD230 CTX (CAN TX)
 *   ESP32 GPIO5  → SN65HVD230 CRX (CAN RX)
 *   3V3           → SN65HVD230 VCC
 *   GND           → SN65HVD230 GND
 *
 * CAN Node IDs (Standard 11-bit):
 *   0x10  left_arm
 *   0x11  right_arm
 *   0x12  left_leg
 *   0x13  right_leg
 *   0x14  head
 *   0x15  torso
 *   0x01  brain (Jetson Orin master — read-only on this firmware)
 */

// ─── GPIO pin assignments ─────────────────────────────────────────────────────
#define CAN_TX_PIN  4   // ESP32 GPIO → SN65HVD230 CTX
#define CAN_RX_PIN  5   // SN65HVD230 CRX → ESP32 GPIO

// ─── Bus parameters ───────────────────────────────────────────────────────────
#define CAN_BITRATE    1000E3   // 1 Mbit/s

// ─── Node ID table ────────────────────────────────────────────────────────────
#define NODE_BRAIN      0x01
#define NODE_LEFT_ARM   0x10
#define NODE_RIGHT_ARM  0x11
#define NODE_LEFT_LEG   0x12
#define NODE_RIGHT_LEG  0x13
#define NODE_HEAD       0x14
#define NODE_TORSO      0x15

// ─── Message IDs (11-bit standard frame) ─────────────────────────────────────
// Format: upper nibble = source node, lower byte = message type
#define MSG_HEARTBEAT   0x00    // node → brain, 1 Hz
#define MSG_CMD_POS     0x01    // brain → node, position setpoint [int16 × n_joints]
#define MSG_CMD_TORQUE  0x02    // brain → node, torque setpoint  [int16 × n_joints]
#define MSG_STATE       0x03    // node → brain, joint states      [int16 × n_joints]
#define MSG_FAULT       0x0F    // node → brain, fault code        [uint8]

// Build-time node ID injected by platformio.ini per-environment
#ifndef CAN_NODE_ID
  #error "CAN_NODE_ID must be defined via build flags (-DCAN_NODE_ID=0xNN)"
#endif
