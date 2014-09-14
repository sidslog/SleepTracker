//
//  value_holder.h
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//

#ifndef __SleepTrackerPebble__value_holder__
#define __SleepTrackerPebble__value_holder__

#include <pebble.h>

// add fit value to current
uint32_t addFitValue(uint32_t value);

// store sleep value for every 10 minutes
void addSleepValue(uint32_t value);

// change mode
void setMode(uint32_t mode);

// get mode
uint32_t currentMode();

#endif /* defined(__SleepTrackerPebble__value_holder__) */
