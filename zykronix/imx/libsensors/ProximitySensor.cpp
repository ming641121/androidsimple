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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include <linux/input.h>

#include "configuration.h"
#include "ProximitySensor.h"

//#define FETCH_FULL_EVENT_BEFORE_RETURN 1

/*****************************************************************************/

ProximitySensor::ProximitySensor()
    : SensorBase(NULL, "STM VL6180 proximity sensor"),
      mEnabled(0),
      mInputReader(4),
      mHasPendingEvent(false)
{
    //Added for debug
    ALOGV("ProximitySensor::ProximitySensor gets called!");
	
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_PX;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

    if (data_fd) {
        strcpy(input_sysfs_path, "/sys/class/input/");
        strcat(input_sysfs_path, "input");
        strcat(input_sysfs_path, input_name+5);
        strcat(input_sysfs_path, "/device/");
        input_sysfs_path_len = strlen(input_sysfs_path);
	ALOGV("ProximitySensor: input_name:%s, input_sysfs_path:%s",input_name,input_sysfs_path);
        enable(0, 1);
    }
}

ProximitySensor::~ProximitySensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int ProximitySensor::setInitialState() {
    struct input_absinfo absinfo;
    if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_PROXIMITY), &absinfo) ) {
        mHasPendingEvent = true;
	mPendingEvent.distance = absinfo.value;
    }
    return 0;
}

int ProximitySensor::enable(int32_t, int en) {

    //Added for debug
    ALOGV("ProximitySensor::enable function called for stmvl5180!");

    char buf[2];
    int err;

    int flags = en ? 1 : 0;
    if (flags != mEnabled) {
        int fd;
        //strcpy(&input_sysfs_path[input_sysfs_path_len], "enable");
        //fd = open(input_sysfs_path, O_RDWR);
	fd = open(PROXIMITY_ENABLE_FILENAME, O_RDWR);
        if (fd >= 0) {
            buf[1] = 0;
            if (flags) {
                buf[0] = '1';
            } else {
                buf[0] = '0';
            }
            err = write(fd, buf, sizeof(buf));
            close(fd);
            mEnabled = flags;
            setInitialState();
	    //Added for debug
	    ALOGV("ProximitySensor::enable enabled VL6180! Error code(2 should indicate success): %d", err);
            return 0;
        }
        ALOGV("ProximitySensor::enable: Failed to open file descriptor for enabling VL6180");
        ALOGV("ProximitySensor::enable: Attempted to open file %s", input_sysfs_path);
        return -1;
    }
    return 0;
}

bool ProximitySensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int ProximitySensor::setDelay(int32_t handle, int64_t delay_ns)
{
    ALOGV("ProximitySensor::setDelay called!");
    int fd;
    strcpy(&input_sysfs_path[input_sysfs_path_len], "pollrate_ms");
    fd = open(input_sysfs_path, O_RDWR);
    if (fd >= 0) {
        char buf[80];
        sprintf(buf, "%lld", delay_ns / 1000000);
        write(fd, buf, strlen(buf)+1);
        close(fd);
		//Added for debug
		ALOGV("ProximitySensor::setDelay is successful!");
        return 0;
    }
    ALOGV("ProximitySensor::setDelay failed!");
    return -1;
}

int ProximitySensor::readEvents(sensors_event_t* data, int count)
{

    //Added for debug
    ALOGV("ProximitySensor::readEvents gets called!");

    if (count < 1)
        return -EINVAL;

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0){
        ALOGV("ProximitySensor::readEvents: Did not successfully read an event, error: %d", n);
        return n;
    }

    int numEventReceived = 0;
    input_event const* event;

#if FETCH_FULL_EVENT_BEFORE_RETURN
again:
#endif
    ALOGV("ProximitySensor::readEvents: At readEvent loop, probably was able to read at least one event successfully");
    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_PROXIMITY) {
               if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    //mPendingEvent.distance = indexToValue(event->value);
		    //mPendingEvent.distance = event->value;
		    mPendingEvent.data[0] = event->value;
		    //LOGE("%s: p sensor data (%d)",__func__,event->value);
                }
            } 
	    else if (event->code == ABS_X)
		mPendingEvent.data[1] = event->value;
	    else if (event->code == ABS_HAT0X)
		mPendingEvent.data[2] = event->value;
            ALOGV("ProximitySensor: gets one ps data event!");
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                numEventReceived++;
                count--;
	        //Added for debug
		ALOGV("ProximitySensor: gets one ps sync event!");
            }
        } else {
            ALOGV("ProximitySensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

#if FETCH_FULL_EVENT_BEFORE_RETURN
    /* if we didn't read a complete event, see if we can fill and
       try again instead of returning with nothing and redoing poll. */
    if (numEventReceived == 0 && mEnabled == 1) {
        n = mInputReader.fill(data_fd);
        if (n)
            goto again;
    }
#endif

    return numEventReceived;
}

void ProximitySensor::processEvent(int code, int value)
{
}

