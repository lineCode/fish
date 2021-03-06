#ifndef WAYPOINT_PATHFINDER_H
#define WAYPOINT_PATHFINDER_H

#ifndef _MSC_VER
#include <stdbool.h>
#else
#define inline __inline
#endif





struct pathfinder;

typedef void(*finder_result)(void *ud, int x, int z);
typedef void(*finder_dump)(void* ud, int x, int z);

struct pathfinder* finder_create(const char* file);
void finder_release(struct pathfinder* finder);
int finder_find(struct pathfinder * finder, int x0, int z0, int x1, int z1, finder_result cb, void* result_ud, finder_dump dump, void* dump_ud);

#endif