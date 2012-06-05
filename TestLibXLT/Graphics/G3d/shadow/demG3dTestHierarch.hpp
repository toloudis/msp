/*****************************************************************************
**  demG3dTestHierarch.hpp
**
**		This mode displays a demonstration/test of hierarchical models in the
**	Terawatt G3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTHIERARCH_HPP
#error demG3dTestHierarch.hpp multiply included
#endif
#define DEM_G3DTESTHIERARCH_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENTHANDLE_HPP
#include "g3dFragmentHandle.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dDynamicModel;
class g3dStaticModel;
class g3dDirectionalLight;
class matPlainTexture;

class demG3dTestHierarch 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestHierarch();

		//====================================================================
		//====================================================================
		virtual ~demG3dTestHierarch();

		//====================================================================
		//	Think
		//====================================================================
		virtual void Think();

		//====================================================================
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//====================================================================
		virtual void DeInitialize();

	private:

		g3dFragmentHandle m_RectFragment;
		g3dFragmentHandle m_BaseFragment;
		g3dFragmentHandle m_Arm1Fragment;
		g3dFragmentHandle m_Arm2Fragment;

		g3dDynamicModel* m_Model;
		g3dStaticModel* m_Rect;

		matMaterial m_Mat;

		matTexture* m_Texture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
