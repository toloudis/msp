/****************************************************************************\
**	g3dPassBuffers.hpp
**
**		g3dPassBuffers contains function for manipulating triangle meshes.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_PASSBUFFERS_HPP
#error g3dPassBuffers.hpp multiply included
#endif
#define G3D_PASSBUFFERS_HPP

struct captPassBufferFlags
{
	bool bPassBufferCapturing;
	bool bPassBufferAO;
	bool bPassBufferGI;
	bool bPassBufferRefl;
	bool bPassBufferShadowMask;
	bool bPassBufferBeauty;
};

namespace g3dPassBuffers
{
	enum blendOps
	{
		e_ADD = 0,
		e_MUL = 1,
		e_REPLACE = 2
	};

	//--------------------------------------------------------------------
	//	GetDoingFileRefl()
	//--------------------------------------------------------------------
	 bool GetDoingFileRefl();

	//--------------------------------------------------------------------
	//	SetDoingFileRefl()
	//--------------------------------------------------------------------
	 void SetDoingFileRefl(bool i_Val);

	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	// THE FUNCTIONS BELOW ARE NOT USED
	//////////////////////////////////////////////////////
	//////////////////////////////////////////////////////

	//--------------------------------------------------------------------
	//	GetCaptPassBufferFlags()
	//--------------------------------------------------------------------
	captPassBufferFlags GetCaptPassBufferFlags();

	//--------------------------------------------------------------------
	//	SetCaptPassBufferFlags()
	//--------------------------------------------------------------------
	 void SetCaptPassBufferFlags(captPassBufferFlags i_Flags);

	//--------------------------------------------------------------------
	//	ResetFlags()
	//--------------------------------------------------------------------
	 void ResetFlags();
}
