/*****************************************************************************
**	tmlnDriverAnimatedFilePath.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/FilePath/tmlnDriverAnimatedFilePath.hpp"

#include "Drivers/FilePath/tmlnDriverAnimatedFilePathInfo.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"

//#include "Core/fs/fsResourceTracker.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <algorithm>
#include <sstream>

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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnDriverAnimatedFilePath::tmlnDriverAnimatedFilePath(tmlnChannelFilePath& i_ChannelTexture, 
													   chDefs::Name i_ChunkName)
:	m_ChannelTexture(i_ChannelTexture), 
	m_ChunkName(i_ChunkName),
	m_FirstFilePath("First Texture Filename",i_ChannelTexture.GetValue()),
	m_NumberOfFrames("Number of Frames", 0),
	m_FrameRate("Frame Rate", g3dConstants::c_fDefaultFrameRate),
	m_bLooping("Looping", false),
	m_bReversing("Reversing", false),
	m_bDriverToAnimLength("Driver to Anim Length", true),
	m_bPrevLoop(false)
{
	this->SetBlendType(tmlnDriver::e_NoBlending);

	// Since we are getting our initial value from the channel, we need
	// to actually do the call to load the texture name for this initial value.
	// After this, changes to the property will load the next textures.
	set_textures(m_FirstFilePath.GetValue(), m_NumberOfFrames.GetValue());

	prtyFileChooserUIInfo* pPFCUII = new prtyFileChooserUIInfo(&(m_FirstFilePath), "Animation", "First filename to load and then modify to get other file names");
	// get initial directory from the channel
	//pPFCUII->SetInitialDirectory( i_ChannelTexture.GetDirectory() );
	AddProperty( pPFCUII );
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyFloatEditUIInfo(&(m_NumberOfFrames), "Animation", "Number of files to play in animation");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_FrameRate), "Animation", "Frames per second to play");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bLooping), "Animation", "Animation loops back to start");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bReversing), "Animation", "Animation reverses direction when it loops");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bDriverToAnimLength), "Animation", "Resize driver length to length of animation at given frame rate");
	AddProperty( pPUII );

	//	property callbacks
	m_FirstFilePath.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::FilePathChanged));
	m_NumberOfFrames.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::FilePathChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::PropertyChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::PropertyChanged));
	m_bReversing.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::ReversePropertyChanged));
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimatedFilePath>(this, &tmlnDriverAnimatedFilePath::DriverToAnimLengthChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnDriverAnimatedFilePath::~tmlnDriverAnimatedFilePath()
{
//	std::for_each(m_Textures.begin(), m_Textures.end(), matTextureMgr::ReleaseTexture);
}

//----------------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//----------------------------------------------------------------------------
std::string tmlnDriverAnimatedFilePath::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	if (m_FirstFilePath.GetValue().GetNumNames() > 0)
	{
		std::string str = "List : ";
		str += itStringUtil::GetStdString(m_FirstFilePath.GetValue().GetLastName());
		desc += str;
	}
	return desc;
}

//----------------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//----------------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverAnimatedFilePath::GetDriverInfo() const
{
	tmlnDriverAnimatedFilePathInfo *pInfo = new tmlnDriverAnimatedFilePathInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_FirstFilePath = this->m_FirstFilePath.GetValue();
	pInfo->m_FrameRate = this->m_FrameRate.GetValue();
	pInfo->m_NumberOfFrames = this->m_NumberOfFrames.GetValue();
	pInfo->m_bLooping = this->m_bLooping.GetValue();
	pInfo->m_bReversing = this->m_bReversing.GetValue();
	pInfo->m_bDriverToAnimLength = this->m_bDriverToAnimLength.GetValue();

	return pInfo;
}

//----------------------------------------------------------------------------
// Set internal variables from data structure
//----------------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::SetDriverInfo(	const tmlnDriverAnimatedFilePathInfo& i_Info )
{
	// Turn off the driver length flag in order to prevent 
	// multiple updates to the driver length as the anim
	// info is set.
	this->m_bDriverToAnimLength = false;

	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// this will load the script as needed
	this->m_FirstFilePath.SetValue(i_Info.m_FirstFilePath);
	this->m_NumberOfFrames.SetValue(i_Info.m_NumberOfFrames);

	// Store local info
	this->m_FrameRate.SetValue(i_Info.m_FrameRate);
	this->m_bLooping.SetValue(i_Info.m_bLooping);
	this->m_bReversing.SetValue(i_Info.m_bReversing);

	// Set this data last so that the begin and end time are correct.
	// the earlier sets will alter end time because of the 
	// m_bDriverToAnimLength flag. 
	this->m_bDriverToAnimLength.SetValue(i_Info.m_bDriverToAnimLength);

	this->MarkDirty();
}


//----------------------------------------------------------------------------
//  Update position of things that are being driven
//----------------------------------------------------------------------------
void  tmlnDriverAnimatedFilePath::Operate(const maTime& i_Time)
{
	// If the blend settings say no blend, and time is less than our begin time,
	// leave the old texture in place.
	if (IsBefore(i_Time) && this->GetBlendType() == tmlnDriver::e_NoBlending)
	{
		return;
	}

	if (m_Textures.size())
	{	
		int frame = compute_frame(i_Time);
		m_ChannelTexture.SetValue(m_Textures[frame]);
	}
	else
	{
		// empty means clear the texture (this is a possible animation state)
		m_ChannelTexture.SetValue(fsLocator());
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverAnimatedFilePath::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.6392f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float tmlnDriverAnimatedFilePath::GetAnimationNumFrames() const
{
	return (float)m_NumberOfFrames.GetValue();
}

//--------------------------------------------------------------------
// Returns how long the animation is in seconds given the 
// frame rate and other data about this animation.
//--------------------------------------------------------------------
float tmlnDriverAnimatedFilePath::GetAnimationLength() const
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
void tmlnDriverAnimatedFilePath::GetResourceList( fsResourceTrackerData& io_List )
{
	if (m_FirstFilePath.GetValue().GetNumNames() > 0)
	{
		itString first_filename = m_FirstFilePath.GetValue().GetLastName();
		fsLocator dir_loc(m_FirstFilePath.GetValue());
		dir_loc.Pop();

		if (this->m_NumberOfFrames.GetValue() == 0)
		{
			//io_List.Add( first_filename );
			io_List.Add( m_FirstFilePath.GetValue() );
		}
		else
		{
			// find number pattern from filename
			//
			int init_num = 0;
			itString prefix, extension;
			std::string format;
			find_number_pattern(first_filename, prefix, init_num, format, extension);

			//	loop all the textures
			//
			for (int i=0; i < this->m_NumberOfFrames.GetValue(); i++)
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
			
				// construct fullpath instead of just single filename
				//io_List.Add( filename );
				fsLocator tex_loc(dir_loc);
				tex_loc.Push( filename );
				io_List.Add( tex_loc );
			}
		}
	}
}

//--------------------------------------------------------------------
// Set Textures as list of texture names in numbered order.
// i_FirstFilePath defines the pattern, digits before the
// file extension are used to specify the numbering.
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::SetTextures( const fsLocator& i_FirstFilePath,
				   int i_NumFrames )
{
	m_FirstFilePath.SetValue(i_FirstFilePath);
	m_NumberOfFrames.SetValue(i_NumFrames);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::SetFrameRate( float i_Val )
{
	m_FrameRate.SetValue(i_Val);

	this->MarkDirty();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::SetLooping( bool i_bSetFlag )
{
	m_bPrevLoop = m_bLooping.GetValue();
	m_bLooping.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::SetDriverToAnimLengthFlag( bool i_bSetFlag )
{
	m_bDriverToAnimLength.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Compute frame of animation to use based on frame rate, 
//	start/end frame, etc.
//--------------------------------------------------------------------
int tmlnDriverAnimatedFilePath::compute_frame(const maTime& i_Time)
{
	int num_frames = m_NumberOfFrames.GetValue();
	if (num_frames == 0)
		return 0;

	// should this round off, not clamp down?
	//int frame = (int)((i_Time - this->GetBeginTime()) * this->m_FrameRate.GetValue());
	int frame = (i_Time - this->GetBeginTime()).AsFrame((int)this->m_FrameRate.GetValue());
	if (frame <= 0)
	{
		frame = 0;
	}
	else if (frame >= num_frames)
	{
		if ( this->m_bReversing.GetValue() )
		{
			this->m_bLooping.SetValue(true);
			int rev_total_length = (2 * num_frames) - 2; // (1-2-3-2)-(1-2-3-2) is reversing loop
			frame = frame % rev_total_length;
			if (frame >= num_frames)
				frame = rev_total_length - frame;
		}
		else if ( this->m_bLooping.GetValue() )
		{
			frame = frame % num_frames;
		}
		else
		{
			frame = num_frames - 1;
		}
	}
	return frame;
}

//--------------------------------------------------------------------
// Set Textures as list of texture names in numbered order.
// i_FirstFilePath defines the pattern, digits before the
// file extension are used to specify the numbering.
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::set_textures( const fsLocator& i_FirstFilePath,
											   int i_NumFrames )
{

	// now the textures themselves
	//
	//std::for_each(m_Textures.begin(), m_Textures.end(), matTextureMgr::ReleaseTexture);
	m_Textures.clear();

	if (i_FirstFilePath.GetNumNames() > 0)
	{
		// Check for simple case - treat filename as a single key, not an animation
		if (m_NumberOfFrames.GetValue() == 0)
		{
			m_Textures.push_back(i_FirstFilePath);
		}
		else
		{
			// Load animation - looking for numbers at the end of the 
			// filename in order to use as a counter
			itString first_filename = i_FirstFilePath.GetLastName();
			fsLocator dir = i_FirstFilePath;
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

	//DBG_LOG("AnimatedTexture -- end");
	//fsResourceTracker::Debug_OutputList();
}

//--------------------------------------------------------------------
// Resize duration of driver to match length of animation if requested
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::update_driver_length_from_anim_length()
{
	// if this checkbox is checked, resize the driver length.
	//
	if (   ( m_bDriverToAnimLength.GetValue() )
		&& ( m_FrameRate.GetValue() > 0 ) )
	{
		maTime anim_duration = maTime::FromFrame(m_NumberOfFrames.GetValue(), (int)m_FrameRate.GetValue());
		this->SetEndTime( this->GetBeginTime() + anim_duration );

		// update gui
		//chnlDialogUtil::UpdateChannels();
		chnlDialogUtil::UpdateDriver(this);

		//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
	}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::FilePathChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	set_textures(m_FirstFilePath.GetValue(), m_NumberOfFrames.GetValue());
	update_driver_length_from_anim_length();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	update_driver_length_from_anim_length();
	this->MarkDirty();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimatedFilePath::DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
}

void tmlnDriverAnimatedFilePath::ReversePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if(m_bReversing.GetValue()) 
		SetLooping(true);
	else
		SetLooping(m_bPrevLoop);
	update_driver_length_from_anim_length();
	this->MarkDirty();
}
