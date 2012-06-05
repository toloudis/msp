/*****************************************************************************
**  g2dImageSave.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_IMAGESAVE_HPP
#error g2dImageSave.hpp multiply included
#endif
#define G2D_IMAGESAVE_HPP

#ifndef G2D_IMAGE_HPP
#include "Graphics/g2d/g2dImage.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class gfFileBin;
class g2dImage;
class g2dWindow;


//============================================================================
//============================================================================
class g2dImageSaveImpl
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g2dImageSaveImpl() {};

	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename 
	//	passed in.
	//
	//	If the file format is not known the function will throw 
	//	g2dUnknownImageFileTypeX.
	//------------------------------------------------------------------------
	virtual void Save(const fsLocator& i_FileName, g2dWindow* i_pWin) {};
	virtual void Save(const fsLocator& i_FileName, const g2dImage* i_pImage) {};
};


//============================================================================
//============================================================================
namespace g2dImageSave
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetImplementation(g2dImageSaveImpl *i_pImpl);

	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename 
	//	passed in.
	//
	//	If the file format is not known the function will throw 
	//	g2dUnknownImageFileTypeX.
	//------------------------------------------------------------------------
	void Save(const fsLocator& i_FileName, g2dWindow* i_pWin);
	void Save(const fsLocator& i_FileName, const g2dImage* i_pImage);
}
