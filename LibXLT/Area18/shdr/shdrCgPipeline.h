#pragma once
#include "area18\shdr\shdrpipeline.hpp"
#include <Cg/cg.h>

class oglContext;
class oglDevice;

class shdrCgPipeline :
	public shdrPipeline
{
public:
	shdrCgPipeline(oglContext* i_pDevice, std::string i_Filename);
	virtual ~shdrCgPipeline(void);

	virtual void Bind(oglDevice* i_pDevice);

	CGeffect Effect() { return mCgEffect; }
private:
	CGeffect mCgEffect;
	CGprogram mCgProgram;
};

