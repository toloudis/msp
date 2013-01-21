#pragma once

#include "Area18/ogl/oglTypes.hpp"
#include "Area18/rndr/rndrEngine.h"
#include "Area18/rndr/rndrSampling.h"
#include "Core/ma/maVector3d.hpp"

#include <vector>

class camCamera;
class g2dRenderTarget;
class g3dLayer;
class g3dScene;
class oglDevice;
class oglFramebuffer;
class oglTexture2D;
class rndrConvolution2D;
class rndrBufferToTexture;

class rndrTiledRenderer : public rndrEngine
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rndrTiledRenderer(oglDevice* iDevice);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rndrTiledRenderer();

	//virtual void resize(int w, int h) {mWidth = w; mHeight = h;}
	virtual void draw();

	void setScene(g3dScene* iScene);
	void setCamera(camCamera* iCamera);

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
//	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
//			 const g3dScene &i_Scene,
//			 const std::vector<g3dLayer*>& i_ViewerLayers,
//			 float i_fSimTime );

	int RenderNextTile(camCamera* i_Camera,
		g3dScene* i_Scene);
	bool EndRender();

	virtual void ReleaseResources();

	void setParams(const rndrSampling& params);
private:
	g3dScene* mScene;
	camCamera* mCamera;

	bool SetupRender(const rndrSampling& i_Params, oglDevice* i_pDevice);
	void compileConvolutionKernel();
	cl::Program mConvolutionProgram;
    std::vector<cl::Kernel> mConvolutionKernels;

	rndrSampling m_Params;

	// where are the tiles and image allocated:
	oglDevice* m_pDevice;

	// rgba32f
	GLuint mTileFramebuffer;
	GLuint mTileTexture;
	cl::Image2DGL mTileBuffer;

	GLuint mTileDepthRenderbuffer;

	// filter kernel weights go here
	cl::Buffer mKernelBuffer;

	// result of filtering goes here
	GLuint mFilteredTileTexture;
	cl::Image2DGL mFilteredBuffer;
	GLuint mFilteredTileFramebuffer;

	// tiles will be placed into finalimage and into output window
	GLuint mFinalImageTexture;
	GLuint mFinalImageFramebuffer;

	// the display window (draw to default buffer in context)
	//g2dWindowPrimaryDX11* m_pDisplayWindow;

	// bucket rendering dimensions:
	static const int PREFERRED_TILE_SIZE = 1024;
	struct rndrTile
	{
		float m_ScreenTop, m_ScreenBottom, m_ScreenLeft, m_ScreenRight;
		int m_Width, m_Height; // in pixels (samples)
		int m_X, m_Y; //offset of screentop and screenleft in samples
		int m_OutX, m_OutY;
		int m_OutWidth, m_OutHeight;
	};
	// tiles will overlap by the filter kernel width
	std::vector<rndrTile> m_Tiles;

	// how many tiles for the full image
	int m_nTilesX;// = 1;
	int m_nTilesY;// = 1;
	// how many samples in a standard tile
	int m_TileSizeX;// = 1;
	int m_TileSizeY;// = 1;
	// how many final pixels are in a standard tile?
	int m_TileFinalSizeX;// = 1;
	int m_TileFinalSizeY;// = 1;
	// how many samples does the filter kernel need?
	int m_KernelSamplesX;// = 1;
	int m_KernelSamplesY;// = 1;

	std::vector<maVector3d> m_SamplePoints;
	std::vector<float> m_FilterKernelWeights;

	// track which tile is currently being rendered. for looping over all tiles.
	int m_CurrentTile; 

	void GetTileInfo();
	void FilterAndDownsampleTile(oglDevice* i_pDevice, int i_Index);
	void TextureToWindow(oglDevice* i_pDevice);
};

