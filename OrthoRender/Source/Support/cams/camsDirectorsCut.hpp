/*****************************************************************************
**	camsDirectorsCut.hpp
**
**	Base class for cutting between different cameras
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CAMS_DIRECTORSCUT_HPP
#error camsDirectorsCut.hpp multiply included
#endif
#define CAMS_DIRECTORSCUT_HPP


//============================================================================
//============================================================================
class camsDirectorsCut
{
public:
	//--------------------------------------------------------------------
	//  Return index for which camera to use at this moment 
	//  for this director's cut.
	//--------------------------------------------------------------------
	virtual int GetCameraIndex() = 0;

};

