//
//  persist.c
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//

#include "persist.h"

#define MODE_KEY 1
#define FIT_KEY 2
#define SLEEP_COUNT_KEY 3
#define SLEEP_KEY 4

uint32_t mode() {
    return persist_exists(MODE_KEY) ? persist_read_int(MODE_KEY) : MODE_FIT;
}

uint32_t fitValue() {
    return persist_exists(FIT_KEY) ? persist_read_int(FIT_KEY) : 0;
}

uint32_t sleepDataCount() {
    return persist_exists(SLEEP_COUNT_KEY) ? persist_read_int(SLEEP_COUNT_KEY) : 0;
}

uint32_t *sleepData(uint32_t dataCount) {
    if (persist_exists(SLEEP_KEY)) {
        uint32_t *buffer = malloc(dataCount * sizeof(uint32_t));
        persist_read_data(SLEEP_KEY, buffer, dataCount * sizeof(uint32_t));
        return buffer;
    } else {
        return NULL;
    }
}

void storeFit(uint32_t todayValue) {
    persist_write_int(FIT_KEY, todayValue);
}

void storeMode(uint32_t sleepValue) {
    persist_write_int(MODE_KEY, sleepValue);
}

void storeSleepData(uint32_t *data, uint32_t dataLength) {
    persist_write_data(SLEEP_KEY, data, dataLength);
    persist_write_int(SLEEP_COUNT_KEY, dataLength);
}
