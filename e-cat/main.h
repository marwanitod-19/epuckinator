#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"

//à replacer ces deux dans moves
#define CMtoSTEP	77.922 // 1 cm/s -> 77.922 step/s
#define NB_MOVES	5

#define GOAL_ANGLE	 			0.0f
#define MAX_ANGLE				25.0f
#define ERROR_THRESHOLD			0.05f
#define KP						20.0f	//800.0f
#define KI 						0.05f	//must not be zero
#define KD						0.5f
#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/(KI*CMtoSTEP))


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
