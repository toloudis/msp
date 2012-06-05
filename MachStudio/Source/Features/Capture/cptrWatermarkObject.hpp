/*****************************************************************************
**	cptrWatermarkObject.hpp
**
**	3D Object that holds the watermark for rendering
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CPTR_WATERMARKOBJECT_HPP
#error cptrWatermarkObject.hpp multiply included
#endif
#define CPTR_WATERMARKOBJECT_HPP


//============================================================================
//============================================================================
class api3dObjectSimple;
class fsLocator;
class g3dFragment;
class matTexture;


//============================================================================
//============================================================================
class cptrWatermarkObject 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrWatermarkObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cptrWatermarkObject();

	//--------------------------------------------------------------------
	//	Create and Add the watermark
	//--------------------------------------------------------------------
	void Initialize(const fsLocator& i_ResourceID);

	//--------------------------------------------------------------------
	//	Remove and Destroy the watermark
	//--------------------------------------------------------------------
	void DeInitialize();

private:
	g3dFragment*		m_pTextureFrag;
	api3dObjectSimple*	m_pTextureObject;
	matTexture*			m_pWatermarkTexture;
};
