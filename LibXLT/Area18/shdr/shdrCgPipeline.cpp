#include "shdrCgPipeline.h"

#include "Area18\ogl\oglContext.h"
#include <Cg/cgGL.h>

shdrCgPipeline::shdrCgPipeline(oglContext* iDevice, std::string iFilename)
{
	mCgEffect = cgCreateEffectFromFile(iDevice->cgContext(),
		iFilename.c_str(),
		NULL);//const char ** args );
	mCgProgram = cgCreateProgramFromEffect( mCgEffect,
                                     CG_PROFILE_GENERIC,
                                     NULL,//const char * entry,
                                     NULL);//const char ** args );
	cgGLLoadProgram(mCgProgram);
}


shdrCgPipeline::~shdrCgPipeline(void)
{
}

void shdrCgPipeline::Bind(oglDevice* iDevice)
{
	cgGLBindProgram(mCgProgram);
}
