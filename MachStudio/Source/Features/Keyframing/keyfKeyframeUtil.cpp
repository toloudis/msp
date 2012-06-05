/****************************************************************************\
**	keyfKeyframeUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Keyframing/keyfKeyframeUtil.hpp"

#include "Features/Keyframing/keyfKeyCreator.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace keyfKeyframeUtil
{
	namespace
	{
		// AutoKey - if on, check for changes every frame and make keys
		bool l_bAutoKey = false;

		// The threshold around the current time to look for drivers
		//const float c_TimeThreshold = 1.0f / (g3dConstants::c_fDefaultFrameRate + 10.0f); // Threshold less than 1 frame at g3dConstants::c_fDefaultFrameRate fps
		//TIME - changed threshold to a 1/60 second threshold
		const maTime c_TimeThreshold = maTime::FromFrame(1, 60);

		struct sObjectChannel
		{
			sObjectChannel(tmlnScriptObject* i_pObject, tmlnChannel* i_pChannel)
				: m_pObject(i_pObject), m_pChannel(i_pChannel)	{ }

			tmlnScriptObject* m_pObject;
			tmlnChannel* m_pChannel;
		};

		//--------------------------------------------------------------------
		// Get channels from the script objects that have current values
		//	than the scripted value. This channels are the candidates
		//	for new keys.
		// If i_bAllChannelsWithDrivers is true, then 
		//--------------------------------------------------------------------
		void get_candidate_channels(const std::vector<tmlnScriptObject*> &i_Objects,
									std::vector<sObjectChannel> &o_Channels,
									bool i_bAllChannelsWithDrivers)
		{
			// Create a list of channels that report differences.
			// If a channel cannot tell the difference, then it will 
			// just return false.
			maTime current_time = tmlnTimeLine::GetValue();

			
			std::vector<tmlnScriptObject*>::const_iterator it, end = i_Objects.end();
			for (it = i_Objects.begin(); it != end; ++it)
			{
				tmlnChannelSet &channel_set = (*it)->ChannelSet();
				
				const int num_channels = channel_set.GetNumChannels();
				for (int i=0; i<num_channels; ++i)
				{
					tmlnChannel &channel = channel_set.Channel(i);
							
				
					// This prevents the autokey from creating keys when the channel is locked.
					if(channel.IsLocked()) tmlnTimeLine::SetLocked(true);
					if (channel.HasValueVariation() && ( !channel.IsLocked()))
					{
						//tmlnTimeLine::SetLocked(false);
						o_Channels.push_back(sObjectChannel(*it, &channel));
					}
					else if (i_bAllChannelsWithDrivers)
					{
						if (channel.CanBeKeyed() && (channel.GetNumDrivers() > 0) && (!channel.IsLocked()))
						{
							// This channel doesn't have value variations, but
							// it still should get a new key because it has animation
							// on it somewhere else
							//tmlnTimeLine::SetLocked(false);
							std::vector<tmlnDriver*> active_drivers;
							channel.GetActiveDrivers( current_time, current_time, active_drivers);

							// Only key the channel if there is no driver active at
							// current time (if it needs a new key)
							if (active_drivers.empty())
								o_Channels.push_back(sObjectChannel(*it, &channel));
						}
					}
				}
			}
		}

		//--------------------------------------------------------------------
		// Look at each channel in list and either add a new key
		//	alter a driver at the given time.
		//--------------------------------------------------------------------
		void key_channels(const std::vector<sObjectChannel> &i_Channels,
						  std::set<std::string>& o_NewKeyedChannels,
						  std::set<std::string>& o_AlteredKeyChannels,
						  std::set<std::string>& o_ProblemChannels,
						  bool i_bDoUndo)
		{
			maTime current_time = tmlnTimeLine::GetValue();

			std::vector<sObjectChannel>::const_iterator it, end = i_Channels.end();
			for (it = i_Channels.begin(); it != end; ++it)
			{
				// for each channel,
				tmlnChannel *pChannel = it->m_pChannel;

				// see if there is a driver at the current time
				std::vector<tmlnDriver*> drivers;
				pChannel->GetActiveDrivers(current_time, current_time, drivers);
				if (drivers.empty())
				{
					// if no drivers found exactly, look for drivers within 
					// a threshold range of time
					pChannel->GetActiveDrivers(current_time - c_TimeThreshold, 
											   current_time + c_TimeThreshold, 
											   drivers);
				}

				// Were any drivers found at this time?
				if (!drivers.empty())
				{
								
					// Try to alter the value of the drivers to match
					// the current value of the channels
					std::vector<tmlnDriver*>::iterator it, end = drivers.end();
					for (it = drivers.begin(); it != end; ++it)
					{
						// Can the driver be altered?
						if ((*it)->AlterKey())
						{
							// record that the driver was altered
							//if(pChannel->IsLocked()) continue;
							o_AlteredKeyChannels.insert(pChannel->GetName());
						}
						else 
						{
							// record that the driver could not be altered
							o_ProblemChannels.insert(pChannel->GetName());
						}
					}
				}
				else
				{
					// If no drivers were found within the threshold region,
					// Create a new key for this channel.
					keyfKeyCreator::CreateKey(it->m_pObject, pChannel, i_bDoUndo);
					o_NewKeyedChannels.insert(pChannel->GetName());
				}
			}

		}
	}


	//--------------------------------------------------------------------
	// Look at the scripted context of the selected objects and
	// either create new keys where needed or alter the existing
	// drivers to match the current channel values.
	//--------------------------------------------------------------------
	void DoKeyframe()	// Need void() function pointer for simple command
	{
		// This is coming from the SetKey command, not from AutoKey...
		// Default quiet mode as false and all channels with drivers as true
		const bool bQuietMode = false;
		bool bAllChannelsWithDrivers = PrefsMgr::GetDataSimple().m_bAllChannelsWithDrivers.GetValue();

		DoKeyframe(bQuietMode, bAllChannelsWithDrivers);	
	}
	void DoKeyframe(bool i_bQuietMode, bool i_bAllChannelsWithDrivers)
	{
		// Gather list of selected timeline objects
		std::vector<tmlnScriptObject*> objects;
		tmlnSelectionUtil::GetSelectedScriptObjects(objects);

		// Check to see if there are any channels that vary from their scripts
		std::vector<sObjectChannel> channels;
		get_candidate_channels(objects, channels, i_bAllChannelsWithDrivers);
		if (!channels.empty())
		{
			// We need the full set of changes made automatically
			// to act as one undo operation
			undoUndoMgr::BeginMultipleOperationBlock("Keyframing");

			// If we have channels, add new keys or alter the existing drivers		
			std::set<std::string> new_keys, altered_keys, problem_channels;
			const bool bDoUndo = (!i_bQuietMode);
			key_channels(channels, new_keys, altered_keys, problem_channels, bDoUndo);

			undoUndoMgr::EndMultipleOperationBlock();

			// Report successful results to status bar
			std::string key_msg;
			if (!new_keys.empty())
			{
				key_msg += "Created keys for:";
				std::set<std::string>::iterator it, end = new_keys.end();
				for (it = new_keys.begin(); it != end; ++it)
				{
					key_msg += " ";
					key_msg += (*it);
				}
			}
			if (!altered_keys.empty())
			{
				key_msg += (" Altered keys for:");
				std::set<std::string>::iterator it, end = altered_keys.end();
				for (it = altered_keys.begin(); it != end; ++it)
				{
					key_msg += " ";
					key_msg += (*it);
				}
			}
			if (!key_msg.empty())
				guiStatusBarMgr::SetText(mnmConstants::e_SBPanel_Messages, key_msg.c_str());

			//
			if (!i_bQuietMode)
			{
				// If some channels could not be keyed, report them as problems
				if (!problem_channels.empty())
				{
					std::string problem_msg("Could not key these channels: ");
					std::set<std::string>::iterator it, end = problem_channels.end();
					for (it = problem_channels.begin(); it != end; ++it)
					{
						problem_msg += "\n";
						problem_msg += (*it);
					}
					guiMessageBox::Show(problem_msg.c_str(), "Problem creating keys", guiMessageBox::e_OKOnly);
				}
			}
		}
		else
		{
			// If not, create new drivers for the keys
			if (!i_bQuietMode)
			{
				// first, just report no differences found?
				guiMessageBox::Show("No channels were found with differences to key", "No keys created", guiMessageBox::e_OKOnly);
			}

			// Other ways to handle this:
			// use default values of channels?
			// display a list of properties that can be keyed?
				// grouped by category
			// (this requires a boolean for if the channel can be keyed)
		}
	}

	//--------------------------------------------------------------------
	//	Either alters a existing driver on the given channel or 
	//	creates a key driver for the given channel at the current time.
	//	Returns true if either a driver could be altered or a new
	//	driver could be successfully created.
	//--------------------------------------------------------------------
	bool AlterOrCreateKey(tmlnScriptObject *i_pObject, 
						  tmlnChannel *i_pChannel)
	{
		const bool do_undo = true;
		
		std::vector<sObjectChannel> obj_channels;
		obj_channels.push_back(sObjectChannel( i_pObject, i_pChannel ));
		
		std::set<std::string> keyed, altered, problem;
		key_channels(obj_channels, keyed, altered, problem, do_undo );

		return (problem.empty());
	}

	//--------------------------------------------------------------------
	// In auto-key mode, check every frame for changes to channels and
	// then create or alter drivers immediately.
	//--------------------------------------------------------------------
	void SetAutoKey(bool i_bVal)
	{			
		l_bAutoKey = i_bVal;
		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Key, l_bAutoKey?"Key":"" );
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Key, 
			l_bAutoKey ? "Auto Key: On" : "Auto Key: Off" );

		PrefsMgr::GetDataSimple().m_bAutoKey = i_bVal;
		
	}
	bool IsAutoKey()
	{
		return l_bAutoKey;
	}

	//--------------------------------------------------------------------
	// Call AlterOrCreateKey for the channel with the
	// given name on the selected object.
	//--------------------------------------------------------------------
	void KeyProperty(const std::string& i_PropertyName)
	{
		// Special case for materials, need to prefix material name
		std::set<std::string> full_property_names;
		full_property_names.insert( i_PropertyName );

		// multiple material selections means multiple property names possible
		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			if ( mtrlPropertyObject *pMatObj = sel3dCastUtil::CastPickObject<mtrlPropertyObject>(*sit) )
			{
				full_property_names.insert( pMatObj->GetName() + "." + i_PropertyName );
			}
		}

		// this code was single selection
		//tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetSelectedScriptObject();
		//if (pScriptObject)
		
		// Gather list of multiple selected timeline objects
		std::vector<tmlnScriptObject*> objects;
		tmlnSelectionUtil::GetSelectedScriptObjects(objects);
		bool bSuccess = true, bNotFound = false;
		for (int i=0; i<objects.size(); ++i)
		{	
			tmlnScriptObject *pScriptObject = objects[i];

			tmlnChannelSet &channel_set = pScriptObject->ChannelSet();
			int c;
			bool bFound = false;
			for (c=0; c<channel_set.GetNumChannels(); ++c)
			{
				tmlnChannel& channel = channel_set.Channel(c);
				//if (channel.GetName() == full_property_name)
				if ( full_property_names.find(channel.GetName()) != full_property_names.end() )
				{
					//TODO: Should automatically select the new driver(s)
					bSuccess &= (keyfKeyframeUtil::AlterOrCreateKey(pScriptObject, &channel));

					// In multiple material selection, have to continue on to see if other channels can be found...
					bFound = true;
					//break;
				}
			}
			//if (c == channel_set.GetNumChannels())
			if (!bFound)
				bNotFound = true;
		}
		
		if (!bSuccess)
		{
			std::string msg = "Could not create key for property " + i_PropertyName;
			guiMessageBox::Show(msg.c_str(), "Could not create key");
		}

		if (bNotFound)
		{
			std::string msg = "Could not find animation channel for property " + i_PropertyName;
			guiMessageBox::Show(msg.c_str(), "Could not create key");
		}
	}

} // end of namespace
