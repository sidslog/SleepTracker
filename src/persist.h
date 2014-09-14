//
//  persist.h
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//


#ifndef __SleepTrackerPebble__persist__
#define __SleepTrackerPebble__persist__

#include <pebble.h>

#define MODE_FIT 0
#define MODE_SLEEP 1

// store data for today fit and sleep
uint32_t mode();
uint32_t fitValue();
uint32_t sleepDataCount();
uint32_t *sleepData(uint32_t dataCount);

void storeFit(uint32_t todayValue);
void storeMode(uint32_t isSleep);
void storeSleepData(uint32_t *data, uint32_t dataLength);

#endif /* defined(__SleepTrackerPebble__persist__) */
