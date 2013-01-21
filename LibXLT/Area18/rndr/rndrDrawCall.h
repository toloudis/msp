#pragma once

class meshTriMeshFrag;
class oglContext;
class shdrPipeline;
class matMaterial;

class rndrDrawCall
{
public:
	rndrDrawCall(void);
	~rndrDrawCall(void);

	void run(oglContext* iDevice);

	// geometry
	const meshTriMeshFrag* mFrag;
	// shader + params
	shdrPipeline* mEffect;
};

