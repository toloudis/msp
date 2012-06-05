#ifdef SHDW_FRAMEBUFFERMGR_HPP
#error shdwFrameBufferMgr.hpp multiply included
#endif
#define SHDW_FRAMEBUFFERMGR_HPP

class g2dRenderTarget;
class matRenderTargetTexture;

class shdwFrameBufferMgr
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwFrameBufferMgr();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwFrameBufferMgr();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReleaseSurfaces();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CreateSurfaces(g2dRenderTarget* i_pWindow, int i_OverscanSize);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool IsAntiAliased() {return m_bAA;}

	//--------------------------------------------------------------------
	// Simple accessors for now. Could be updated to do refcounting to 
	// monitor buffer usage.
	//--------------------------------------------------------------------

	//All HDR rendering is done to this target
	//This target creates it's own depths to support MSAA
	matRenderTargetTexture* HDRRenderTarget()	{ return m_pHDRRenderTarget; }

	//This target is used for reading and must be resolved before use
	//This target currently doesn't support MSAA
	//This target doesn't have a depth buffer so we must set it to the 
	// framebuffer's so that resolves have something to write to.
	matRenderTargetTexture* HDRRenderTargetTex(){return m_pHDRRenderTargetTex;}

	//This target will be deprecated with the new MSAA changes
	matRenderTargetTexture* HDRAATarget()		{return m_pHDRAATarget;}

	matRenderTargetTexture* HDRAAScratchTarget(){return m_pHDRAATarget;}

	matRenderTargetTexture* DOFReserveTarget()	{return m_pDOFReserveTarget;}
	matRenderTargetTexture* DOFBlurTarget()		{return m_pDOFBlurTarget;}
	matRenderTargetTexture* DOFHBlurTarget()	{return m_pDOFHBlurTarget;}

	matRenderTargetTexture* HDRBufferBackup()	{return m_pHDRBufferBackup;}
	matRenderTargetTexture* HDRScratchTex0()	{return m_pHDRScratchTex0;}
	matRenderTargetTexture* HDRScratchTex1()	{return m_pHDRScratchTex1;}
	matRenderTargetTexture* HDRScratchTex2()	{return m_pHDRScratchTex2;}

	matRenderTargetTexture* DepthBuffer()		{return m_pDepthBuffer;}
	matRenderTargetTexture* DepthBuffer2()		{return m_pDepthBuffer2;}
	matRenderTargetTexture* MultiDepthBuffer()	{return m_pMultiDepthBuffer;}
	matRenderTargetTexture* NormalsBuffer()		{return m_pNormalsBuffer;}
	matRenderTargetTexture* VelocityBuffer()	{return m_pVelocityBuffer;}

	matRenderTargetTexture* TransDepthBuffer1()	{return m_pTransDepthBuffer1;}
	matRenderTargetTexture* TransDepthBuffer2()	{return m_pTransDepthBuffer2;}
	matRenderTargetTexture* TransDepthAux()		{return m_pTransDepthAux;}

	matRenderTargetTexture* HDRScaledTex()		{return m_pHDRScaledTex;}
	matRenderTargetTexture* TexBrightPass()		{return m_pTexBrightPass;}

	matRenderTargetTexture* TexBloomSource()	{return m_pTexBloomSource;}
	matRenderTargetTexture* TexStarSource()		{return m_pTexStarSource;}

	//--------------------------------------------------------------------
	// Number of stages in the 4x4 down-scaling of average luminance textures
	//--------------------------------------------------------------------
#define NUM_TONEMAP_TEXTURES  4       
	int GetNumTonemapTextures() {return NUM_TONEMAP_TEXTURES;}
	struct shdwToneMapTex
	{
		matRenderTargetTexture* m_apTexToneMap[NUM_TONEMAP_TEXTURES];
	};
	/*const*/ shdwToneMapTex& TexToneMap()		{return m_TexToneMap;}

	//--------------------------------------------------------------------
	// Number of textures used for the bloom post-processing effect
	//--------------------------------------------------------------------
#define NUM_BLOOM_TEXTURES    3       
	int GetNumBloomTextures()	{return NUM_BLOOM_TEXTURES;}
	struct shdwBloomTex
	{
		matRenderTargetTexture* m_apTexBloom[NUM_BLOOM_TEXTURES];
	};
	/*const*/ shdwBloomTex& TexBloom()			{return m_TexBloom;}

	//--------------------------------------------------------------------
	// Number of textures used for the star post-processing effect
	//--------------------------------------------------------------------
#define NUM_STAR_TEXTURES     12      
	int GetNumStarTextures()	{return NUM_STAR_TEXTURES;}
	struct shdwStarTex
	{
		matRenderTargetTexture* m_apTexStar[NUM_STAR_TEXTURES];
	};
	/*const*/ shdwStarTex& TexStar()			{return m_TexStar;}

	//--------------------------------------------------------------------
	// Opacity shadow maps for hair rendering
	//--------------------------------------------------------------------
	struct shdwOSM
	{
		matRenderTargetTexture* m_pOSM[4];	//RGBA8 Opacity Shadow Map (used in shdwPassHair)
	};
	/*const*/ shdwOSM& OSM()					{return m_OSM;}

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void InitMultisample();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanupMultisample();

	int m_Width, m_Height;
//	int m_nHairShadowMapRes;
//	int m_nHairShadowMapType;
	bool m_bAA;

	matRenderTargetTexture* m_pHDRRenderTarget;		//primary HDR target (only one that can have MSAA)
	matRenderTargetTexture* m_pHDRRenderTargetTex;  //non MSAA target that can be used as a texture source
	matRenderTargetTexture* m_pHDRAATarget;

	matRenderTargetTexture* m_pDepthBuffer;
	matRenderTargetTexture* m_pDepthBuffer2;
	matRenderTargetTexture* m_pMultiDepthBuffer;	//depth peeled (nearest in r)
	matRenderTargetTexture* m_pNormalsBuffer;
	matRenderTargetTexture* m_pVelocityBuffer;
	
	matRenderTargetTexture* m_pHDRScratchTex0;
	matRenderTargetTexture* m_pHDRScratchTex1;
	matRenderTargetTexture* m_pHDRScratchTex2;

	matRenderTargetTexture* m_pHDRScaledTex;
	matRenderTargetTexture* m_pTexBrightPass;

	matRenderTargetTexture* m_pTransDepthBuffer1;
	matRenderTargetTexture* m_pTransDepthBuffer2;
	matRenderTargetTexture* m_pTransDepthAux;			//used as a copy for reverse peeling (only depth used)

	shdwOSM m_OSM;

	shdwToneMapTex m_TexToneMap;

	shdwBloomTex m_TexBloom;
	matRenderTargetTexture* m_pTexBloomSource;

	shdwStarTex m_TexStar;
	matRenderTargetTexture* m_pTexStarSource;

	matRenderTargetTexture* m_pHDRBufferBackup;

	matRenderTargetTexture* m_pDOFReserveTarget;
	matRenderTargetTexture* m_pDOFHBlurTarget;
	matRenderTargetTexture* m_pDOFBlurTarget;
};
