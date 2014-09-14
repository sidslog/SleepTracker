//
//  pedometer.c
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//

#include "pedometer.h"

// Timer used to determine next step check
static AppTimer *timer;

bool startedSession = false;

bool validX, validY, validZ = false;

int X_DELTA = 35;
int Y_DELTA, Z_DELTA = 185;
int YZ_DELTA_MIN = 175;
int YZ_DELTA_MAX = 195;
int X_DELTA_TEMP, Y_DELTA_TEMP, Z_DELTA_TEMP = 0;
int lastX, lastY, lastZ, currX, currY, currZ = 0;
int sensitivity = 1;

// interval to check for next step (in ms)
const int ACCEL_STEP_MS = 475;
// value to auto adjust step acceptance
const int PED_ADJUST = 2;
// value by which step goal is incremented
const int STEP_INCREMENT = 50;

bool did_pebble_vibrate = false;

UIUpdate _updateCallback;

void startTrack(UIUpdate updateCallback) {
    _updateCallback = updateCallback;
    
    timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

void stopTrack() {
    app_timer_cancel(timer);
    accel_data_service_unsubscribe();
}


void autoCorrectZ(){
    if (Z_DELTA > YZ_DELTA_MAX){
        Z_DELTA = YZ_DELTA_MAX;
    } else if (Z_DELTA < YZ_DELTA_MIN){
        Z_DELTA = YZ_DELTA_MIN;
    }
}

void autoCorrectY(){
    if (Y_DELTA > YZ_DELTA_MAX){
        Y_DELTA = YZ_DELTA_MAX;
    } else if (Y_DELTA < YZ_DELTA_MIN){
        Y_DELTA = YZ_DELTA_MIN;
    }
}

void pedometer_update() {
    if (startedSession) {
        X_DELTA_TEMP = abs(abs(currX) - abs(lastX));
        if (X_DELTA_TEMP >= X_DELTA) {
            validX = true;
        }
        Y_DELTA_TEMP = abs(abs(currY) - abs(lastY));
        if (Y_DELTA_TEMP >= Y_DELTA) {
            validY = true;
            if (Y_DELTA_TEMP - Y_DELTA > 200){
                autoCorrectY();
                Y_DELTA = (Y_DELTA < YZ_DELTA_MAX) ? Y_DELTA + PED_ADJUST : Y_DELTA;
            } else if (Y_DELTA - Y_DELTA_TEMP > 175){
                autoCorrectY();
                Y_DELTA = (Y_DELTA > YZ_DELTA_MIN) ? Y_DELTA - PED_ADJUST : Y_DELTA;
            }
        }
        Z_DELTA_TEMP = abs(abs(currZ) - abs(lastZ));
        if (abs(abs(currZ) - abs(lastZ)) >= Z_DELTA) {
            validZ = true;
            if (Z_DELTA_TEMP - Z_DELTA > 200){
                autoCorrectZ();
                Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
            } else if (Z_DELTA - Z_DELTA_TEMP > 175){
                autoCorrectZ();
                Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
            }
        }
    } else {
        startedSession = true;
    }
}

void resetUpdate() {
    lastX = currX;
    lastY = currY;
    lastZ = currZ;
    validX = false;
    validY = false;
    validZ = false;
}


#define iter1(N) try = root + (1 << (N)); if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
uint32_t Wilco_sqrt (uint32_t n)
{
    	uint32_t root = 0, try;
    	iter1 (15); iter1 (14); iter1 (13); iter1 (12); iter1 (11);
    	iter1 (10); iter1 ( 9); iter1 ( 8); iter1 ( 7); iter1 ( 6);
    	iter1 ( 5); iter1 ( 4); iter1 ( 3); iter1 ( 2); iter1 ( 1); iter1 ( 0);
    	return root >> 1;
}

static void timer_callback(void *data) {
    AccelData accel = (AccelData ) { .x = 0, .y = 0, .z = 0 };
    accel_service_peek(&accel);
    
    if (!startedSession) {
        lastX = accel.x;
        lastY = accel.y;
        lastZ = accel.z;
    } else {
        currX = accel.x;
        currY = accel.y;
        currZ = accel.z;
    }

    did_pebble_vibrate = accel.did_vibrate;
    
    if ((validX && validY && !did_pebble_vibrate) || (validX && validZ && !did_pebble_vibrate)) {
        int xDiff = currX - lastX;
        int yDiff = currY - lastY;
        int zDiff = currZ - lastZ;
        uint32_t value = Wilco_sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

        app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "value: %ld", (long)value);
        
        _updateCallback(value);
        resetUpdate();
    }
    
    pedometer_update();
    
    timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}
