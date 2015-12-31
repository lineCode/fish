#include "AoiObject.h"
#include <stddef.h>

AoiObject::AoiObject(int id)
{
	_objPrev = _objNext = _watcherPrev = _watcherNext = NULL;
}


AoiObject::~AoiObject(void)
{
}
