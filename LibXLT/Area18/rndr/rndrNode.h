#pragma once

#include <vector>

class rndrDrawCall;
class rndrPass;

class rndrNode
{
public:
	// parent only null for root node!
	rndrNode(rndrNode* parent, rndrPass& renderer);
	virtual ~rndrNode(void);

	virtual void addDrawCalls(const std::vector<rndrDrawCall*>& iDrawCalls);
	virtual void execute();

	virtual void clearGraph();
	virtual void clear();
private:
	virtual void preTraverse();
	virtual void postTraverse();

	std::vector<rndrDrawCall*> mDrawCalls;
	rndrPass* mRenderPass;

	std::vector<rndrNode*> mChildren;
};

