#ifndef MEMORYTRACKER_H_
#define MEMORYTRACKER_H_

#include <crtdefs.h>

class MemoryTracker
{
public:
	MemoryTracker();
	virtual ~MemoryTracker();

	bool canAllocate(size_t bytes);
	void increment(size_t bytes);
	void decrement(size_t bytes);
private:
	size_t mBytes;
	size_t mMaxBytes;
};

#endif /* MEMORYTRACKER_H_ */
