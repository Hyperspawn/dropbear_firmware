/**
 * @file main.cpp
 * @brief Dropbear ESP32 limb controller — CAN bus actuator node
 *
 * Build with PlatformIO; select the per-limb environment:
 *   pio run -e left_arm
 *
 * See README.md for full flashing instructions.
 */

#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include "can_config.h"
#include "limb_config.h"

// ─── Globals ─────────────────────────────────────────────────────────────────
static CAN_device_t CAN_cfg;

static int16_t joint_positions[LIMB_JOINT_COUNT] = {0};
static int16_t joint_setpoints[LIMB_JOINT_COUNT] = {0};
static uint32_t last_heartbeat_ms = 0;

// ─── CAN helpers ─────────────────────────────────────────────────────────────

void can_send_heartbeat() {
    CAN_frame_t frame;
    frame.FIR.B.FF = CAN_frame_std;
    frame.MsgID    = ((uint32_t)CAN_NODE_ID << 4) | MSG_HEARTBEAT;
    frame.FIR.B.DLC = 1;
    frame.data.u8[0] = LIMB_ID;
    ESP32Can.CANWriteFrame(&frame);
}

void can_send_state() {
    // Send joint states in two 8-byte frames when joint count > 4
    CAN_frame_t frame;
    frame.FIR.B.FF  = CAN_frame_std;
    frame.MsgID     = ((uint32_t)CAN_NODE_ID << 4) | MSG_STATE;
    frame.FIR.B.DLC = min(LIMB_JOINT_COUNT, 4) * 2;
    for (int i = 0; i < frame.FIR.B.DLC / 2; i++) {
        frame.data.u8[i * 2]     = (joint_positions[i] >> 8) & 0xFF;
        frame.data.u8[i * 2 + 1] =  joint_positions[i]       & 0xFF;
    }
    ESP32Can.CANWriteFrame(&frame);
}

void can_handle_rx() {
    CAN_frame_t frame;
    while (ESP32Can.CANReadFrame(&frame) == ESP_OK) {
        uint8_t src_node  = (frame.MsgID >> 4) & 0xFF;
        uint8_t msg_type  =  frame.MsgID        & 0x0F;

        if (src_node != NODE_BRAIN) continue;

        if (msg_type == MSG_CMD_POS) {
            for (int i = 0; i < LIMB_JOINT_COUNT && (i * 2 + 1) < frame.FIR.B.DLC; i++) {
                joint_setpoints[i] = (int16_t)((frame.data.u8[i * 2] << 8)
                                               | frame.data.u8[i * 2 + 1]);
            }
        }
    }
}

// ─── Arduino lifecycle ────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.printf("[dropbear] limb=%s  node_id=0x%02X  joints=%d\n",
                  LIMB_NAME, CAN_NODE_ID, LIMB_JOINT_COUNT);

    CAN_cfg.speed     = (CAN_speed_t)CAN_BITRATE;
    CAN_cfg.tx_pin_id = (gpio_num_t)CAN_TX_PIN;
    CAN_cfg.rx_pin_id = (gpio_num_t)CAN_RX_PIN;
    CAN_cfg.rx_queue  = xQueueCreate(10, sizeof(CAN_frame_t));

    if (ESP32Can.CANInit() != 0) {
        Serial.println("[dropbear] CAN init FAILED — halting");
        while (true) delay(1000);
    }
    Serial.println("[dropbear] CAN bus online");
}

void loop() {
    can_handle_rx();

    // TODO: replace with closed-loop PID / impedance controller per joint
    for (int i = 0; i < LIMB_JOINT_COUNT; i++) {
        joint_positions[i] = joint_setpoints[i];  // open-loop passthrough for now
    }

    can_send_state();

    uint32_t now = millis();
    if (now - last_heartbeat_ms >= (1000 / HEARTBEAT_HZ)) {
        can_send_heartbeat();
        last_heartbeat_ms = now;
    }

    delay(1000 / CONTROL_LOOP_HZ);
}
