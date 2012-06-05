/****************************************************************************\
**	g3dPassBuffers.hpp
**
**		g3dPassBuffers contains function for manipulating triangle meshes.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dPassBuffers.hpp"


//--------------------------------------------------------------------
//	namespace
//--------------------------------------------------------------------
namespace
{
	captPassBufferFlags l_CaptPassBufferFlags;

	bool l_DoingFileRefl;
}

//--------------------------------------------------------------------
//	GetDoingFileRefl()
//--------------------------------------------------------------------
bool g3dPassBuffers::GetDoingFileRefl()
{
	return l_DoingFileRefl;
}

//--------------------------------------------------------------------
//	SetDoingFileRefl()
//--------------------------------------------------------------------
void g3dPassBuffers::SetDoingFileRefl(bool i_Val)
{
	l_DoingFileRefl = i_Val;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// THE FUNCTIONS BELOW ARE NOT USED
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

//--------------------------------------------------------------------
//	GetCaptPassBufferFlags()
//--------------------------------------------------------------------
captPassBufferFlags g3dPassBuffers::GetCaptPassBufferFlags()
{
	return l_CaptPassBufferFlags;
}

//--------------------------------------------------------------------
//	SetCaptPassBufferFlags()
//--------------------------------------------------------------------
 void g3dPassBuffers::SetCaptPassBufferFlags(captPassBufferFlags i_Flags)
{
	l_CaptPassBufferFlags = i_Flags;
}

//--------------------------------------------------------------------
//	ResetFlags()
//--------------------------------------------------------------------
void g3dPassBuffers::ResetFlags()
{
	l_CaptPassBufferFlags.bPassBufferCapturing = false;
	l_CaptPassBufferFlags.bPassBufferAO = false;
	l_CaptPassBufferFlags.bPassBufferGI = false;
	l_CaptPassBufferFlags.bPassBufferRefl = false;
	l_CaptPassBufferFlags.bPassBufferShadowMask = false;
	l_CaptPassBufferFlags.bPassBufferBeauty = false;
}



