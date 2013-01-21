/*****************************************************************************
**  Area18Layer.hpp
**
**      Area18Layer contains the initialization functions
**	for the all packages within the Area18 Layer.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#pragma once

class oglContext;
class g2dSystem;
class g3dSystem;
class oglDevice;
class rndrEngine;
class testScene;

class Area18Layer
{
public:
	//------------------------------------------------------------------------
	//	Init
	//------------------------------------------------------------------------
	static void Init(void* i_hWnd);

	//------------------------------------------------------------------------
	//	CleanUp
	//------------------------------------------------------------------------
	static void CleanUp() throw();

	//------------------------------------------------------------------------
	//	InitGraphics - should be called after device is created
	//------------------------------------------------------------------------
	static void InitGraphics();

	//------------------------------------------------------------------------
	//	CleanUpGraphics - should be called before device is destroyed
	//------------------------------------------------------------------------
	static void CleanUpGraphics();

	static oglContext* defaultContext();

	static g2dSystem* GetSystem2D();
	static g3dSystem* GetSystem3D();

	static oglDevice* GetDevice(int i);

	enum TestScene {
		TS_CAMERA,
		TS_FBO,
		TS_TEXTURE,
		TS_PRIMITIVE,
		TS_VOLUME,
		TS_TEXT,
		TS_DIRLT,
		NUM_TESTS
	};
	// client must delete.
	static testScene* GetTestScene(TestScene t);

	static rndrEngine* createTiledRenderer();
};
