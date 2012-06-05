/*****************************************************************************
**	api3dTargetRendererMgr.hpp
**
**		api3dTargetRendererMgr maintains targets for textures that need
**	to be updated by pre-renders of the scene.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_TARGETRENDERERMGR_HPP
#error api3dTargetRendererMgr.hpp multiply included
#endif
#define API3D_TARGETRENDERERMGR_HPP


//============================================================================
//============================================================================
class g3dTargetRenderer;


//============================================================================
//============================================================================
namespace api3dTargetRendererMgr
{
	enum TargetUsage
	{
		e_Shadow,
		e_Reflection,
		e_Texture,
		e_Opacity,
	};

	//------------------------------------------------------------------------
	// Add TargetRenderer to management. The caller retains ownership of the target
	//	and should call RemoveTargetRenderer before deleting.
	//------------------------------------------------------------------------
	void AddTargetRenderer( g3dTargetRenderer *i_pTargetRenderer, 
		TargetUsage i_Usage = e_Shadow );

	//------------------------------------------------------------------------
	// Remove TargetRenderer from manager. Return true if found and removed.
	//------------------------------------------------------------------------
	bool RemoveTargetRenderer( g3dTargetRenderer *i_pTargetRenderer );

	//------------------------------------------------------------------------
	// Examine if the target renderer exists
	//------------------------------------------------------------------------
	bool IsTargetRendererExist( g3dTargetRenderer *i_pTargetRenderer );

	//------------------------------------------------------------------------
	// Render all targets
	//------------------------------------------------------------------------
	void RenderTargets(float i_Time);	
};
