#include "Area18/rndr/rndrTiledRenderer.hpp"

#include "Area18/Area18Layer.hpp"
#include "Area18/ogl/oglContext.h"
#include "Area18/ogl/oglDevice.hpp"
#include "Area18/rndr/rndrDrawScene.hpp"
#include "Area18/test/testScene.h"
#include "Core/ma/maFunctions.hpp"
//#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maSampling.hpp"
#include "Graphics/cam/camCamera.hpp"

// for debugging
#include "FreeImage/Dist/FreeImage.h"
#pragma comment(lib, "FreeImage.lib")

#include <fstream>

namespace
{
	maFilter* CreateFilter(eFilterFunc i_FilterType)
	{
		maFilter* filter = NULL;
		switch (i_FilterType)
		{
		case eFilterBox:
			filter = new maBoxFilter;
			break;
		case eFilterGaussian:
			filter = new maGaussianFilter;
			break;
		case eFilterMitchell:
			filter = new maMitchellFilter;
			break;
		case eFilterTriangle:
			filter = new maTriangleFilter;
			break;
		case eFilterSinc:
			filter = new maSincFilter;
			break;
		case eFilterLanczos:
			filter = new maLanczosFilter;
			break;
		case eFilterBlackmanHarris:
			filter = new maBlackmanHarrisFilter;
			break;
		case eFilterCatmullRom:
			filter = new maCatmullRomFilter;
			break;
		default:
			//DBG_WARNING("unknown filter type.");
			break;
		}
		return filter;
	}

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
rndrTiledRenderer::rndrTiledRenderer(oglDevice* i_pDevice)
:	m_pDevice(i_pDevice)
,	mTileFramebuffer(0)
,	mFilteredTileFramebuffer(0)
,	mFinalImageFramebuffer(0)
,	mTileTexture(0)
,	mTileDepthRenderbuffer(0)
,	mFilteredTileTexture(0)
,	mFinalImageTexture(0)
{
//	m_PrtyObject = new rndrTilePrty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rndrTiledRenderer::~rndrTiledRenderer()
{
	ReleaseResources();

//	delete m_PrtyObject;
}

void rndrTiledRenderer::ReleaseResources()
{
	glDeleteFramebuffers(1, &mTileFramebuffer);
	glDeleteFramebuffers(1, &mFilteredTileFramebuffer);
	glDeleteFramebuffers(1, &mFinalImageFramebuffer);

	glDeleteTextures(1, &mTileTexture);
	// hopefully this releases the prior one 
	mTileBuffer = cl::ImageGL();

	glDeleteRenderbuffers(1, &mTileDepthRenderbuffer);

	// hopefully this releases the prior one 
	mKernelBuffer = cl::Buffer();

	glDeleteTextures(1, &mFilteredTileTexture);
	// hopefully this releases the prior one 
	mFilteredBuffer = cl::ImageGL();

	glDeleteTextures(1, &mFinalImageTexture);
}

void rndrTiledRenderer::setParams(const rndrSampling& params)
{
	SetupRender(params, m_pDevice);
	m_Params = params;
}

void rndrTiledRenderer::setScene(g3dScene* iScene)
{
	mScene = iScene;
}

void rndrTiledRenderer::setCamera(camCamera* iCamera)
{
	mCamera = iCamera;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
void rndrTiledRenderer::draw()
//int rndrTiledRenderer::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
//			 const g3dScene &i_Scene,
//			 const std::vector<g3dLayer*>& i_ViewerLayers,
//			 float i_fSimTime )
{
//	m_pDisplayWindow = dynamic_cast<g2dWindowPrimaryDX11*>(i_pWindow);
//	m_pDisplayWindow->Clear(maFloatRGBA(0,0,0,0), true);

	rndrSampling i_Params = m_Params;
	i_Params.mWidth = width();
	i_Params.mHeight = height();
	i_Params.mFilterType = eFilterBox;
	i_Params.mFilterWidthX = 1;
	i_Params.mFilterWidthY = 1;
	i_Params.mPixelSamplesX = 1;
	i_Params.mPixelSamplesY = 1;
	if ((i_Params.mFilterWidthX*i_Params.mPixelSamplesX > 512) ||
		(i_Params.mFilterWidthY*i_Params.mPixelSamplesY > 512) 
		)
	{
		DBG_LOG("Resetting samples to 1x1, filter to 1x1");
		i_Params.mFilterWidthX = 1;
		i_Params.mFilterWidthY = 1;
		i_Params.mPixelSamplesX = 1;
		i_Params.mPixelSamplesY = 1;
	}
	if (i_Params != m_Params)
	{
		SetupRender(i_Params, m_pDevice);
		m_Params = i_Params;
	}

	// testScene camera overrides my mCamera member var
	testScene* s = NULL;//Area18Layer::GetTestScene(m_pDevice);
	if (s && s->camera() != NULL)
	{
		mCamera = s->camera();
		mCamera->SetAspect(width(), height());
	}


// If we want to render separate tiles on separate devices, we need to make sure 
// tile memory and scene assets are allocated on each device.
// Then, as tiles are completed, we have to bring them back to the window's device
// for display, and also collect them in the main image buffer's device for final 
// assembly and output.

	m_CurrentTile = 0;
	while (m_CurrentTile < m_Tiles.size())
	{
		RenderNextTile(mCamera,
			mScene);

//		m_pDisplayWindow->MakeCurrent();
//		rndrTextureToTexture copyOp;
//		copyOp.SetInputDevice(device);
//		copyOp.SetInputBuffer(m_pTileSRV);
//		copyOp.SetInputSize(m_pTile->GetWidth(), m_pTile->GetHeight());
//		copyOp.SetOutputTexture(m_pDisplayWindow->GetBackBuffer());
//		copyOp.Process();

		//::D3DX11SaveTextureToFile(device->m_pDeviceContext, m_pTile->GetResource(), D3DX11_IFF_PNG, L"TestTile.png");

		FilterAndDownsampleTile(m_pDevice, m_CurrentTile);

		// put filteredtiletexture into image (or direct to window)
		TextureToWindow(m_pDevice);

		m_CurrentTile++;
	}

	EndRender();
}

//------------------------------------------------------------------------
// input params:
// finalImageXY, tileSizeXY, pixelSamplesXY, filtertype, filterwidth
// if any of these change, then everything has to be recomputed.
//------------------------------------------------------------------------
bool rndrTiledRenderer::SetupRender(const rndrSampling& i_Params, oglDevice* i_pDevice)
{
	ReleaseResources();

	int oldKernelSamplesX = m_KernelSamplesX;

	// these values are along one side of the pixel.
	// assume they are to be squared to 2d quantities.
	int samplesPerPixelX = i_Params.mPixelSamplesX;
	int samplesPerPixelY = i_Params.mPixelSamplesY;

	float filterWidthPixelsX = i_Params.mFilterWidthX;
	float filterWidthPixelsY = i_Params.mFilterWidthY;
	// don't allow filter widths less than 1 pixel.
	filterWidthPixelsX = max(1.0f, filterWidthPixelsX);
	filterWidthPixelsY = max(1.0f, filterWidthPixelsY);

	int outputWidthPixels = i_Params.mWidth;
	int outputHeightPixels = i_Params.mHeight;

	// pixel sampling support:
	// outputWidthPixels * samplesPerPixelX

	// filter support at borders of image:
	// (kernelSamples-samplesPerPixel)/2 on each side.
	// note integer truncation.
	// (taking out one pixel from the filter width)
	int halfSupportX = (int) ceil(max( (filterWidthPixelsX-1.0)*samplesPerPixelX / 2.0 , 0));
	int halfSupportY = (int) ceil(max( (filterWidthPixelsY-1.0)*samplesPerPixelY / 2.0 , 0));

	int filterEdgeSupportX = halfSupportX * 2;
	int filterEdgeSupportY = halfSupportY * 2;

	m_KernelSamplesX = samplesPerPixelX + filterEdgeSupportX;
	m_KernelSamplesY = samplesPerPixelY + filterEdgeSupportY;

	// assumption: input image is supersampled, with filter support.
	// each pixel in input image is a sample for filtering.
	int totalImageSamplesWidth = outputWidthPixels*samplesPerPixelX + (filterEdgeSupportX);
	int totalImageSamplesHeight = outputHeightPixels*samplesPerPixelY + (filterEdgeSupportY);
	// or,
	// outputWidthPixels = (totalImageSamplesWidth - (kernelSamples-samplesPerPixel))/samplesPerPixel

	// in screen space, which spans -1..1:
	// -1 maps to 0+halfSupport.  +1 maps to totalImageSamplesWidth - halfSupport.
	// we need these values to orient the camera properly.
	// 0   h       W/2     W-h  W
	// ?--(-1)------0------(1)--?

	//want screen coord of output pixel centered on center sample?
	// so shift by samplesPerPixel/2
	int halfPixelShiftX = samplesPerPixelX/2;
	int halfPixelShiftY = samplesPerPixelY/2;

	// center sample of first pixel:
	float pixLeft = (float)halfSupportX + halfPixelShiftX;
	float pixBottom = (float)halfSupportY + halfPixelShiftY;
	// center sample on last pixel: (do i need to subtract 1 from this?)
	float pixRight = (float)totalImageSamplesWidth - halfSupportX - halfPixelShiftX;
	float pixTop = (float)totalImageSamplesHeight - halfSupportY - halfPixelShiftY;

	// The clipping region we have
//	float sampleClipLeft		=	(float) (								-	halfSupportX);
//	float sampleClipRight		=	(float) (outputWidthPixels*samplesPerPixelX	+	halfSupportX);
//	float sampleClipBottom		=	(float) (								-	halfSupportY);
//	float sampleClipTop	=	(float) (outputHeightPixels*samplesPerPixelY	+	halfSupportY);


	float scrLeft = maFunctions::Interpolate(-1, 1, 0, pixLeft, pixRight);
	float scrRight = maFunctions::Interpolate(-1, 1, (float)totalImageSamplesWidth, pixLeft, pixRight);
	float scrBottom = maFunctions::Interpolate(-1, 1, 0, pixBottom, pixTop);
	float scrTop = maFunctions::Interpolate(-1, 1, (float)totalImageSamplesHeight, pixBottom, pixTop);

	// TILES (BUCKETS):

	int desiredTileSize = PREFERRED_TILE_SIZE;
	// Strip away the filter support, then figure out how many pixels fit.
	// Integer divide, truncates any fraction.
	// Then add the filter support back in.
	int actualTileSizeX = ((desiredTileSize - filterEdgeSupportX)/samplesPerPixelX) * samplesPerPixelX + filterEdgeSupportX;
	int actualTileSizeY = ((desiredTileSize - filterEdgeSupportY)/samplesPerPixelY) * samplesPerPixelY + filterEdgeSupportY;
	DBG_ASSERT(actualTileSizeX <= desiredTileSize, "Bad tile size");
	DBG_ASSERT((actualTileSizeX - filterEdgeSupportX) % samplesPerPixelX == 0, "Bad tile size");
	DBG_ASSERT(actualTileSizeY <= desiredTileSize, "Bad tile size");
	DBG_ASSERT((actualTileSizeY - filterEdgeSupportY) % samplesPerPixelY == 0, "Bad tile size");

	// how many final image pixels does this tile cover?
	m_TileSizeX = actualTileSizeX;
	m_TileSizeY = actualTileSizeY;

	int nFinalPixelsInTileX = (actualTileSizeX - filterEdgeSupportX)/samplesPerPixelX;
	int nFinalPixelsInTileY = (actualTileSizeY - filterEdgeSupportY)/samplesPerPixelY;
	m_TileFinalSizeX = nFinalPixelsInTileX;
	m_TileFinalSizeY = nFinalPixelsInTileY;

	// edge tiles might be smaller:
	int nTilesX = outputWidthPixels / nFinalPixelsInTileX;
	int nExtraPixelsX = outputWidthPixels % nFinalPixelsInTileX;
	if (nExtraPixelsX > 0)
	{
		nTilesX ++;
	}
	m_nTilesX = nTilesX;

	int nTilesY = outputHeightPixels / nFinalPixelsInTileY;
	int nExtraPixelsY = outputHeightPixels % nFinalPixelsInTileY;
	if (nExtraPixelsY > 0)
	{
		nTilesY ++;
	}
	m_nTilesY = nTilesY;

	DBG_LOG("rndr Image Size       : " << outputWidthPixels << "x" << outputHeightPixels);
	DBG_LOG("rndr Pixel Samples    : " << samplesPerPixelX << "x" << samplesPerPixelY);
	DBG_LOG("rndr Filter Width     : " << filterWidthPixelsX << "x" << filterWidthPixelsY);
	DBG_LOG("rndr Filter Samples   : " << m_KernelSamplesX << "x" << m_KernelSamplesY);
	DBG_LOG("rndr Tile Size        : " << actualTileSizeX << "x" << actualTileSizeY);
	DBG_LOG("rndr Tile Output Size : " << nFinalPixelsInTileX << "x" << nFinalPixelsInTileY);
	DBG_LOG("rndr Num Tiles        : " << m_nTilesX << "x" << m_nTilesY);
	DBG_LOG("----------------------------------");

	m_Tiles.clear();

	for (int i = 0; i < nTilesX * nTilesY; i++)
	{
		int row = i / nTilesX;
		int col = i % nTilesX;

		rndrTile tile;
		tile.m_Width = m_TileSizeX;
		tile.m_OutWidth = m_TileFinalSizeX;
		tile.m_OutX = col * m_TileFinalSizeX;
		tile.m_X = (col * m_TileFinalSizeX*samplesPerPixelX);
		if ((nExtraPixelsX > 0) && (col == nTilesX-1 ))
		{
			tile.m_Width = nExtraPixelsX * samplesPerPixelX + (filterEdgeSupportX);
			tile.m_OutWidth = nExtraPixelsX;
		}

		tile.m_Height = m_TileSizeY;
		tile.m_OutHeight = m_TileFinalSizeY;
		tile.m_OutY = row * m_TileFinalSizeY;
		tile.m_Y = (row * m_TileFinalSizeY*samplesPerPixelY);
		if ((nExtraPixelsY > 0) && (row == nTilesY-1 ))
		{
			tile.m_Height = nExtraPixelsY * samplesPerPixelY + (filterEdgeSupportY);
			tile.m_OutHeight = nExtraPixelsY;
		}
			
	// for now, let's render the full size tile 
	// and then capture a subsection of it for edge tiles?
		// (the screen coords are for the full tile, and not the clipped edge tile.)

		tile.m_ScreenLeft = maFunctions::Interpolate(scrLeft, scrRight, (float)tile.m_X, 0, (float)totalImageSamplesWidth);
//			tile.m_ScreenRight = maFunctions::Interpolate(scrLeft, scrRight, tile.m_X+tile.m_Width, 0, totalImageSamplesWidth);
		tile.m_ScreenRight = maFunctions::Interpolate(scrLeft, scrRight, (float)tile.m_X+m_TileSizeX, 0, (float)totalImageSamplesWidth);

		tile.m_ScreenBottom = maFunctions::Interpolate(scrBottom, scrTop, (float)tile.m_Y, 0, (float)totalImageSamplesHeight);
//			tile.m_ScreenBottom = maFunctions::Interpolate(scrBottom, scrTop, tile.m_Y+tile.m_Height, 0, totalImageSamplesHeight);
		tile.m_ScreenTop = maFunctions::Interpolate(scrBottom, scrTop, (float)tile.m_Y+m_TileSizeY, 0, (float)totalImageSamplesHeight);

		m_Tiles.push_back(tile);
	}


	// alternately, I could ask for a number of final pixels in my bucket/tile:
//		int desiredFinalPixelsInTile = 64;
//		actualTileSize = desiredFinalPixelsInTile * i_SamplesPerPixel + (filterEdgeSupport);
	// i can then modify the desiredFinalSize until i get an actualTileSize less than 1024 or whatever.
	// and so on...

	// note that the subviewport of this tile is now altered because of the filterEdgeSupport!
	// if the full frame went from screenspace -1..1 (0..w pixels) then we have to adjust for the extras.

	maFilter* filter = CreateFilter(i_Params.mFilterType);

	if (filter != NULL)
	{
		m_FilterKernelWeights.clear();
		m_SamplePoints.clear();

		// get random sample positions
		maSampling::GetSamples2D_Repeatable(m_KernelSamplesX, m_KernelSamplesY,
			m_SamplePoints, 1.0f, (float)samplesPerPixelX, (float)samplesPerPixelY);
		maSampling::WeightSamples(m_SamplePoints, filter, filterWidthPixelsX, filterWidthPixelsY);
		for (int i = 0; i < m_SamplePoints.size(); i++)
		{
			m_FilterKernelWeights.push_back(m_SamplePoints[i].m_Z);
		}

		mKernelBuffer = cl::Buffer(m_pDevice->mContext->clContext(), 
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    m_KernelSamplesX * m_KernelSamplesY * sizeof(float),
                                    &m_FilterKernelWeights[0]);

		delete filter;
	}
	
	if (m_KernelSamplesX != oldKernelSamplesX)
		compileConvolutionKernel();

	GLenum sampleFormat = GL_RGBA32F;
	// assumes bits is a multiple of 8!!
	int sampleSizeBytes = ogl::bitsPerPixel( sampleFormat )/8;

	// temporary: just fix the tile at main window size
//	m_TileSizeX = outputWidthPixels;
//	m_TileSizeY = outputHeightPixels;

	// one complete supersampled tile
	glGenTextures(1, &mTileTexture);
	glBindTexture(GL_TEXTURE_2D, mTileTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, sampleFormat, m_TileSizeX, m_TileSizeY, 0, GL_RGBA, GL_FLOAT, NULL);
	CHECKGLERROR();

	cl_int result;
	// hopefully this releases the prior one 
	mTileBuffer = cl::ImageGL();
	mTileBuffer = cl::ImageGL(m_pDevice->mContext->clContext(),
		CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, mTileTexture, &result);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("CL error creating ImageGLGL read only " << result);
	}

	// depth buffer for tile!
	GLenum depthFormat = GL_DEPTH24_STENCIL8;
	glGenRenderbuffers(1, &mTileDepthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mTileDepthRenderbuffer);
	// need stencil?
	glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, m_TileSizeX, m_TileSizeY);
	CHECKGLERROR();

	// tile as render target:
	glGenFramebuffers(1, &mTileFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mTileFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTileTexture, 0); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mTileDepthRenderbuffer); 
	CHECKGLERROR();

