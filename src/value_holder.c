//
//  value_holder.c
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//

#include "value_holder.h"
#include "persist.h"

uint32_t _currentMode = MODE_FIT;

uint32_t todayFitValue = 0;

uint32_t *todaySleepValue = NULL;
uint32_t todaySleepCount = 0;

// start/stop events for modes
void startSleepMode();
void stopSleepMode();

void startFitMode();
void stopFitMode();


// add fit value to current
uint32_t addFitValue(uint32_t value) {
    todayFitValue += value;
    return todayFitValue;
}


// store sleep value for every 10 minutes
void addSleepValue(uint32_t value) {
    if (todaySleepCount == 0) {
        todaySleepCount = 1;
        todaySleepValue = malloc(sizeof(uint32_t) * todaySleepCount);
    } else {
        todaySleepCount += 1;
        todaySleepValue = realloc(todaySleepValue, sizeof(uint32_t) * todaySleepCount);
    }
    todaySleepValue[todaySleepCount - 1] = value;
}


void setMode(uint32_t newMode) {
    if (newMode == MODE_FIT) {
        stopSleepMode();
        startFitMode();
    } else {
        stopFitMode();
        startSleepMode();
    }
    storeMode(newMode);
}

uint32_t currentMode() {
    return _currentMode;
}


// start/stop events for modes
void startSleepMode() {
    _currentMode = MODE_SLEEP;
    todaySleepCount = sleepDataCount();
    todaySleepValue = sleepData(todaySleepCount);
}

void stopSleepMode() {
    if (todaySleepValue) {
        storeSleepData(todaySleepValue, todaySleepCount);
        free(todaySleepValue);
    }
}

void startFitMode() {
    _currentMode = MODE_FIT;
    todayFitValue = fitValue();
}

void stopFitMode() {
    if (todayFitValue) {
        storeFit(todayFitValue);
    }
}
