/*****************************************************************************
**	cmraDriverRenderPass.hpp
**
**		Derived driver class for turning on camera capturing
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERRENDERPASS_HPP
#error cmraDriverRenderPass.hpp multiply included
#endif
#define CMRA_DRIVERRENDERPASS_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif

#include <vector>

//============================================================================
//============================================================================
class cmraDriverRenderPassInfo;
class tmlnChannelTextureFileName;
class tmlnChannelRenderPass;
class tmlnChannel;
class tmlnDriverInfo;

namespace cmraRenderPasses
{
	enum RenderPasses
	{
		e_AO = 0,
		e_GI = 1,
		e_Reflections = 2,
		e_ShadowMask = 3,
		e_Beauty = 4
	};
}

//============================================================================
//============================================================================
class cmraDriverRenderPass : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverRenderPass(tmlnChannelRenderPass& i_Channel, int i_RenderPass);

	//--------------------------------------------------------------------
	//  ~cmraDriverRenderPass
	//--------------------------------------------------------------------
	~cmraDriverRenderPass();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  SetFileReflectionMode()
	//--------------------------------------------------------------------
	void SetFileReflectionMode(bool i_Val);

	//--------------------------------------------------------------------
	//  SetBuffer
	//--------------------------------------------------------------------
	void SetBuffer();

	//--------------------------------------------------------------------
	//  Update
	//--------------------------------------------------------------------
	void Update();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(	const cmraDriverRenderPassInfo& i_Info );

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	// Compute frame of animation to use based on frame rate, 
	//	start/end frame, etc.
	//--------------------------------------------------------------------
	int compute_frame(const maTime& i_Time);

	//--------------------------------------------------------------------
	// Set Textures as list of texture names in numbered order.
	// i_FirstTextureFileName defines the pattern, digits before the
	// file extension are used to specify the numbering.
	//--------------------------------------------------------------------
	void set_textures( const fsLocator& i_FirstTextureFileName,
					   int i_NumFrames );

	//--------------------------------------------------------------------
	// Resize duration of driver to match length of animation if requested
	//--------------------------------------------------------------------
	void update_driver_length_from_anim_length();

	//--------------------------------------------------------------------
	//  SetBufferData()
	//--------------------------------------------------------------------
	void SetBufferData(prtyTextureFileName i_Tex);

	//--------------------------------------------------------------------
	//  GetCaptRenderPassBool()
	//--------------------------------------------------------------------
	bool GetCaptRenderPassBool();

	//--------------------------------------------------------------------
	// Callbacks
	//--------------------------------------------------------------------
	void TextureFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void BlendOpChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void BlendIntensityChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void EnableChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelRenderPass	&m_Channel;

	std::vector<fsLocator>	m_Textures;

	prtyTextureFileName		m_FirstTextureFileName;
	prtyTextureFileName		m_CurrTex;

	prtyInt32				m_NumberOfFrames;
	prtyInt32				m_RenderPass;

	prtyEnum				m_BlendOp;

	prtyBoolean				m_bEnable;

	prtyFloat				m_FrameRate;	
	prtyFloat				m_BlendIntensity;

	maTime					m_CurrTime;

	bool					m_OrigG3dReflVal;
};
