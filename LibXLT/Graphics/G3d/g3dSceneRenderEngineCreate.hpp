/****************************************************************************\
**	g3dSceneRenderEngineCreate.hpp
**
**	g3dSceneRenderEngineCreate is a factory for creating renderers for the current
**	graphics implementation. 
**
**	StudioGPU
**	Copyright(C) 2005. - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENERENDERENGINECREATE_HPP
#error g3dSceneRenderEngineCreate.hpp multiply included
#endif
#define G3D_SCENERENDERENGINECREATE_HPP


//============================================================================
//	Forward References
//============================================================================
class g3dPickRenderer;
class g3dSceneRenderEngine;
class g3dSceneRenderEngineCreateImpl;
class scObject;
class matMaterial;


//============================================================================
//============================================================================
class g3dSceneRenderEngineCreate
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum RenderEngine
	{
		e_Default,
		e_RmanPrman,
		e_MentalRay
	};
	
	//--------------------------------------------------------------------
	//	Creates renderer based on type id
	//--------------------------------------------------------------------
	static g3dSceneRenderEngine* CreateRenderEngine(RenderEngine i_Engine);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static g3dSceneRenderEngine* CreateDefaultEngine( );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static g3dSceneRenderEngine* CreateRmanPrmanEngine( );	

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static g3dSceneRenderEngine* CreateMentalRayEngine( );	

	//--------------------------------------------------------------------
	// Methods for defining implementation
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		// Set new implementation method, returns pointer to last one
		// that was being used.  Both can be NULL.
		// Ownership for the pointer remains with the caller.
		//--------------------------------------------------------------------
		static g3dSceneRenderEngineCreateImpl* SetImplementation(g3dSceneRenderEngineCreateImpl* i_pCreator);

private:
	static g3dSceneRenderEngineCreateImpl* sm_pImplementation;
};


//============================================================================
//============================================================================
class g3dSceneRenderEngineCreateImpl
{
public:

	//--------------------------------------------------------------------
	// Virtual functions to be overriden in implementation
	//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual g3dSceneRenderEngine* CreateDefaultRenderEngine( ) = 0;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual g3dSceneRenderEngine* CreateRmanPrmanEngine( ) = 0;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual g3dSceneRenderEngine* CreateMentalRayEngine( ) = 0;
};

