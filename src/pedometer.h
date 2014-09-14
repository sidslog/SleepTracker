//
//  pedometer.h
//  SleepTrackerPebble
//
//  Created by Sergey Sedov on 14/09/14.
//  Copyright (c) 2014 sidslog. All rights reserved.
//

#ifndef __SleepTrackerPebble__pedometer__
#define __SleepTrackerPebble__pedometer__

#include <pebble.h>

typedef void (*UIUpdate)(int value);

void startTrack(UIUpdate updateCallback);
void stopTrack();

static void timer_callback(void *data);

#endif /* defined(__SleepTrackerPebble__pedometer__) */
