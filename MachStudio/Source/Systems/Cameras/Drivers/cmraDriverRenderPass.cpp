/*****************************************************************************
**	cmraDriverRenderPass.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverRenderPass.hpp"

#include "Systems/Cameras/Drivers/cmraDriverRenderPassInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverRenderPassParser.hpp"

#include "Support/mnm/mnmDebugInfo.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnChannelRenderPass.hpp"

//#include "Core/fs/fsResourceTracker.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"

#include "Tool/gpx/gpxRenderControl.hpp"

#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"

//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	// find_number_pattern - take a filename like image001.jpg and break it
	//	apart so that the number can be incremented. 
	//  o_Format will receive a pattern for use in sprintf()
	//  In this example (i_Filename == "image001.jpg"), the results
	//	would be:
	//		o_Prefix = "image"
	//		o_InitNum = 1;
	//		o_Format = "%03d"
	//		o_Extension = ".jpg"
	//----------------------------------------------------------------------------

	void find_number_pattern(const itString &i_Filename, 
							 itString &o_Prefix, 
							 int &o_InitNum, 
							 std::string &o_Format, 
							 itString &o_Extension)
	{
		int length = i_Filename.GetLength();

		// Break out extension
		o_Extension = "";
		int pos = length-1;
		if (i_Filename.HasSubString(itString(".")))
		{
			for (; pos>=0; pos--)
			{
				o_Extension.InsertCharAt(0, i_Filename[pos]);
				if (i_Filename[pos] == '.')
				{
					pos--;
					break;
				}
			}
		}

		// Count number of digits
		int digits_end_pos = pos;
		int digit_count = 0;
		for (; pos>=0; pos--)
		{
			itString::CharType ch = i_Filename[pos];
			if (ch >= '0' && ch <= '9')
				digit_count++;
			else
				break;
		}

		// Get the first number of the count
		o_InitNum = 0;
		if (length > 0)
		{
			int index = 0;
			char numpart[12];
			for (int ccnt = digits_end_pos-digit_count+1; ccnt <= digits_end_pos; ccnt++)
				numpart[index++] = (char) i_Filename[ccnt];
			sscanf(numpart,"%d", &o_InitNum);
		}

		// convert digit count into format string for sprintf
		//char buffer[64];
		//::sprintf(buffer, "%%0%dd", digit_count);

		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss.setf(std::ios::fixed, std::ios::floatfield);
		oss << "%0"<<digit_count<<"d";
		std::string buffer(oss.str());
		o_Format = buffer.c_str();

		// Now take off prefix from front of string
		o_Prefix = itString(0, pos+1, i_Filename);
	}

}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverRenderPass::cmraDriverRenderPass(tmlnChannelRenderPass& i_Channel, int i_RenderPass)
:	m_Channel(i_Channel),
	m_FirstTextureFileName("First Texture Filename"),
	m_NumberOfFrames("Number of Frames", 0),
	m_FrameRate("Frame Rate", g3dConstants::c_fDefaultFrameRate),
	m_BlendOp("Blend Operation",0),
	m_RenderPass("Render Pass",i_RenderPass),
	m_bEnable("Enable",true),
	m_BlendIntensity("Blend Intensity",1),
	m_CurrTex("First Texture Filename")
{

	m_BlendOp.SetEnumTag(0,"Add");
	m_BlendOp.SetEnumTag(1,"Multiply");
	//m_BlendOp.SetEnumTag(2,"Replace");

	this->SetRestoreOriginalValue(false);

	m_CurrTex.SetValue(i_Channel.GetTexValue());
	m_BlendIntensity.SetValue(i_Channel.GetFloatValue().GetValue());
	m_BlendOp.SetValue(i_Channel.GetIntValue().GetValue());

	m_FirstTextureFileName = m_CurrTex;

	this->SetBlendType(tmlnDriver::e_NoBlending);

	// Since we are getting our initial value from the channel, we need
	// to actually do the call to load the texture name for this initial value.
	// After this, changes to the property will load the next textures.
	set_textures(m_FirstTextureFileName.GetValue(), m_NumberOfFrames.GetValue());

	prtyPropertyUIInfo* pPUII;
	prtyTextureFileChooserUIInfo* pPFCUII;

	pPUII = new prtyCheckBoxUIInfo(&(m_bEnable), "Animated Render Pass", "Enable");
	AddProperty( pPUII );

	pPFCUII = new prtyTextureFileChooserUIInfo(&(m_FirstTextureFileName), "Animated Render Pass", "First filename to load and then modify to get other file names");
	AddProperty( pPFCUII );

	prtyComboBoxUIInfo* pCBUII = new prtyComboBoxUIInfo(&(m_BlendOp), "Animated Render Pass", "Buffer Type");

	switch ( m_RenderPass.GetValue() )
	{
		case cmraRenderPasses::e_AO:
			m_BlendOp.SetValue( g3dPassBuffers::e_MUL );
			pCBUII->SetReadOnly(true);
			break;

		case cmraRenderPasses::e_GI:
			m_BlendOp.SetValue( g3dPassBuffers::e_ADD );
			pCBUII->SetReadOnly(true);
			break;

		case cmraRenderPasses::e_Reflections:
			m_BlendOp.SetValue( g3dPassBuffers::e_ADD );
			pCBUII->SetReadOnly(true);
			break;

		case cmraRenderPasses::e_ShadowMask:
			m_BlendOp.SetValue( g3dPassBuffers::e_MUL );
			pCBUII->SetReadOnly(true);
			break;

		case cmraRenderPasses::e_Beauty:
			m_BlendOp.SetValue( g3dPassBuffers::e_ADD );
			pCBUII->SetReadOnly(false);
			break;
	}

	SetBufferData(m_FirstTextureFileName);

	AddProperty( pCBUII );

	prtyRangedFloatUIInfo* pRFUII = new prtyRangedFloatUIInfo(&(m_BlendIntensity), "Animated Render Pass", "Blend Intensity");
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(1);
	AddProperty(pRFUII);

	pPUII = new prtyFloatEditUIInfo(&(m_NumberOfFrames), "Animated Render Pass", "Number of files to play in animation");
	AddProperty( pPUII );

	//	property callbacks
	m_FirstTextureFileName.AddCallback(new prtyCallbackWrapper<cmraDriverRenderPass>(this, &cmraDriverRenderPass::TextureFileNameChanged));
	m_NumberOfFrames.AddCallback(new prtyCallbackWrapper<cmraDriverRenderPass>(this, &cmraDriverRenderPass::TextureFileNameChanged));
	m_BlendOp.AddCallback(new prtyCallbackWrapper<cmraDriverRenderPass>(this, &cmraDriverRenderPass::BlendOpChanged));
	m_BlendIntensity.AddCallback(new prtyCallbackWrapper<cmraDriverRenderPass>(this, &cmraDriverRenderPass::BlendIntensityChanged));
	m_bEnable.AddCallback(new prtyCallbackWrapper<cmraDriverRenderPass>(this, &cmraDriverRenderPass::EnableChanged));
}

//--------------------------------------------------------------------
//  ~cmraDriverRenderPass
//--------------------------------------------------------------------
cmraDriverRenderPass::~cmraDriverRenderPass()
{
	SetFileReflectionMode(false);
}

//--------------------------------------------------------------------
//  SetFileReflectionMode()
//--------------------------------------------------------------------
void cmraDriverRenderPass::SetFileReflectionMode(bool i_Val)
{
	if ( m_RenderPass.GetValue() == cmraRenderPasses::e_Reflections )
	{
		g3dPassBuffers::SetDoingFileRefl(i_Val);
	}
}

//--------------------------------------------------------------------
//  SetBufferData()
//--------------------------------------------------------------------
void cmraDriverRenderPass::SetBufferData(prtyTextureFileName i_Tex)
{
	m_Channel.SetTexValue( i_Tex.GetValue() );
	m_Channel.SetFloatValue( m_BlendIntensity );
	m_Channel.SetIntValue( m_BlendOp.GetValue() );
}

//--------------------------------------------------------------------
// Compute frame of animation to use based on frame rate, 
//	start/end frame, etc.
//--------------------------------------------------------------------
int cmraDriverRenderPass::compute_frame(const maTime& i_Time)
{
	int num_frames = m_NumberOfFrames.GetValue();

	// should this round off, not clamp down?
	//float beginTime = this->GetBeginTime();
	//float test = this->GetBeginTime() * this->m_FrameRate.GetValue();
	//int frame = (int)((i_Time - this->GetBeginTime()) * this->m_FrameRate.GetValue());
	int frame = (int)(i_Time - this->GetBeginTime()).AsFrame( (int)this->m_FrameRate.GetValue() );
	if (frame <= 0)
	{
		frame = 0;
	}
	else if (frame >= num_frames)
	{
		frame = -1;
	}
	return frame;
}

//--------------------------------------------------------------------
// GetCaptRenderPassBool()
//--------------------------------------------------------------------
bool cmraDriverRenderPass::GetCaptRenderPassBool()
{
	captPassBufferFlags flags = g3dPassBuffers::GetCaptPassBufferFlags();
	switch ( m_RenderPass.GetValue() )
	{
		case cmraRenderPasses::e_AO:
			return flags.bPassBufferAO;

		case cmraRenderPasses::e_GI:
			return flags.bPassBufferGI;

		case cmraRenderPasses::e_Reflections:
			return flags.bPassBufferRefl;

		case cmraRenderPasses::e_ShadowMask:
			return flags.bPassBufferShadowMask;

		case cmraRenderPasses::e_Beauty:
			return flags.bPassBufferBeauty;
	}
	return false;
}

//--------------------------------------------------------------------
// Update() - The body of Operate()
//--------------------------------------------------------------------
void cmraDriverRenderPass::Update()
{
	// If the blend settings say no blend, and time is less than our begin time,
	// leave the old texture in place.
	if (IsBefore(m_CurrTime) && this->GetBlendType() == tmlnDriver::e_NoBlending)
	{
		m_CurrTex = fsLocator();
		SetBufferData(m_CurrTex);
		SetFileReflectionMode(false);
		return;
	}

	//bool enable;
	//if ( g3dPassBuffers::GetCaptPassBufferFlags().bPassBufferCapturing )
	//{
	//	enable = GetCaptRenderPassBool();
	//}
	//else
	//{
	//	enable = m_bEnable.GetValue();
	//}

	if (m_Textures.size() && m_bEnable.GetValue() )
	{	
		int frame = compute_frame(m_CurrTime);

		if ( frame == -1 )
		{
			m_CurrTex = fsLocator();
			SetFileReflectionMode(false);
		}
		else
		{
			m_CurrTex = m_Textures[frame];
			SetFileReflectionMode(true);
		}
	}
	else
	{
		// empty means clear the texture (this is a possible animation state)
		m_CurrTex = fsLocator();
		SetFileReflectionMode(false);
	}
	SetBufferData(m_CurrTex);

}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void cmraDriverRenderPass::Operate(const maTime& i_Time)
{
	m_CurrTime = i_Time;
	Update();
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverRenderPass::GetHoverDescription()
{
	return tmlnDriver::GetHoverDescription();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverRenderPass::GetDriverInfo() const
{
	cmraDriverRenderPassInfo *pInfo = new cmraDriverRenderPassInfo(cmraDriverRenderPassParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_FirstTextureFileName = this->m_FirstTextureFileName.GetValue();
	pInfo->m_bEnable = this->m_bEnable.GetValue();
	pInfo->m_NumberOfFrames = this->m_NumberOfFrames.GetValue();
	pInfo->m_BlendOp = this->m_BlendOp.GetValue();
	pInfo->m_BlendIntensity = this->m_BlendIntensity.GetValue();
	pInfo->m_RenderPass = this->m_RenderPass.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverRenderPass::SetDriverInfo( const cmraDriverRenderPassInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// this will load the script as needed
	this->m_FirstTextureFileName.SetValue(i_Info.m_FirstTextureFileName.GetFullValue());
	this->m_bEnable.SetValue(i_Info.m_bEnable);
	this->m_NumberOfFrames.SetValue(i_Info.m_NumberOfFrames);
	this->m_BlendOp.SetValue(i_Info.m_BlendOp);
	this->m_BlendIntensity.SetValue(i_Info.m_BlendIntensity);
	this->m_RenderPass.SetValue(i_Info.m_RenderPass);
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverRenderPass::GetClipFillColor() const
{
	switch ( m_RenderPass.GetValue() )
	{
		case cmraRenderPasses::e_AO:
			return maFloatRGBA( 0.8f, 0.53f, 0.6f, 1.0f );
			break;

		case cmraRenderPasses::e_GI:
			return maFloatRGBA( 0.5f, 0.73f, 0.5f, 1.0f );
			break;

		case cmraRenderPasses::e_Reflections:
			return maFloatRGBA( 0.8f, 0.43f, 0.8f, 1.0f );
			break;

		case cmraRenderPasses::e_ShadowMask:
			return maFloatRGBA( 0.4f, 0.4f, 0.4f, 1.0f );
			break;

		case cmraRenderPasses::e_Beauty:
			return maFloatRGBA( 0.4f, 0.53f, 0.7f, 1.0f );
			break;
	}
	return maFloatRGBA( 0.5f, 0.5f, 0.5f, 1.0f );
}

//--------------------------------------------------------------------
// Set Textures as list of texture names in numbered order.
// i_FirstTextureFileName defines the pattern, digits before the
// file extension are used to specify the numbering.
//--------------------------------------------------------------------
void cmraDriverRenderPass::set_textures( const fsLocator& i_FirstTextureFileName,
											   int i_NumFrames )
{
	//std::for_each(m_Textures.begin(), m_Textures.end(), matTextureMgr::ReleaseTexture);
	m_Textures.clear();

	if (i_FirstTextureFileName.GetNumNames() > 0)
	{
		// Check for simple case - treat filename as a single key, not an animation
		if (m_NumberOfFrames.GetValue() == 0)
		{
			m_Textures.push_back(i_FirstTextureFileName);
		}
		else
		{
			// Load animation - looking for numbers at the end of the 
			// filename in order to use as a counter
			itString first_filename = i_FirstTextureFileName.GetLastName();
			fsLocator dir = i_FirstTextureFileName;
			dir.Pop();

			// find number pattern from filename
			//
			int init_num = 0;
			itString prefix, extension;
			std::string format;
			find_number_pattern(first_filename, prefix, init_num, format, extension);
			//	create all the texture names
			//
			for (int i=0; i<i_NumFrames; i++)
			{
				// get filename based on numbering
				itString filename;
				if (i==0)
				{
					filename = first_filename;
				}
				else
				{
					filename = prefix;
					char buffer[64];
					::sprintf(buffer, format.c_str(), init_num+i);
					filename += itString(buffer);
					filename += extension;
				}

				fsLocator fullpath = dir;
				fullpath.Push( filename );
				m_Textures.push_back(fullpath);
			}
		}
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Resize duration of driver to match length of animation if requested
//--------------------------------------------------------------------
void cmraDriverRenderPass::update_driver_length_from_anim_length()
{
	// if this checkbox is checked, resize the driver length.
	if ( m_FrameRate.GetValue() > 0 )
	{
		//float anim_duration = (m_NumberOfFrames.GetValue() / m_FrameRate.GetValue());
		maTime anim_duration = maTime::FromFrame(m_NumberOfFrames.GetValue(), (int)m_FrameRate.GetValue());
		this->SetEndTime( this->GetBeginTime() + anim_duration );

		// update gui
		chnlDialogUtil::UpdateDriver(this);
	}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverRenderPass::TextureFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	set_textures(m_FirstTextureFileName.GetValue(), m_NumberOfFrames.GetValue());
	update_driver_length_from_anim_length();
	this->MarkDirty();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverRenderPass::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//update_driver_length_from_anim_length();
	this->MarkDirty();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverRenderPass::BlendOpChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	SetBufferData(m_CurrTex);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverRenderPass::BlendIntensityChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	SetBufferData(m_CurrTex);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverRenderPass::EnableChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	Update();
	this->MarkDirty();
}