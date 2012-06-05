/*****************************************************************************
**	mbrwPaintUtil.hpp
**
**	Utility for pasting materials with mouse clicks.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MBRW_PAINTUTIL_HPP
#error mbrwPaintUtil.hpp multiply included
#endif
#define MBRW_PAINTUTIL_HPP

class fsLocator;
class tma3dRenderView;

namespace mbrwPaintUtil
{
	//--------------------------------------------------------------------
	// PasteMaterialFile - called from darg and drop, find the 
	//	material picked in the render view and apply the given 
	// material file from the material library.
	//--------------------------------------------------------------------
	void  PasteMaterialFile(tma3dRenderView *i_pRenderView,
							int i_XMousePos,
							int i_YMousePos,
							const fsLocator& i_MaterialFile);

	//--------------------------------------------------------------------
	// Returns true if a material object is selected
	//--------------------------------------------------------------------
	bool CanAssignMaterial();

	//--------------------------------------------------------------------
	// If we have a selected material script object, then
	// paste the material file that is selected onto that material
	//--------------------------------------------------------------------
	void  AssignMaterialFile(const fsLocator& i_MaterialFile);

	//--------------------------------------------------------------------
	// While drag and drop of material is active, auto-GPU pick
	// materials.
	//--------------------------------------------------------------------
	//bool GetDoAutoPickMaterial();
	//void SetDoAutoPickMaterial(bool i_bVal);


}	// end of namespace
