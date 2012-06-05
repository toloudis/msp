/*****************************************************************************
**	sbrdDriverAnimatedTexture.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTexture.hpp"

#include "Systems/Storyboards/Timeline/sbrdChannelTexture.hpp"
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureForm.h"						// GUI
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureInfo.hpp"
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureParser.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Graphics/G3d/g3dConstants.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Graphics/Mat/matExceptionX.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"

#include <algorithm>


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
			{
				numpart[index++] = i_Filename[ccnt];
			}
			sscanf(numpart,"%d", &o_InitNum);
		}

		// convert digit count into format string for sprintf
		char buffer[64];
		::sprintf(buffer, "%%0%dd", digit_count);
		o_Format = buffer;

		// Now take off prefix from front of string
		o_Prefix = itString(0, pos+1, i_Filename);
	}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdDriverAnimatedTexture::sbrdDriverAnimatedTexture(sbrdChannelTexture& i_ChannelTexture)
:	m_ChannelTexture(i_ChannelTexture),
	m_NumberOfFrames("Number of Frames", 0),
	m_FrameRate("Frame Rate", g3dConstants::c_fDefaultFrameRate),
	m_bLooping("Looping", false),
	m_bDriverToAnimLength("Driver to Anim Length", false),
	m_FirstTextureFilename("First Texture Filename")
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyPropertyUIInfo(&(m_NumberOfFrames), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_FrameRate), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_bLooping), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_bDriverToAnimLength), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_FirstTextureFilename), "category", "description");
	AddProperty( pPUII );

	//	property callbacks
	m_NumberOfFrames.AddCallback(new prtyCallbackWrapper<sbrdDriverAnimatedTexture>(this, &sbrdDriverAnimatedTexture::PropertyChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<sbrdDriverAnimatedTexture>(this, &sbrdDriverAnimatedTexture::PropertyChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<sbrdDriverAnimatedTexture>(this, &sbrdDriverAnimatedTexture::PropertyChanged));
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<sbrdDriverAnimatedTexture>(this, &sbrdDriverAnimatedTexture::PropertyChanged));
	m_FirstTextureFilename.AddCallback(new prtyCallbackWrapper<sbrdDriverAnimatedTexture>(this, &sbrdDriverAnimatedTexture::PropertyChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdDriverAnimatedTexture::~sbrdDriverAnimatedTexture()
{
}

//----------------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//----------------------------------------------------------------------------
std::string sbrdDriverAnimatedTexture::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	std::string str = "Animated Texture : ";
	str += itStringUtil::GetStdString(m_FirstTextureFilename.GetValue());
	desc += str;
	return desc;
}

//----------------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//----------------------------------------------------------------------------
tmlnDriverInfo*  sbrdDriverAnimatedTexture::GetDriverInfo() const
{
	sbrdDriverAnimatedTextureInfo *pInfo = new sbrdDriverAnimatedTextureInfo(sbrdDriverAnimatedTextureParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_FirstTextureFilename = this->m_FirstTextureFilename.GetValue();
	pInfo->m_FrameRate = this->m_FrameRate.GetValue();
	pInfo->m_NumberOfFrames = this->m_NumberOfFrames.GetValue();
	pInfo->m_bLooping = this->m_bLooping.GetValue();
	pInfo->m_bDriverResizeByAnimLength = this->m_bDriverToAnimLength.GetValue();

	return pInfo;
}

//----------------------------------------------------------------------------
// Set internal variables from data structure
//----------------------------------------------------------------------------
void sbrdDriverAnimatedTexture::SetDriverInfo(	const sbrdDriverAnimatedTextureInfo& i_Info, 
												prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// this will load the script as needed
	this->SetTextures(i_Info.m_FirstTextureFilename, i_Info.m_NumberOfFrames);

	// Store local info
	this->m_FrameRate.SetValue(i_Info.m_FrameRate, i_Undoable);
	this->m_bLooping.SetValue(i_Info.m_bLooping, i_Undoable);
	this->m_bDriverToAnimLength.SetValue(i_Info.m_bDriverResizeByAnimLength, i_Undoable);

	this->MarkDirty();
}


//----------------------------------------------------------------------------
//  Update position of things that are being driven
//----------------------------------------------------------------------------
void  sbrdDriverAnimatedTexture::Operate(float i_Time)
{
	// Execute camera script
	if (m_Textures.size() > 0)
	{
		// If the blend settings say no blend, and time is less than our begin time,
		// leave the old texture in place.
		if (IsBefore(i_Time) && this->GetBlendType() == tmlnDriver::e_NoBlending)
		{
			return;
		}

		tmlnDriver* pNextDriver = m_ChannelTexture.GetNextDriver(i_Time);

		std::string str = itStringUtil::GetStdString(m_FirstTextureFilename.GetValue());
		if (pNextDriver != 0)
		{
			DBG_LOG3("          operate [%s]  next begin=%6.3f   curr time=%6.3f", str.c_str(), pNextDriver->GetBeginTime(), i_Time);
		}

		if ((pNextDriver == 0) || (pNextDriver->IsBefore(i_Time))) //GetBeginTime() > i_Time))
		{
			if (pNextDriver != 0)
			{
				DBG_LOG3("   Setting Texture [%s]  next begin=%6.3f   curr time=%6.3f", str.c_str(), pNextDriver->GetBeginTime(), i_Time);
			}

			int frame = 0; //compute_frame(i_Time);
			m_ChannelTexture.SetTexture(m_Textures[frame]);
		}
	}
}

//----------------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//----------------------------------------------------------------------------
void  sbrdDriverAnimatedTexture::DoEditProperties()
{
#ifdef _MANAGED
	SystemStoryboards::sbrdDriverAnimatedTextureForm ^form =
		gcnew SystemStoryboards::sbrdDriverAnimatedTextureForm(*this);
//	form->Show();
	tmaDialogTabbedMgr::RemoveTabPages("Driver");
	tmaDialogTabbedMgr::AddTabPage("Driver", form->GetTabPage(0));

	tmlnDriver::AddBasePropertiesTab();

	tmaDialogTabbedMgr::Show("Driver");

	// The tab page from our custom form is now in the general
	// driver properties dialog, we can delete the custom form.
	delete form;
#endif
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA sbrdDriverAnimatedTexture::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.6392f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float sbrdDriverAnimatedTexture::GetAnimationNumFrames() const
{
	return (float)m_NumberOfFrames.GetValue();
}

//--------------------------------------------------------------------
// Returns how long the animation is in seconds given the 
// frame rate and other data about this animation.
//--------------------------------------------------------------------
float sbrdDriverAnimatedTexture::GetAnimationLength() const
{
	if (m_FrameRate.GetValue() == 0)
		return 0;
	else
		return (m_NumberOfFrames.GetValue() / m_FrameRate.GetValue());
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void sbrdDriverAnimatedTexture::GetResourceList( fsResourceTrackerData& io_List )
{
	// find number pattern from filename
	//
	int init_num = 0;
	itString prefix, extension;
	std::string format;
	find_number_pattern(m_FirstTextureFilename.GetValue(), prefix, init_num, format, extension);

	//	loop all the textures
	//
	for (int i=0; i < this->m_NumberOfFrames.GetValue(); i++)
	{
		fsLocator tex_loc;
		// get filename based on numbering
		itString filename;
		if (i==0)
		{
			filename = m_FirstTextureFilename.GetValue();
		}
		else
		{
			filename = prefix;
			char buffer[64];
			::sprintf(buffer, format.c_str(), init_num+i);
			filename += itString(buffer);
			filename += extension;
		}
	
		io_List.Add( filename );
	}
}

//--------------------------------------------------------------------
// Set Textures as list of texture names in numbered order.
// i_FirstTextureFilename defines the pattern, digits before the
// file extension are used to specify the numbering.
//--------------------------------------------------------------------
void sbrdDriverAnimatedTexture::SetTextures( const itString& i_FirstTextureFilename,
											int i_NumFrames )
{
	if ( (m_FirstTextureFilename.GetValue() != i_FirstTextureFilename) ||
		 (m_NumberOfFrames.GetValue() != i_NumFrames) )
	{
		// find number pattern from filename
		//
		int init_num = 0;
		itString prefix, extension;
		std::string format;
		find_number_pattern(m_FirstTextureFilename.GetValue(), prefix, init_num, format, extension);

		// Clear out old textures
		//
		// first from the resource tracker
		for (int i=0; i< m_NumberOfFrames.GetValue(); i++)
		{
			fsLocator tex_loc;

			itString filename;

			// get filename based on numbering
			if (i==0)
			{
				filename = m_FirstTextureFilename.GetValue();
			}
			else
			{
				filename = prefix;
				char buffer[64];
				::sprintf(buffer, format.c_str(), init_num+i);
				filename += itString(buffer);
				filename += extension;
			}
			//DBG_LOG3("removing from ResTracker (%s) %d of %d", itStringUtil::GetStdString(filename).c_str(), i, m_NumberOfFrames.GetValue());

			fsResourceTracker::Remove(filename);

			// FIX - the new loaded textures below will get added to the bottom of the Resource Tracker, not
			//	to their correct spot in the hierarchy.
		}

		// now the textures themselves
		//
		std::for_each(m_Textures.begin(), m_Textures.end(), matTextureMgr::ReleaseTexture);
		m_Textures.clear();

		//	set values
		//
		m_FirstTextureFilename.SetValue(i_FirstTextureFilename);
		m_NumberOfFrames.SetValue(i_NumFrames);
		find_number_pattern(m_FirstTextureFilename.GetValue(), prefix, init_num, format, extension);

		//	create all the textures
		//
		for (int i=0; i<i_NumFrames; i++)
		{
			fsLocator tex_loc;
			// get filename based on numbering
			itString filename;
			if (i==0)
			{
				filename = m_FirstTextureFilename.GetValue();
			}
			else
			{
				filename = prefix;
				char buffer[64];
				::sprintf(buffer, format.c_str(), init_num+i);
				filename += itString(buffer);
				filename += extension;
			}

			DBG_LOG4("loading texture (%s) %d of %d (%s)", itStringUtil::GetStdString(filename).c_str(), i+1, i_NumFrames, itStringUtil::GetStdString(m_FirstTextureFilename.GetValue()).c_str());

			if (sbrdGeomList::FindFile(filename, tex_loc))
			{
				fsResourceTracker::MarkBegin(tex_loc);

				try
				{
					const bool c_bMIPMAP = false;
					matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc, c_bMIPMAP);
					m_Textures.push_back(pTexture);
				}
				catch (const matInvalidTextureSizeX& i_Ex)
				{
					std::string dirstr;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), dirstr);
					DBG_ERROR1("Invalid Texture Size (%s)", dirstr.c_str());
				}

				fsResourceTracker::MarkEnd(tex_loc);
			}
			else
			{
				DBG_ERROR1("!!!!Cannot find file (%s)", itStringUtil::GetStdString(filename).c_str());

				// should this be a warning?
				m_Textures.push_back(NULL);
			}
		}

		this->MarkDirty();

		//DBG_LOG0("AnimatedTexture -- end");
		//fsResourceTracker::Debug_OutputList();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdDriverAnimatedTexture::SetFrameRate( float i_Val )
{
	m_FrameRate.SetValue(i_Val);

	this->MarkDirty();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdDriverAnimatedTexture::SetLooping( bool i_bSetFlag )
{
	m_bLooping.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdDriverAnimatedTexture::SetDriverToAnimLengthFlag( bool i_bSetFlag )
{
	m_bDriverToAnimLength.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Compute frame of animation to use based on frame rate, 
//	start/end frame, etc.
//--------------------------------------------------------------------
int sbrdDriverAnimatedTexture::compute_frame(float i_Time)
{
	// should this round off, not clamp down?
	int frame = (int)((i_Time - this->GetBeginTime()) * this->m_FrameRate.GetValue());
	if (frame <= 0)
	{
		frame = 0;
	}
	else if (frame >= m_NumberOfFrames.GetValue())
	{
		if ( this->m_bLooping.GetValue() )
		{
			frame = frame % m_NumberOfFrames.GetValue();
		}
		else
		{
			frame = m_NumberOfFrames.GetValue() - 1;
		}
	}
	return frame;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdDriverAnimatedTexture::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
#ifdef _MANAGED
	if (SystemStoryboards::sbrdDriverAnimatedTextureForm::FormInstance != nullptr)
	{
		SystemStoryboards::sbrdDriverAnimatedTextureForm::FormInstance->UpdateForm();
	}
#endif

	this->MarkDirty();
}
