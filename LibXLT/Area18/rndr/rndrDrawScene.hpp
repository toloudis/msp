/****************************************************************************\
**	rndrDrawScene.hpp
**
**	The rndrDrawScene is a rendering algorithm that uses
**	only projected lights to generate shadows. It also includes many optional
**	post process render passes.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef RNDR_DRAWSCENE_HPP
#error rndrDrawScene.hpp multiply included
#endif
#define RNDR_DRAWSCENE_HPP

#include <vector>

class camCamera;
class oglDevice;
class g3dScene;

class rndrDrawScene
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rndrDrawScene();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rndrDrawScene();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( const camCamera& i_Camera,
		const g3dScene &i_Scene,
		oglDevice* i_pDevice);

};

