#include "rndrPass.h"

#include "rndrDrawCall.h"

rndrPass::rndrPass(oglContext* iDevice)
	: mDevice(iDevice)
{
}

rndrPass::~rndrPass(void)
{
}

void rndrPass::run(const std::vector<rndrDrawCall*>& iDrawCalls)
{
	pushGraphicsState();
	for (int i = 0; i < iDrawCalls.size(); ++i) 
	{
		iDrawCalls[i]->run(mDevice);
	}
	popGraphicsState();
}


