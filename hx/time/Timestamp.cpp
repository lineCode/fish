#include "Timestamp.h"


#if defined (WIN32)
#include <windows.h>
#define LOCALTIME(ts,tm) localtime_s(tm, ts);
#else
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LOCALTIME(ts,tm) localtime_r(ts, tm);
#endif

#define DAY_TIME_FAST

static int G_TIMEZONE = 0xffffffff;

double GetTimeMillis() {
	struct timeval tv;

#ifdef _WIN32

#define EPOCH_BIAS (116444736000000000)
#define UNITS_PER_SEC (10000000)
#define USEC_PER_SEC (1000000)
#define UNITS_PER_USEC (10)

	union {
		FILETIME ft_ft;
		uint64_t ft_64;
	} ft;

	GetSystemTimeAsFileTime(&ft.ft_ft);

	if (ft.ft_64 < EPOCH_BIAS) {
		return -1;
	}
	ft.ft_64 -= EPOCH_BIAS;
	tv.tv_sec = (long)(ft.ft_64 / UNITS_PER_SEC);
	tv.tv_usec = (long)((ft.ft_64 / UNITS_PER_USEC) % USEC_PER_SEC);
#else
	gettimeofday(&tv, NULL);
#endif

	return (double)tv.tv_sec * 1000 + (double)tv.tv_usec / 1000;
}

uint64_t Now() {
	return GetTimeMillis() / 1000;
}

uint64_t TimeStamp() {
	return GetTimeMillis() * 1000;
}

int GetTimeZone() {
	if (G_TIMEZONE == 0xffffffff) {
#ifdef _WIN32
		TIME_ZONE_INFORMATION tmp;
		GetTimeZoneInformation(&tmp);
		G_TIMEZONE = tmp.Bias/(-60);
#else
		time_t now = time(NULL);
		struct tm local = { 0 };
		LOCALTIME(&now, &local);
		G_TIMEZONE = local.tm_gmtoff / 3600;
#endif
	}
	return G_TIMEZONE;
}

time_t GetTodayStart(time_t ts) {
	if (ts == 0) {
		ts = time(NULL);
	}

#ifdef DAY_TIME_FAST
	return ts - (ts + GetTimeZone() * 3600) % 86400;
#else
	struct tm local = { 0 };
	LOCALTIME(&ts, &local);
	local.tm_hour = local.tm_min = local.tm_sec = 0;
	return mktime(&local);
#endif
}

time_t GetTodayOver(time_t ts) {
	return GetTodayStart(ts) + 24 * 3600;
}

time_t GetWeekStart(time_t ts) {
	time_t weekTime = GetTodayStart(ts);
	struct tm local = { 0 };
	LOCALTIME(&weekTime, &local);

	if (local.tm_wday == 0) {
		weekTime -= 6 * 24 * 3600;
	} else {
		weekTime -= (local.tm_wday - 1) * 24 * 3600;
	}
	
	return weekTime;
}

time_t GetWeekOver(time_t ts) {
	return GetWeekStart(ts) + 7 * 24 * 3600;
}

time_t GetMonthStart(time_t ts) {
	if (ts == 0) {
		ts = time(NULL);
	}

	struct tm local = { 0 };
	LOCALTIME(&ts, &local);
	local.tm_mday = 1;
	local.tm_hour = local.tm_min = local.tm_sec = 0;
	return mktime(&local);
}

time_t GetMonthOver(time_t ts) {
	if (ts == 0) {
		ts = time(NULL);
	}

	struct tm local = { 0 };
	LOCALTIME(&ts, &local);
	local.tm_mday = local.tm_hour = local.tm_min = local.tm_sec = 0;

	if (local.tm_mon == 11) {
		local.tm_year += 1;
		local.tm_mon = 0;
	} else { 
		local.tm_mon += 1; 
	}
	return mktime(&local);
}

int GetDiffDay(time_t ts0, time_t ts1) {
	time_t time0 = GetTodayStart(ts0);
	time_t time1 = GetTodayStart(ts1);
	return (int)(time1 - time0) / (3600 * 24);
}

int GetDiffWeek(time_t ts0, time_t ts1) {
	time_t time0 = GetMonthStart(ts0);
	time_t time1 = GetMonthStart(ts1);
	return (int)(time1 - time0) / (3600 * 24 * 7);
}

time_t GetDayTime(time_t ts, int hour, int min, int sec) {
	if (ts == 0) {
		ts = time(NULL);
	}

	time_t time = GetTodayStart(ts);
	return time + hour * 3600 + min * 60 + sec;
}

time_t GetDayTimeWithSec(time_t ts, int sec) {
	return GetDayTime(ts, 0, 0, sec);
}

void LocalTime(time_t time, struct tm* tm) {
	LOCALTIME(&time, tm);
}