#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"

#define GOAL_ANGLE	 			0.0f
#define MAX_ANGLE				25.0f
#define ERROR_THRESHOLD			0.1f
#define KP						20.0f	//800.0f
#define KI 						0.05f	//must not be zero
#define KD						0.5f
#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/(KI*CMtoSTEP))
#define STROLL_SPEED			5		//= 5cm/s but possible to add a certain number to make it go faster.
										// To go twice as fast, just do 2*STROLL_SPEED for example.

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