	// one downsampled + filtered tile
	glGenTextures(1, &mFilteredTileTexture);
	glBindTexture(GL_TEXTURE_2D, mFilteredTileTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, sampleFormat, m_TileFinalSizeX, m_TileFinalSizeY, 0, GL_RGBA, GL_FLOAT, NULL);

	// result tile as Buffer
	mFilteredBuffer = cl::ImageGL();
	mFilteredBuffer = cl::ImageGL(m_pDevice->mContext->clContext(),
		CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, mFilteredTileTexture, &result);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("CL error creating ImageGLGL write only " << result);
	}

	// filtered tile as framebuffer:
	glGenFramebuffers(1, &mFilteredTileFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFilteredTileFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFilteredTileTexture, 0); 
	CHECKGLERROR();

	// final image
	glGenTextures(1, &mFinalImageTexture);
	glBindTexture(GL_TEXTURE_2D, mFinalImageTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, sampleFormat, outputWidthPixels, outputHeightPixels, 0, GL_RGBA, GL_FLOAT, NULL);
	CHECKGLERROR();

	// filtered tile as framebuffer:
	glGenFramebuffers(1, &mFinalImageFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFinalImageFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalImageTexture, 0); 
	CHECKGLERROR();

	return true;
}

int rndrTiledRenderer::RenderNextTile(
	camCamera* i_Camera,
	g3dScene* i_Scene)
{
	rndrTile tile = m_Tiles[m_CurrentTile];

	glBindFramebuffer(GL_FRAMEBUFFER, mTileFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTileTexture, 0); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mTileDepthRenderbuffer); 
	CHECKGLFRAMEBUFFER(GL_FRAMEBUFFER);

	glClearColor(0,0,0,0);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glViewport(0,0,tile.m_Width, tile.m_Height);
	glViewport(0,0,m_TileSizeX, m_TileSizeY);

	camCamera* camera = mCamera;
	testScene* s = NULL;//Area18Layer::GetTestScene(m_pDevice);

	// Must restore this to default after all tiles done.
	const_cast<camCamera*>(camera)->SetSubViewport(tile.m_ScreenTop, tile.m_ScreenBottom, tile.m_ScreenLeft, tile.m_ScreenRight);

	if (s)
		s->Render(camera);
	else
	{
		rndrDrawScene draw;
		draw.Render(*mCamera, *i_Scene, m_pDevice);
	}

	// Restore camera viewport
	const_cast<camCamera*>(camera)->SetSubViewport(1, -1, -1, 1);

	// unset target so it can be used as an input later
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return 0;
}

