/* Copyright 2013 David Axmark

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * @file CustomMoblet.cpp
 *
 * @brief Event manager, base class for applications.
 *
 * This is a modified version of class MAUtil/Moblet that
 * allows for more flexibility. It is more extension friendly
 * than the original version and provides more hook methods.
 *
 * @author Mikael Kindborg
 */

#include "CustomMoblet.h"

using namespace MAUtil;

namespace Wormhole
{
CustomMoblet::CustomMoblet() :
	mRun(true)
{
	addKeyListener(this);
	addPointerListener(this);
	addCloseListener(this);
	addCustomEventListener(this);
}

CustomMoblet::~CustomMoblet()
{
}

void CustomMoblet::enterEventLoop()
{
	CustomMoblet* moblet = this;

	moblet->mRun = true;

	while (moblet->mRun)
	{
		MAEvent event;
		while (maGetEvent(&event))
		{
			bool handled = handleEvent(event);
			if (!handled)
			{
				processEvent(event);
			}
		}

		moblet->runPendingTimers();
		moblet->runIdleListeners();

		if (moblet->mIdleListeners.size() == 0 && moblet->mRun)
		{
			int ttnt = moblet->timeToNextTimer();
			// ttnt may be >= -1. If we get zero, we do not do maWait(0)
			// because in that case time to next timer is zero.
			if (ttnt)
			{
				maWait(ttnt);
			}
		}
	}
}

void CustomMoblet::exitEventLoop()
{
	mRun = false;
}

void CustomMoblet::exit()
{
	exitEventLoop();
}

void CustomMoblet::run(CustomMoblet* moblet)
{
	moblet->enterEventLoop();
}

void CustomMoblet::closeEvent()
{
	close();
}

void CustomMoblet::close()
{
	maExit(0);
}

void CustomMoblet::processEvent(const MAEvent& event)
{
	CustomMoblet* moblet = this;

	switch (event.type)
	{
		case EVENT_TYPE_CLOSE:
			moblet->fireCloseEvent();
			break;
		case EVENT_TYPE_FOCUS_GAINED:
			moblet->fireFocusGainedEvent();
			break;
		case EVENT_TYPE_FOCUS_LOST:
			moblet->fireFocusLostEvent();
			break;
		case EVENT_TYPE_KEY_PRESSED:
			moblet->fireKeyPressEvent(event.key, event.nativeKey);
			break;
		case EVENT_TYPE_KEY_RELEASED:
			moblet->fireKeyReleaseEvent(event.key, event.nativeKey);
			break;
		case EVENT_TYPE_CHAR:
			moblet->fireCharEvent(event.character);
			break;
		case EVENT_TYPE_POINTER_PRESSED:
			if (event.touchId == 0)
			{
				moblet->firePointerPressEvent(event.point);
			}
			moblet->fireMultitouchPressEvent(
				event.point,
				event.touchId);
			break;
		case EVENT_TYPE_POINTER_DRAGGED:
			if (event.touchId == 0)
			{
				moblet->firePointerMoveEvent(event.point);
			}
			moblet->fireMultitouchMoveEvent(
				event.point,
				event.touchId);
			break;
		case EVENT_TYPE_POINTER_RELEASED:
			if (event.touchId == 0)
			{
				moblet->firePointerReleaseEvent(event.point);
			}
			moblet->fireMultitouchReleaseEvent(
				event.point,
				event.touchId);
			break;
		case EVENT_TYPE_CONN:
			moblet->fireConnEvent(event.conn);
			break;
		case EVENT_TYPE_BT:
			moblet->fireBluetoothEvent(event.state);
			break;
		case EVENT_TYPE_TEXTBOX:
			moblet->fireTextBoxListeners(
				event.textboxResult,
				event.textboxLength);
			break;
		case EVENT_TYPE_SENSOR:
			moblet->fireSensorListeners(event.sensor);
			break;
		case EVENT_TYPE_ORIENTATION_DID_CHANGE:
			moblet->fireOrientationChangedEvent(event.orientation);
			break;
		case EVENT_TYPE_ORIENTATION_WILL_CHANGE:
			moblet->fireOrientationWillChangeEvent();
			break;
		default:
			moblet->fireCustomEventListeners(event);
	}
}

/**
 * Get the time to the next timer that should fire.
 * @return Time to next timer event.
 * Returns -1 when there are no timers.
 */
int CustomMoblet::timeToNextTimer()
{
	int now = maGetMilliSecondCount();
	int minTime = -1;
	ListenerSet_each(TimerEventInstance, tei, mTimerEvents)
	{
		int ttn = tei->nextInvoke - now;
		if (ttn < 0)
		{
			ttn = 0;
		}
		if (minTime == -1)
		{
			minTime = ttn;
		}
		else
		{
			// Get MIN(minTime, ttn).
			minTime = minTime < ttn ? minTime : ttn;
		}
	}
	return minTime;
}

void CustomMoblet::runPendingTimers()
{
	int now = maGetMilliSecondCount();

	// Run all timer events and remove those that have run
	// their number of times. Run them at most one time each,
	// as to allow for periods <= 0.
	mTimerEvents.setRunning(true);
	ListenerSet_each(TimerEventInstance, tei, mTimerEvents)
	{
		if (now >= tei->nextInvoke)
		{
			tei->e->runTimerEvent();
			if (tei->numTimes > 0)
			{
				tei->numTimes--;
				if (tei->numTimes == 0)
				{
					mTimerEvents.remove(&*tei);
				}
			}
			tei->nextInvoke += tei->period;
		}
	}
	mTimerEvents.setRunning(false);
}

} // namespace
