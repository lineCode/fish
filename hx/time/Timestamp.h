#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <stdint.h>
#include <time.h>

uint64_t Now();

uint64_t TimeStamp();

double GetTimeMillis();

int GetTimeZone();

time_t GetTodayStart(time_t ts);

time_t GetTodayOver(time_t ts);

time_t GetWeekStart(time_t ts);

time_t GetWeekOver(time_t ts);

time_t GetMonthStart(time_t ts);

time_t GetMonthOver(time_t ts);

int GetDiffDay(time_t ts0, time_t ts1);

int GetDiffWeek(time_t ts0, time_t ts1);

time_t GetDayTime(time_t ts, int hour, int min, int sec);

time_t GetDayTimeWithSec(time_t ts, int sec);

void LocalTime(time_t time, struct tm* tm);

#endif