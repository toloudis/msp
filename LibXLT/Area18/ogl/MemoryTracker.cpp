#include "MemoryTracker.h"

#include <assert.h>

MemoryTracker::MemoryTracker()
: mBytes(0)
, mMaxBytes(-1) // max value
{
}

MemoryTracker::~MemoryTracker()
{
}

bool MemoryTracker::canAllocate(size_t bytes)
{
	assert(mBytes <= mMaxBytes);
	return (mMaxBytes-mBytes) >= bytes;
}
void MemoryTracker::increment(size_t bytes)
{
	assert(mBytes <= mMaxBytes);
	mBytes += bytes;
}
void MemoryTracker::decrement(size_t bytes)
{
	assert(mBytes <= mMaxBytes);
	mBytes -= bytes;
}