std::string FileToString(const std::string fileName)
{
    std::ifstream f(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

    try
    {
        size_t size;
        char*  str;
        std::string s;

        if(f.is_open())
        {
            size_t fileSize;
            f.seekg(0, std::ifstream::end);
            size = fileSize = f.tellg();
            f.seekg(0, std::ifstream::beg);

            str = new char[size+1];
            if (!str) throw(std::string("Could not allocate memory"));

            f.read(str, fileSize);
            f.close();
            str[size] = '\0';
        
            s = str;
            delete [] str;
            return s;
        }
    }
    catch(std::string msg)
    {
        DBG_ERROR("Exception caught in FileToString(): " << msg);
        if(f.is_open())
            f.close();
    }
    catch(...)
    {
        DBG_ERROR("Exception caught in FileToString()");
        if(f.is_open())
            f.close();
    }
    std::string errorMsg = "FileToString()::Error: Unable to open file "
                            + fileName;
    throw(errorMsg);
}
void rndrTiledRenderer::compileConvolutionKernel()
{
	cl_int result;

    /////////////////////////////////////////////////////////////////
    // Load CL file, build CL program object, create CL kernel object
    /////////////////////////////////////////////////////////////////
    std::string  sourceStr = FileToString("D:/dev/CompletelyDifferent/LibXLT/Area18/rndr/Kernels.cl");

    cl::Program::Sources sources(1, std::make_pair(sourceStr.c_str(), sourceStr.length()));
    mConvolutionProgram = cl::Program(m_pDevice->mContext->clContext(), sources, &result);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("Error creating cl program " << result);
	}

    /* create a cl program executable with some #defines */
    char options[128];
    sprintf(options, "-DFILTER_WIDTH=%d", m_KernelSamplesX);
    result = mConvolutionProgram.build(m_pDevice->mContext->clDevices(), options);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("Error building cl program " << result);
		DBG_LOG(mConvolutionProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_pDevice->mContext->clDevices()[0], &result));
	}

    std::vector<std::string> kernelNames;
	kernelNames.push_back("IConvolve");
