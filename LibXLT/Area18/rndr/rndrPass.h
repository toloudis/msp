#pragma once

#include <vector>

class oglContext;
class rndrDrawCall;

class rndrPass
{
public:
	rndrPass(oglContext* iDevice);
	virtual ~rndrPass(void);

	virtual void run(const std::vector<rndrDrawCall*>& iDrawCalls);

protected:
	virtual void pushGraphicsState() {}
	virtual void popGraphicsState() {}

	oglContext* mDevice;
};

