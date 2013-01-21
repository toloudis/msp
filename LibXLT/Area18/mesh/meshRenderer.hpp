/*****************************************************************************
**  meshRenderer.hpp
**
**      meshRenderer renders tri mesh fragments with extra data
**	for texture space at each vertex in order to render per-pixel effects
**  like bump mapping.
**
** Area17
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MESH_RENDERER_HPP
#error meshRenderer.hpp multiply included
#endif
#define MESH_RENDERER_HPP

#include "Area18/ogl/oglTypes.hpp"

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class oglDevice;
class matMaterial;
class meshTriMeshFrag;
class shdrPipeline;

class meshRenderer
{
public:

	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	meshRenderer();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~meshRenderer();

	//------------------------------------------------------------------------
	//	Deallocate - called when all device dependent resources should be
	//	released.
	//------------------------------------------------------------------------
	virtual void Deallocate();

	//------------------------------------------------------------------------
	//	Reallocate - called when the device has been Reset and resources can
	//	be reloaded again.
	//------------------------------------------------------------------------
	virtual void Reallocate();

	//--------------------------------------------------------------------
	// Render
	//--------------------------------------------------------------------
	int Render( const meshTriMeshFrag* i_pFrag, const matMaterial* i_pMaterial, shdrPipeline* i_pEffect,
		oglDevice* i_pDevice);

	//--------------------------------------------------------------------
	// draw a limited number of triangles per draw call.  
	// this is to mitigate the windows TDR (timeout detection response)
	// for expensive calls (e.g. high tessellation + GS amplification)
	// i_NumIndices should be a multiple of 2,3 and 4 ideally.
	// -1 means use the D3D limit.
	//--------------------------------------------------------------------
	static void SetDrawLimit(int i_NumIndices);

private:

};