//    kernelNames.push_back("Convolve_Unroll");
//    kernelNames.push_back("Convolve_UnrollIf");
//    kernelNames.push_back("Convolve_Def");
//    kernelNames.push_back("Convolve_Def_Unroll");
//    kernelNames.push_back("Convolve_Def_UnrollIf");
//    kernelNames.push_back("Convolve_Float4");
//    kernelNames.push_back("Convolve_Float4If");
//    kernelNames.push_back("Convolve_Def_Float4");
//    kernelNames.push_back("Convolve_Def_Float4If");
    int nTotalKernels = kernelNames.size();
	mConvolutionKernels.clear();
    for (int nKernel = 0; nKernel < nTotalKernels; nKernel++)
    {
        /* get a kernel object handle for a kernel with the given name */
        cl::Kernel kernel = cl::Kernel(mConvolutionProgram, kernelNames[nKernel].c_str(), &result);
		if (result != CL_SUCCESS) 
		{
			DBG_LOG("Error building cl kernel " << nKernel << " error num " << result);
		}
        mConvolutionKernels.push_back(kernel);
    }
}

void saveRGBAF(GLuint fb, int x, int y, const char* filename)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
	float* pixels = new float[x*y*4];
	glReadPixels(0,0, x, y, GL_RGBA, GL_FLOAT, pixels);

	FreeImage_Initialise();
	FIBITMAP * dib = FreeImage_AllocateT(FIT_RGBAF, x, y);
	BYTE * dibbits = FreeImage_GetBits(dib);
	size_t z = sizeof(float) * x*y*4;
	memcpy(dibbits, pixels, z);
	BOOL saved = FreeImage_Save(FIF_EXR, dib, filename);
	FreeImage_Unload(dib);
	FreeImage_DeInitialise();

	delete [] pixels;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rndrTiledRenderer::FilterAndDownsampleTile(oglDevice* i_pDevice, int i_Index)
{
	rndrTile tile = m_Tiles[i_Index];
	if (tile.m_Height == tile.m_OutHeight && tile.m_Width == tile.m_OutWidth) 
	{
		// skip this step. 
		// this means we must read from mTile instead of mFilteredTile in the next step!
		return;
	}

#if 0
	// put filteredtiletexture into image (or direct to window)
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalImageFramebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mTileFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFilteredTileFramebuffer);
	glBlitFramebuffer(0, 0, tile.m_Width, tile.m_Height,
		0, 0, tile.m_OutWidth, tile.m_OutHeight, 
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
#endif	

	// TODO: why can't destination be mFinalImageTexture? 
	// more flexibility writing to temp output tile, can process that tile if needed?

	glFinish();

	cl::Event ev;
	std::vector<cl::Memory> memObjs;
	memObjs.clear();
	memObjs.push_back(mTileBuffer);
	memObjs.push_back(mFilteredBuffer);
	cl_int result = m_pDevice->mContext->clCommandQueue().enqueueAcquireGLObjects(&memObjs, NULL, NULL);//&ev);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("CL error enqueueAcquireGLObjects" << result);	
	}
	//ev.wait();

	int nKernel = 0; // which implementation to run
    /* input image */
    mConvolutionKernels[nKernel].setArg(0, mTileBuffer);

    /* filter */
    mConvolutionKernels[nKernel].setArg(1, mKernelBuffer);

    /* output image */
    mConvolutionKernels[nKernel].setArg(2, mFilteredBuffer);

    /* input image width*/
    mConvolutionKernels[nKernel].setArg(3, m_TileSizeX);
    mConvolutionKernels[nKernel].setArg(4, m_TileSizeY);

    /* filter width*/
    mConvolutionKernels[nKernel].setArg(5, m_KernelSamplesX);
    mConvolutionKernels[nKernel].setArg(6, m_KernelSamplesY);

    /* pixel samples*/
    mConvolutionKernels[nKernel].setArg(7, m_Params.mPixelSamplesX);
    mConvolutionKernels[nKernel].setArg(8, m_Params.mPixelSamplesY);

	m_pDevice->mContext->clCommandQueue().finish();

	// the shader has 16x16 threads per block.
	static const int shaderThreadsX = 16;
	static const int shaderThreadsY = 16;
	cl::NDRange globalRange = cl::NDRange(m_TileFinalSizeX, m_TileFinalSizeY);
	cl::NDRange localRange = cl::NullRange;// cl::NDRange(shaderThreadsX, shaderThreadsY);
    result = m_pDevice->mContext->clCommandQueue().enqueueNDRangeKernel(mConvolutionKernels[nKernel],
                                            cl::NDRange(),
                                            globalRange,
                                            localRange,
                                            0,
                                            NULL);//&ev);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("CL error enqueueNDRangeKernel " << result);	
	}
    /* wait for the kernel call to finish execution */
	//ev.wait();



	// now results are in the gl objects
	result = m_pDevice->mContext->clCommandQueue().enqueueReleaseGLObjects(&memObjs, NULL, NULL);//&ev);
	if (result != CL_SUCCESS) 
	{
		DBG_LOG("CL error enqueueReleaseGLObjects" << result);	
	}
	//ev.wait();

	m_pDevice->mContext->clCommandQueue().finish();

	// input mTileTexture and mKernelBuffer
	// output into mFilteredTileTexture (or mFilteredTileBuffer?)
//	saveRGBAF(mFilteredTileTexture, m_TileFinalSizeX, m_TileFinalSizeY, "E:\\temp.exr");
}

void rndrTiledRenderer::TextureToWindow(oglDevice* i_pDevice)
{
	rndrTile tile = m_Tiles[m_CurrentTile];
	GLuint readBuffer = 0;
	if (tile.m_Height == tile.m_OutHeight && tile.m_Width == tile.m_OutWidth) 
	{
		readBuffer = mTileFramebuffer;
	}
	else 
	{
		readBuffer = mFilteredTileFramebuffer;
	}

	// put filteredtiletexture into image (or direct to window)
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalImageFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer);
	glBlitFramebuffer(0, 0, tile.m_OutWidth, tile.m_OutHeight,
		tile.m_OutX, tile.m_OutY, tile.m_OutX+tile.m_OutWidth, tile.m_OutY+tile.m_OutHeight, 
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
bool rndrTiledRenderer::EndRender()
{
	m_CurrentTile = 0;
	return true;
}
