//
// Created by Swagger on 2021/9/4.
//

#include "timeval.h"




TimeVal::~TimeVal() {}

TimeVal::TimeVal(long seconds, long useconds)
{
    fTv_.tv_sec = seconds;
    fTv_.tv_usec = useconds;
}


long TimeVal::seconds() const {
    return fTv_.tv_sec;
}


long TimeVal::useconds() const {
    return fTv_.tv_usec;
}

// 简单包装
DelayInterval::DelayInterval(long seconds, long useconds)
:   TimeVal(seconds, useconds)
{

}

// arg1 * arg2
DelayInterval operator * (short arg1, DelayInterval& arg2){
    long seconds = arg1 * arg2.seconds();
    long useconds = arg1 * arg2.useconds();
    long carry = useconds / 1000 * 1000;
    seconds += carry;
    useconds -= carry * 1000 * 1000;
    return DelayInterval(seconds, useconds);
}


EventTime::EventTime(long secondsEpoch, long usecondsEpoch)
: TimeVal(secondsEpoch, usecondsEpoch)
{

}

EventTime now(){
    timeval val{};
    gettimeofday(&val, nullptr);
    return EventTime(val.tv_sec, val.tv_usec);
}




