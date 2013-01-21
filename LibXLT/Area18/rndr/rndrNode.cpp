#include "rndrNode.h"

#include "Area18/rndr/rndrPass.h"

rndrNode::rndrNode(rndrNode* parent, rndrPass& renderer)
	: mRenderPass(&renderer)
{
	if (parent)
		parent->mChildren.push_back(this);
}

rndrNode::~rndrNode(void)
{
	// owner must destroy nodes
}

void rndrNode::addDrawCalls(const std::vector<rndrDrawCall*>& iDrawCalls)
{
	mDrawCalls.insert(mDrawCalls.end(), iDrawCalls.begin(), iDrawCalls.end());
}

void rndrNode::execute()
{
	preTraverse();

	for(int i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->execute();
	}

	postTraverse();
}

void rndrNode::clearGraph()
{
	for(int i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->clearGraph();
	}
	mDrawCalls.clear();
}

void rndrNode::clear()
{
	mDrawCalls.clear();
}

void rndrNode::preTraverse()
{
}

void rndrNode::postTraverse()
{
	mRenderPass->run(mDrawCalls);
}
