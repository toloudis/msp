/*****************************************************************************
**	rmpTextureMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpTextureMgr.hpp"

#include "Support/rmp/private/rmpTextureUtil.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpDialogUtil.hpp"
#include "Support/rmp/rmpObject.hpp"

#include "Graphics/eff/effRampData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/gpx/gpxCamera.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace rmpTextureMgr
{
	namespace
	{
		typedef struct
		{
			g3dSceneRenderer* pRampSceneRenderer;
			g3dTargetRenderer* pRampTargetRenderer;
			g3dFragment*		pRampTextureFrag;
			api3dObjectSimple*	pRampTextureObject;
			g3dSceneNode* pRampSceneRoot;
			g3dSceneNode* pRampSceneRootFirstLayer;
			g3dScene* pRampScene;
			matTexture* pRampTextureSrc;
			//matTexture* pRampNoiseMap;
		}RampRendererGroup;

		std::vector<RampRendererGroup*> l_pRampRendererList;
		camCamera l_rampCamera;
		int l_RampTextureSize;

		//--------------------------------------------------------------------
		// Create 1d texture according to maGradient
		//--------------------------------------------------------------------
		int l_GradientTextureSize = 512;;
		int l_NoiseTextureSize = 64;
		float* l_GradientTextureBuffer = new float[l_GradientTextureSize * 4];
		float* l_NoiseTextureBuffer = new float[l_GradientTextureSize * l_GradientTextureSize];
		g2dPFD l_GradientPFD(g2dPFD::e_RGBA32f, 32 * 4);
		g2dPFD l_RampTexturePFD(g2dPFD::e_Color, 32);
		g2dPFD l_NoiseTexturePFD(g2dPFD::e_Float32, 32);
		matTexture* l_pRampNoiseMap;

		void CreateNoiseTexture(matTexture* o_NoiseMap, float i_NoiseFreq);


		// Interpolation method are implemented according to autodesk maya definition
		// No interpolation
		void FillColor_None(const int i_StartPixel, const int i_BlockSize, const maFloatRGBA& i_Color)
		{
			int currentPixel = i_StartPixel;

			while(currentPixel < i_StartPixel + i_BlockSize)
			{
				l_GradientTextureBuffer[currentPixel * 4] = i_Color.GetRed();
				l_GradientTextureBuffer[currentPixel * 4 + 1] = i_Color.GetGreen();
				l_GradientTextureBuffer[currentPixel * 4 + 2] = i_Color.GetBlue();
				l_GradientTextureBuffer[currentPixel * 4 + 3] = i_Color.GetAlpha();
				currentPixel ++;
			}
		}
		
		// Linear interpolation
		void FillColor_Linear(const int i_StartPixel, const int i_BlockSize, const maFloatRGBA& i_Color1, const maFloatRGBA& i_Color2)
		{
			int currentPixel = i_StartPixel;
			
			while(currentPixel < i_StartPixel + i_BlockSize)
			{
				float w1 = (float)(currentPixel - i_StartPixel) / (float)i_BlockSize;
				//float w2 = (float)(i_StartPixel + i_BlockSize - currentPixel) / (float)i_BlockSize;
				float w2 = 1 - w1;
				maFloatRGBA c = (i_Color2 * w1 + i_Color1 * w2) ;
				l_GradientTextureBuffer[currentPixel * 4] = c.GetRed();
				l_GradientTextureBuffer[currentPixel * 4 + 1] = c.GetGreen();
				l_GradientTextureBuffer[currentPixel * 4 + 2] = c.GetBlue();
				l_GradientTextureBuffer[currentPixel * 4 + 3] = c.GetAlpha();
				currentPixel ++;
			}
		}

#ifndef PI
#define PI 3.14159265f
#endif // PI
		// Smooth interpolation
		void FillColor_Smooth(const int i_StartPixel, const int i_BlockSize, const maFloatRGBA& i_Color1, const maFloatRGBA& i_Color2)
		{
			int currentPixel = i_StartPixel;
			
			while(currentPixel < i_StartPixel + i_BlockSize)
			{
				float w1 = (float)(currentPixel - i_StartPixel) / (float)i_BlockSize;
				w1 = 0.5f * (cos((w1+1)*PI) + 1);
				float w2 = 1-w1;
				maFloatRGBA c = (i_Color2 * w1 + i_Color1 * w2) ;
				l_GradientTextureBuffer[currentPixel * 4] = c.GetRed();
				l_GradientTextureBuffer[currentPixel * 4 + 1] = c.GetGreen();
				l_GradientTextureBuffer[currentPixel * 4 + 2] = c.GetBlue();
				l_GradientTextureBuffer[currentPixel * 4 + 3] = c.GetAlpha();
				currentPixel ++;
			}
		}

		// Bump interpolation
		void FillColor_Bump(const int i_StartPixel, const int i_BlockSize, const maFloatRGBA& i_Color1, const maFloatRGBA& i_Color2)
		{
			int currentPixel = i_StartPixel;
			double h1, s1, l1, h2, s2, l2;
			rmpTextureUtil::RGB_to_HSL(i_Color1.m_Red, i_Color1.m_Green, i_Color1.m_Blue, h1, s1, l1);
			rmpTextureUtil::RGB_to_HSL(i_Color2.m_Red, i_Color2.m_Green, i_Color2.m_Blue, h2, s2, l2);
			
			while(currentPixel < i_StartPixel + i_BlockSize)
			{
				float w1 = (float)(currentPixel - i_StartPixel) / (float)i_BlockSize;
				w1 = (l1 > l2) ? sin(w1 * (PI / 2.0f)) : sin((w1 - 1) * (PI / 2.0f)) + 1; 
				float w2 = 1 - w1;
				maFloatRGBA c = (i_Color2 * w1 + i_Color1 * w2) ;
				l_GradientTextureBuffer[currentPixel * 4] = c.GetRed();
				l_GradientTextureBuffer[currentPixel * 4 + 1] = c.GetGreen();
				l_GradientTextureBuffer[currentPixel * 4 + 2] = c.GetBlue();
				l_GradientTextureBuffer[currentPixel * 4 + 3] = c.GetAlpha();
				currentPixel ++;
			}
		}

		// Spike interpolation
		void FillColor_Spike(const int i_StartPixel, const int i_BlockSize, const maFloatRGBA& i_Color1, const maFloatRGBA& i_Color2)
		{
			int currentPixel = i_StartPixel;
			double h1, s1, l1, h2, s2, l2;
			rmpTextureUtil::RGB_to_HSL(i_Color1.m_Red, i_Color1.m_Green, i_Color1.m_Blue, h1, s1, l1);
			rmpTextureUtil::RGB_to_HSL(i_Color2.m_Red, i_Color2.m_Green, i_Color2.m_Blue, h2, s2, l2);
			
			while(currentPixel < i_StartPixel + i_BlockSize)
			{
				float w1 = (float)(currentPixel - i_StartPixel) / (float)i_BlockSize;
				w1 = (l1 < l2) ? sin(w1 * (PI / 2.0f)) : sin((w1 - 1) * (PI / 2.0f)) + 1; 
				float w2 = 1 - w1;
				maFloatRGBA c = (i_Color2 * w1 + i_Color1 * w2) ;
				l_GradientTextureBuffer[currentPixel * 4] = c.GetRed();
				l_GradientTextureBuffer[currentPixel * 4 + 1] = c.GetGreen();
				l_GradientTextureBuffer[currentPixel * 4 + 2] = c.GetBlue();
				l_GradientTextureBuffer[currentPixel * 4 + 3] = c.GetAlpha();
				currentPixel ++;
			}
		}

		void CreateGradientTexture(const maGradient& i_Gradient, matTexture* o_Texture, int i_Interpolation)
		{
			gpxRenderControl::ConfirmSingleThread();

			int currentPixel = 0;

			// Fill color from position 0 to first color node
			int blockSize = (int)(i_Gradient[0].first * l_GradientTextureSize - currentPixel);
			FillColor_None(currentPixel, blockSize, i_Gradient[0].second);
			currentPixel += blockSize;


			int i;
			switch(i_Interpolation)
			{
			case rmpData::RT_LINEAR:
				for (i = 0; i < i_Gradient.GetSize() - 1; i++)
				{
					blockSize = (int)((i_Gradient[i + 1].first - i_Gradient[i].first) * l_GradientTextureSize);
					FillColor_Linear(currentPixel, blockSize , i_Gradient[i].second, i_Gradient[i+1].second);
					currentPixel += blockSize;
				}
				break;
			case rmpData::RT_NONE:
				for (i = 0; i < i_Gradient.GetSize() - 1; i++)
				{
					blockSize = (int)((i_Gradient[i + 1].first - i_Gradient[i].first) * l_GradientTextureSize);
					FillColor_None(currentPixel, blockSize , i_Gradient[i].second);
					currentPixel += blockSize;
				}
				break;
			case rmpData::RT_SMOOTH:
				for (i = 0; i < i_Gradient.GetSize() - 1; i++)
				{
					blockSize = (int)((i_Gradient[i + 1].first - i_Gradient[i].first) * l_GradientTextureSize);
					FillColor_Smooth(currentPixel, blockSize , i_Gradient[i].second, i_Gradient[i+1].second);
					currentPixel += blockSize;
				}
				break;
			case rmpData::RT_BUMP:
				for (i = 0; i < i_Gradient.GetSize() - 1; i++)
				{
					blockSize = (int)((i_Gradient[i + 1].first - i_Gradient[i].first) * l_GradientTextureSize);
					FillColor_Bump(currentPixel, blockSize , i_Gradient[i].second, i_Gradient[i+1].second);
					currentPixel += blockSize;
				}
				break;
			case rmpData::RT_SPIKE:
				for (i = 0; i < i_Gradient.GetSize() - 1; i++)
				{
					blockSize = (int)((i_Gradient[i + 1].first - i_Gradient[i].first) * l_GradientTextureSize);
					FillColor_Spike(currentPixel, blockSize , i_Gradient[i].second, i_Gradient[i+1].second);
					currentPixel += blockSize;
				}
				break;
			};

			// Fill color from the last color node to width of texture
			blockSize = l_GradientTextureSize - currentPixel;
			FillColor_None(currentPixel, blockSize, i_Gradient[i_Gradient.GetSize () - 1].second);
			currentPixel += blockSize;

			matTextureMgr::FillTexture(o_Texture, l_GradientTextureBuffer, l_GradientTextureSize * 4 * sizeof(float));
		}

		//--------------------------------------------------------------------
		//  Create ramp renderer group
		//--------------------------------------------------------------------
		RampRendererGroup* CreateRampRendererGroup()
		{
			gpxRenderControl::ConfirmSingleThread();

			RampRendererGroup* rampRenderer = new RampRendererGroup();
			rampRenderer->pRampSceneRenderer = g3dSceneRendererCreate::CreateTextureRenderer();
			rampRenderer->pRampSceneRoot = new g3dSceneNode();
			rampRenderer->pRampSceneRootFirstLayer = new g3dSceneNode();

			rampRenderer->pRampScene = new g3dScene(
				new g3dLayer(rampRenderer->pRampSceneRootFirstLayer, g3dLayer::e_ZSort, 
				g3dLayer::e_Screen, g3dLayer::e_Multiplicative, 
				true, true)); // scene owns layers
			rampRenderer->pRampScene->AppendLayer(
				new g3dLayer(rampRenderer->pRampSceneRoot, g3dLayer::e_ZSort, 
				g3dLayer::e_Screen, g3dLayer::e_Multiplicative, 
				true, true));

			rampRenderer->pRampTargetRenderer = NULL;
			rampRenderer->pRampTargetRenderer = new g3dTargetRenderer();//l_pRampRenderTarget, l_pRampSceneRenderer);
			//rampRenderer->pRampTargetRenderer->SetRenderer(rampRenderer->pRampSceneRenderer);
			//rampRenderer->pRampTargetRenderer->SetBackgroundColor(g2dRGBColor(0x00, 0x00, 0x00));
			//rampRenderer->pRampTargetRenderer->SetScene(rampRenderer->pRampScene);
			//rampRenderer->pRampTargetRenderer->SetCamera(&l_rampCamera);

			// Setup ramp quad in the scene
			float fragWidth = 2.0, fragHeight = 2.0;
			rampRenderer->pRampTextureFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(
				fragWidth, fragWidth, 1, 1, false);
			rampRenderer->pRampTextureFrag->SetDoubleSided(false);


			matMaterial * pTexMat = new matMaterial("Ramp.fx");
			pTexMat->SetHasSpecular( false );

			// Set up fragment in object
			rampRenderer->pRampTextureObject = new api3dObjectSimple(rampRenderer->pRampTextureFrag, pTexMat);
			rampRenderer->pRampTextureObject->SetGPUPickable(false);
			rampRenderer->pRampTextureObject->SetPosition(maPoint3d(0, 0, 0)); // center of the screen

			// Add object into scenegraph
			rampRenderer->pRampSceneRoot->AddChild(rampRenderer->pRampTextureObject->Object()->GetBase());
			rampRenderer->pRampTextureObject->SetRenderable( true ); // not visible until we get a texture

			rampRenderer->pRampTextureSrc = matTextureMgr::CreateTexture(l_GradientTextureSize, 1, &l_GradientPFD);

			l_pRampRendererList.push_back(rampRenderer);
			return rampRenderer;
		}

		//--------------------------------------------------------------------
		//  Get free ramp renderer group
		//--------------------------------------------------------------------
		RampRendererGroup* GetRampRendererGroup()
		{
			RampRendererGroup* rampRenderer = NULL;
			
			// scan target renderer list to see if there is an unused one
			// todo: delete unused resource once a while
			for (int i = 0; i < l_pRampRendererList.size(); i++)
			{
				if (!api3dTargetRendererMgr::IsTargetRendererExist(l_pRampRendererList[i]->pRampTargetRenderer))
				{	
					if (l_pRampRendererList[i]->pRampTargetRenderer)
						delete l_pRampRendererList[i]->pRampTargetRenderer;
					l_pRampRendererList[i]->pRampTargetRenderer = new g3dTargetRenderer();
					rampRenderer = l_pRampRendererList[i];
				}
			}

			if (!rampRenderer)
			{
				rampRenderer = CreateRampRendererGroup();
			}

			rampRenderer->pRampTargetRenderer->SetRenderer(rampRenderer->pRampSceneRenderer);
			rampRenderer->pRampTargetRenderer->SetBackgroundColor(g2dRGBColor(0x00, 0x00, 0x00));
			rampRenderer->pRampTargetRenderer->SetScene(rampRenderer->pRampScene);
			rampRenderer->pRampTargetRenderer->SetCamera(&l_rampCamera);

			return rampRenderer;
		}

		//--------------------------------------------------------------------
		//  remove the render target in render queue if the render target
		//	is from given texture
		//  Note: MUST call this function before changes to ramp texture (size)
		//--------------------------------------------------------------------
		void CleanTargetRendererInRenderQueue(matTexture* i_RampTexture)
		{
			if (!i_RampTexture)
				return;

			gpxRenderControl::ConfirmSingleThread();
			for (int i = 0; i < l_pRampRendererList.size(); i++)
			{
				if (l_pRampRendererList[i]->pRampTargetRenderer->GetTarget() == i_RampTexture->GetRenderTargetAPI())
				{
					api3dTargetRendererMgr::RemoveTargetRenderer(l_pRampRendererList[i]->pRampTargetRenderer);
				}
			}
		}

		//--------------------------------------------------------------------
		//  Functions for generating 2D noise
		//--------------------------------------------------------------------
		float Noise2D(int x, int y)
		{
			int t;
			t = x+y*57;
			t = (t<<13)^t;	
			float result = ( 1.0f - ( (t * (t * t * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);	
			return result;  
		}

		float SmoothedNoise2D(int x , int y)
		{
			float corners = ( Noise2D(x-1, y-1)+Noise2D(x+1, y-1)+Noise2D(x-1, y+1)+Noise2D(x+1, y+1) ) / 16.0f;
			float sides   = ( Noise2D(x-1, y)  +Noise2D(x+1, y)  +Noise2D(x, y-1)  +Noise2D(x, y+1) ) /  8.0f;
			float center  =  Noise2D(x, y) / 4.0f;
			return corners + sides + center;
		}

		/*float Cosine_Interpolate(float  a, float b, float x)
		{
			float ft = x * PI;
			float f = (1 - cos(ft)) * 0.5f;

			return  a*(1-f) + b*f;
		}

		float StaticNoise2D(float x,float y)
		{
			int integer_X    = (int) x;
			float fractional_X = x - integer_X;

			int integer_Y    = (int) y;
			float fractional_Y = y - integer_Y;

			float v1 = SmoothedNoise2D(integer_X,     integer_Y);
			float v2 = SmoothedNoise2D(integer_X + 1, integer_Y);
			float v3 = SmoothedNoise2D(integer_X,     integer_Y + 1);
			float v4 = SmoothedNoise2D(integer_X + 1, integer_Y + 1);

			float i1= Cosine_Interpolate(v1 , v2 , fractional_X);
			float i2= Cosine_Interpolate(v3 , v4 , fractional_X);	  

			return  Cosine_Interpolate(i1 , i2 , fractional_Y);	  
		}*/

		void CreateNoiseTexture(matTexture* o_NoiseMap, float i_NoiseFreq)
		{
			for (int u = 0; u < l_GradientTextureSize; u++)
			{
				for (int v = 0; v < l_GradientTextureSize; v++)
				{
					l_NoiseTextureBuffer[u * l_GradientTextureSize + v] = SmoothedNoise2D(u, v);
				}
			}
			matTextureMgr::FillTexture(o_NoiseMap, l_NoiseTextureBuffer, l_NoiseTextureSize * l_NoiseTextureSize * sizeof(float));
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Setup the initial operations for the ramp texture
	//--------------------------------------------------------------------
	void SetupRampTexture(effParamTexture* i_pParamTexture)
	{
		gpxRenderControl::ConfirmSingleThread();

		matTexture* pRampTex = NULL;
		//Get the param's ramp property object and set the dialog mgr with the new object
		rmpObject* pRampObject = dynamic_cast<rmpObject*>(i_pParamTexture->GetRampProperty());
		
		if(!pRampObject)
			return;

		pRampTex = CreateRampTexture(pRampObject->m_Data.m_TexSize.GetValue());
		
		//Enable the first instance of the ramp
		EnableRampTexture(pRampObject->m_Data, pRampTex);

		//Set the param texture
		i_pParamTexture->SetTexture(pRampTex);

		//Set Dialog properties
		rmpDialogMgr::SetRampChangedCallback(NULL);
		rmpDialogMgr::SetData(pRampObject->m_Data);
	}

	//--------------------------------------------------------------------
	// cleanup the ramp texture
	//--------------------------------------------------------------------
	void CleanupRampTexture()
	{	
		//clear the dialog callback
		rmpDialogMgr::SetDataObject(NULL);
		rmpDialogMgr::SetRampChangedCallback(NULL);
	}

	//--------------------------------------------------------------------
	//  Enable Ramp Texture
	//--------------------------------------------------------------------
	void EnableRampTexture(const rmpData& i_RampData, matTexture* i_RampTexture)
	{
		gpxRenderControl::ConfirmSingleThread();
		
		RampRendererGroup* rampRenderer = GetRampRendererGroup();

		const matMaterial * pTexMat = rampRenderer->pRampTextureObject->GetMaterial();
		effRampData* pData = dynamic_cast<effRampData*>(pTexMat->GetEffectData());
		DBG_ASSERT(pData != NULL, "rmpTextureMgr not using effRampData");

		if (!i_RampTexture)
			return;
		
		if (l_GradientTextureSize != i_RampTexture->GetWidth())
		{
			l_GradientTextureSize = i_RampTexture->GetWidth();

			delete[] l_GradientTextureBuffer;
			l_GradientTextureBuffer = NULL;
			l_GradientTextureBuffer = new float[l_GradientTextureSize * 4];

			/*delete[] l_NoiseTextureBuffer;
			l_NoiseTextureBuffer = NULL;
			l_NoiseTextureBuffer = new float[l_GradientTextureSize * l_GradientTextureSize];*/
			
			/*matTextureMgr::ReleaseTexture(rampRenderer->pRampNoiseMap);
			rampRenderer->pRampNoiseMap = NULL;
			rampRenderer->pRampNoiseMap = matTextureMgr::CreateTexture(l_GradientTextureSize, l_GradientTextureSize, &l_NoiseTexturePFD);
			CreateNoiseTexture(rampRenderer->pRampNoiseMap, pData->m_NoiseFreq);*/
		}

		if (l_GradientTextureSize != rampRenderer->pRampTextureSrc->GetWidth())
		{
			matTextureMgr::ReleaseTexture(rampRenderer->pRampTextureSrc);
			rampRenderer->pRampTextureSrc = NULL;
			rampRenderer->pRampTextureSrc = matTextureMgr::CreateTexture(l_GradientTextureSize, 1, &l_GradientPFD);
		}

		rampRenderer->pRampTargetRenderer->SetTarget(i_RampTexture->GetRenderTargetAPI());

		pData->m_Gradient = i_RampData.m_Gradient.GetValue();
		pData->m_RampShape = i_RampData.m_Shape.GetValue();
		pData->m_RampInterpolation = i_RampData.m_Interpolation.GetValue();
		pData->m_UWave = i_RampData.m_UWave.GetValue();
		pData->m_UWaveFreq = i_RampData.m_UWaveFreq.GetValue();
		pData->m_VWave = i_RampData.m_VWave.GetValue();
		pData->m_VWaveFreq = i_RampData.m_VWaveFreq.GetValue();
		pData->m_Noise = i_RampData.m_Noise.GetValue();
		pData->m_NoiseFreq = i_RampData.m_NoiseFreq.GetValue();

		CreateGradientTexture(i_RampData.m_Gradient.GetValue(), rampRenderer->pRampTextureSrc, i_RampData.m_Interpolation.GetValue());
		pData->m_Texture = rampRenderer->pRampTextureSrc;

		
		pData->m_NoiseTexture = l_pRampNoiseMap;

		api3dTargetRendererMgr::AddTargetRenderer(rampRenderer->pRampTargetRenderer, api3dTargetRendererMgr::e_Texture);
	}

	//--------------------------------------------------------------------
	//  Create ramp texture
	//--------------------------------------------------------------------
	matTexture* CreateRampTexture(const int i_Size)
	{
		return matTextureMgr::CreateRenderTargetTexture(i_Size, i_Size, false,  NULL);
	}

	//--------------------------------------------------------------------
	//  Release ramp texture
	//--------------------------------------------------------------------
	void ReleaseRampTexture(matTexture* io_texture)
	{
		if(io_texture == NULL)
			return;

		gpxRenderControl::ConfirmSingleThread();
		CleanTargetRendererInRenderQueue(io_texture);
		matTextureMgr::ReleaseTexture(io_texture);
		io_texture = NULL;
	}

	//--------------------------------------------------------------------
	//  Setup ramp scene
	//--------------------------------------------------------------------
	void Initialize()
	{

		// Set up camera
		maPoint3d camPos(0.0f, 0.0f, 1.0f);
		maPoint3d camTarg(0.0f, 0.0f, 0.0f);
		maPoint3d camUp(0,1,0);

		l_rampCamera.LookAt(camPos, camTarg, camUp);
		l_rampCamera.SetAspect(512, 512);
		l_rampCamera.SetClip(0.3f, 100.0f);
		l_rampCamera.SetFOV(55);
		l_rampCamera.Set();

		l_pRampNoiseMap = matTextureMgr::CreateTexture(l_NoiseTextureSize, l_NoiseTextureSize, &l_NoiseTexturePFD);
		CreateNoiseTexture(l_pRampNoiseMap, 0);
	}
	
	//--------------------------------------------------------------------
	//  Cleanup ramp scene
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		/*if(m_pRampTexture)
			matTextureMgr::ReleaseTexture(m_pRampTexture);

		if(m_pRampTextureSrc)
			matTextureMgr::ReleaseTexture(m_pRampTextureSrc);*/

		if (l_GradientTextureBuffer)
		{
			delete[] l_GradientTextureBuffer;
			l_GradientTextureBuffer = NULL;
		}

		if (l_NoiseTextureBuffer)
		{
			delete[] l_NoiseTextureBuffer;
			l_NoiseTextureBuffer = NULL;
		}

		if (l_pRampNoiseMap)
		{
			matTextureMgr::ReleaseTexture(l_pRampNoiseMap);
			l_pRampNoiseMap = NULL;
		}

		for (int i = 0; i < l_pRampRendererList.size(); i++)
		{
			delete l_pRampRendererList[i]->pRampTextureObject;
			l_pRampRendererList[i]->pRampTextureObject = NULL;
			l_pRampRendererList[i]->pRampTextureFrag = NULL;

			delete l_pRampRendererList[i]->pRampSceneRenderer;
			l_pRampRendererList[i]->pRampSceneRenderer = NULL;
			delete l_pRampRendererList[i]->pRampScene;
			l_pRampRendererList[i]->pRampScene = NULL;
			delete l_pRampRendererList[i]->pRampSceneRoot;
			l_pRampRendererList[i]->pRampSceneRoot = NULL;
			delete l_pRampRendererList[i]->pRampSceneRootFirstLayer;
			l_pRampRendererList[i]->pRampSceneRootFirstLayer = NULL;

			api3dTargetRendererMgr::RemoveTargetRenderer(l_pRampRendererList[i]->pRampTargetRenderer);
			delete l_pRampRendererList[i]->pRampTargetRenderer;
			l_pRampRendererList[i]->pRampTargetRenderer = NULL;

			matTextureMgr::ReleaseTexture(l_pRampRendererList[i]->pRampTextureSrc);
			l_pRampRendererList[i]->pRampTextureSrc = NULL;

			/*matTextureMgr::ReleaseTexture(l_pRampRendererList[i]->pRampNoiseMap);
			l_pRampRendererList[i]->pRampNoiseMap = NULL;*/

			delete l_pRampRendererList[i];
			l_pRampRendererList[i] = NULL;
		}

		l_pRampRendererList.clear();
	}

}	// end of namespace
