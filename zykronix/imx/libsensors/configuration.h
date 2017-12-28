/*
 * Copyright (C) 2013 STMicroelectronics, Inc.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STM_SENSORS_CONFIGURATION_H
#define STM_SENSORS_CONFIGURATION_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>



__BEGIN_DECLS

#define SENSOR_PROXIMITY_LABEL 			"Time of Flight Proximity Sensor"
#define SENSOR_PROXIMITY_DATANAME		"STM VL6180 proximity sensor"
#define SENSOR_LIGHT_LABLE			"Light Sensor"
#define SENSOR_LIGHT_DATANAME			"STM VL6180 light sensor"
#define PROXIMITY_ENABLE_FILENAME		"/sys/bus/i2c/devices/i2c-0/0-0029/enable_ps_sensor"
#define PROXIMITY_MAX_RANGE			25
#define PROXIMITY_POWER_CONSUMPTION		0.23f
#define LIGHT_ENABLE_FILENAME			"/sys/bus/i2c/devices/i2c-0/0-0029/enable_als_sensor"
#define LIGHT_MAX_RANGE				65535
#define LIGHT_POWER_CONSUMPTION			0.23f

__END_DECLS

#endif  // ANDROID_SENSORS_H
