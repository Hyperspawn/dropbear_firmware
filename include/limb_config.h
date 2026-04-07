#pragma once

/**
 * @file limb_config.h
 * @brief Per-limb joint count and actuator parameters for Dropbear.
 *
 * Each limb runs on a dedicated ESP32 node.  The correct environment
 * (left_arm, right_arm, …) is selected at flash time via platformio.ini.
 *
 * Joint counts:
 *   left_arm  / right_arm  : 7 DOF  (shoulder 3-DOF, elbow 1-DOF, wrist 3-DOF)
 *   left_leg  / right_leg  : 6 DOF  (hip 3-DOF, knee 1-DOF, ankle 2-DOF)
 *   head                   : 3 DOF  (pan, tilt, roll)
 *   torso                  : 2 DOF  (yaw, pitch)
 */

#if   LIMB_ID == 0  // left_arm
  #define LIMB_JOINT_COUNT  7
  #define LIMB_MAX_TORQUE_NM  40.0f

#elif LIMB_ID == 1  // right_arm
  #define LIMB_JOINT_COUNT  7
  #define LIMB_MAX_TORQUE_NM  40.0f

#elif LIMB_ID == 2  // left_leg
  #define LIMB_JOINT_COUNT  6
  #define LIMB_MAX_TORQUE_NM  80.0f

#elif LIMB_ID == 3  // right_leg
  #define LIMB_JOINT_COUNT  6
  #define LIMB_MAX_TORQUE_NM  80.0f

#elif LIMB_ID == 4  // head
  #define LIMB_JOINT_COUNT  3
  #define LIMB_MAX_TORQUE_NM  5.0f

#elif LIMB_ID == 5  // torso
  #define LIMB_JOINT_COUNT  2
  #define LIMB_MAX_TORQUE_NM  60.0f

#else
  #define LIMB_JOINT_COUNT  1
  #define LIMB_MAX_TORQUE_NM  0.0f
#endif

// Control loop rate
#define CONTROL_LOOP_HZ     500   // 500 Hz inner loop
#define HEARTBEAT_HZ        1     // 1 Hz CAN heartbeat to brain
