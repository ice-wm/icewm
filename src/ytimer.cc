/*
 * IceWM
 *
 * Copyright (C) 1998-2001 Marko Macek
 */
#include "config.h"
#include "ytimer.h"
#include "yapp.h"
#include "yprefs.h"

YTimer::YTimer(long ms) :
    fListener(nullptr),
    fTimeout(zerotime()),
    fInterval(0L),
    fFuzziness(0L),
    fRunning(false),
    fFixed(false)
{
    setInterval(ms);
}

YTimer::YTimer(long ms, YTimerListener* listener, bool start, bool fixed) :
    fListener(listener),
    fTimeout(zerotime()),
    fInterval(max(0L, ms)),
    fFuzziness(0L),
    fRunning(false),
    fFixed(fixed)
{
    if (start)
        startTimer();
}

YTimer::~YTimer() {
    stopTimer();
}

void YTimer::setFixed() {
    // Fixed here means: not fuzzy, but exact.
    fFixed = true;
    fFuzziness = 0L;
}

bool YTimer::isFixed() const {
    return fFixed || !fFuzziness;
}

bool YTimer::expires() const {
    return isRunning() && timeout_min() < monotime();
}

void YTimer::setInterval(long ms) {
    fInterval = max(0L, ms);
}

void YTimer::startTimer(long ms) {
    setInterval(ms);
    startTimer();
}

void YTimer::startTimer() {
    fTimeout = monotime() + millitime(fInterval);
    fuzzTimer();
    enlist();
}

void YTimer::fuzzTimer() {
    if (false == fFixed && inrange(DelayFuzziness, 1, 100)) {
        // non-fixed timer: configure fuzzy timeout range
        // to allow for merging of several timers
        fFuzziness = (fInterval * DelayFuzziness) / 100L;
    } else {
        fFuzziness = 0L;
    }
}

void YTimer::runTimer() {
    fTimeout = monotime();
    fuzzTimer();
    enlist();
}

void YTimer::stopTimer() {
    if (fRunning) {
        fRunning = false;
        mainLoop->unregisterTimer(this);
    }
}

void YTimer::enlist() {
    if (fRunning == false) {
        fRunning = true;
        mainLoop->registerTimer(this);
    }
}

void YTimer::disableTimerListener(YTimerListener *listener) {
    // a global timer may be in use by several listeners.
    // here one of those listeners asks to be deregistered.
    // if it was the active listener then also stop the timer.
    YTimer* nonNull(this);
    if (nonNull && fListener == listener) {
        fListener = nullptr;
        stopTimer();
    }
}

void YTimer::setTimer(long interval, YTimerListener *listener, bool start) {
    stopTimer();
    setInterval(interval);
    setTimerListener(listener);
    if (start)
        startTimer();
}

// vim: set sw=4 ts=4 et:
