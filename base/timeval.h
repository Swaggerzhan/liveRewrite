//
// Created by Swagger on 2021/9/4.
//

#ifndef LIVEREWRITE_TIMEVAL_H
#define LIVEREWRITE_TIMEVAL_H
#include <sys/time.h>


class TimeVal{
public:

    ~TimeVal();

    long seconds() const;
    long useconds() const ;

protected:
    TimeVal(long seconds, long useconds);

private:
    timeval fTv_;
};


class DelayInterval: public TimeVal{
public:
    DelayInterval(long seconds, long useconds);
};


class EventTime: public TimeVal{
public:
    EventTime(long secondsEpoch = 0, long usecondsEpoch = 0);
};






#endif //LIVEREWRITE_TIMEVAL_H
