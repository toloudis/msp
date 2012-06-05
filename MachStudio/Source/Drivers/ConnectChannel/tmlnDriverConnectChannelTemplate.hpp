/*****************************************************************************
**	tmlnDriverConnectChannelTemplate.hpp
**
**		ConnectChannel driver template class
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP
#error tmlnDriverConnectChannelTemplate.hpp multiply included
#endif
#define TMLN_DRIVERCONNECTCHANNELTEMPLATE_HPP

#ifndef TMLN_CHANNELSET_HPP
#include "Support/tmln/tmlnChannelSet.hpp"
#endif
#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef	TMLN_DRIVERCONNECTCHANNELINFO_HPP
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelInfo.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif
#ifndef TMLN_TIMELINEMGR_HPP
#include "Support/tmln/tmlnTimelineMgr.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef PRTY_LISTBOXUIINFO_HPP
#include "Core/prty/prtyListBoxUIInfo.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef ENV_AUDITOR_HPP
#include "Core/Env/envAuditor.hpp"
#endif 

#include <sstream>
//============================================================================
//============================================================================
class tmlnChannel;
class tmlnDriverConnectChannelInfo;


//============================================================================
//============================================================================
template<class xxxChannel>
class tmlnDriverConnectChannelTemplate : public tmlnDriver,
										 public envAuditor
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tmlnDriverConnectChannelTemplate(	xxxChannel &i_Channel, 
											chDefs::Name i_ChunkName)
		:	m_ChunkName(i_ChunkName),
			m_ChannelName(""),
			m_Channel(i_Channel), 
			m_pMasterChannel(0)
		{
			this->SetBlendType(tmlnDriver::GetDefaultBlendType());

			m_ChannelName = i_Channel.GetName();

			// Register the properties so they can be displayed to the user
			//
			prtyListBoxUIInfo* pLBUII;
			pLBUII = new prtyListBoxUIInfo(&(m_MasterObjectName), "Connection", "Name of the Master Object");
			AddProperty( pLBUII );

			// Register callbacks to update dirty bit when properties change
			m_MasterObjectName.AddCallback(new prtyCallbackWrapper<tmlnDriverConnectChannelTemplate>(this, &tmlnDriverConnectChannelTemplate::MasterObjectNameChanged));
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tmlnDriverConnectChannelTemplate()
		{
			// if we are deleted first, remove auditor from the channel
			if (m_pMasterChannel)
			{
				m_pMasterChannel->RemoveAuditor(this);
			}
		}

		//--------------------------------------------------------------------
		// Return string for description of driver (include its current state)
		// to display in the gui when the mouse hovers over the clip.
		//--------------------------------------------------------------------
		std::string GetHoverDescription()
		{
			std::string desc;
			desc = tmlnDriver::GetHoverDescription();

			//	if there is a UID for this name, then find it and verify the name
			//
			std::string name_string("none");
			if (m_MasterObjectName.GetUID() != nameString::e_InvalidUID)
			{
				nameMgr::GetNameString( m_MasterObjectName.GetUID(), name_string );
			}

			//char buffer[512];
			//::sprintf(buffer, "Connect Object: %s", name_string.c_str());

			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss << "Connect Object: "<<name_string;
			std::string buffer(oss.str());

			desc += buffer;
			return desc;
		}

		//--------------------------------------------------------------------
		//  GetDriverInfo - return data structure representing state of
		//		this driver suitable for writing to a file.
		//	The returned value should be created with "new" and will
		//		be deleted by the caller.
		//--------------------------------------------------------------------
		tmlnDriverInfo*  GetDriverInfo() const
		{
			tmlnDriverConnectChannelInfo *pInfo = new tmlnDriverConnectChannelInfo(m_ChunkName);

			this->GetBaseDriverInfo(*pInfo);

			pInfo->m_Value = this->m_MasterObjectName.GetValue();

			return pInfo;
		}

		//--------------------------------------------------------------------
		// Set internal variables from data structure
		//--------------------------------------------------------------------
		void SetDriverInfo(	const tmlnDriverConnectChannelInfo& i_Info )
		{
			// Set base info
			this->SetBaseDriverInfo(i_Info);

			// Store local info
			this->m_MasterObjectName.SetValue(i_Info.m_Value);
		}

		//--------------------------------------------------------------------
		//  Show dialog that allows user to edit this driver's properties
		//--------------------------------------------------------------------
		void  DoEditProperties()
		{
			//
			// show a list of objects to attach to
			//

			//	first, clear out the lists
			//
			prtyPropertyUIInfoContainer& UIIContainer = this->GetListContainer();
			PropertyUIIList& PUIIList = UIIContainer.GetPropertyUIInfoList();
			PropertyUIIList::iterator begin	= PUIIList.begin();
			PropertyUIIList::iterator it	= begin;
			while (it != PUIIList.end())
			{
				prtyPropertyUIInfo* pPUII = it->get();
				prtyListBoxUIInfo* pLBUII = dynamic_cast<prtyListBoxUIInfo*>(pPUII);
				if (pLBUII != 0)
				{
					pLBUII->m_List.clear();
				}
				++it;
			}

			//	second, add the objects
			//
			std::vector<tmlnScriptObject*> objects;
			tmlnTimelineMgr::GetObjects( objects );
			for (int i=0; i<objects.size(); i++)
			{
				if (objects[i] != 0)
				{
					bool bValidObject = false;
					const tmlnChannelSet& chnl_set = objects[i]->GetChannelSet();
					for (int j = 0; j < chnl_set.GetNumChannels(); ++j)
					{
						//DBG_LOG3("%02d. %s vs %s", j, chnl_set.GetChannel(j).GetName().c_str(), m_ChannelName.c_str() );

						if (strcmp(chnl_set.GetChannel(j).GetName().c_str(), m_ChannelName.c_str()) == 0)
						{
							bValidObject = true;
						}
					}

					if (bValidObject)
					{
						it = begin;
						while (it != PUIIList.end())
						{
							prtyPropertyUIInfo* pPUII = it->get();
							prtyListBoxUIInfo* pLBUII = dynamic_cast<prtyListBoxUIInfo*>(pPUII);
							if (pLBUII != 0)
							{
								pLBUII->m_List.push_back(objects[i]->GetDisplayName());
							}
							++it;
						}
					}
				}
			}

			// Let base class set up the properties-based dialog
			tmlnDriver::DoEditProperties();
		}

		//--------------------------------------------------------------------
		//	return the fill  to be used for this driver type
		//--------------------------------------------------------------------
		//virtual
		maFloatRGBA GetClipFill() const
		{
			return maFloatRGBA( 0.95f, 0.95f, 0.95f, 1.0f );
		}

private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetMasterChannel( tmlnChannel* i_pMasterChannel )
		{
			// If we were monitoring a differen channel, remove the auditor
			// from that channel
			if (m_pMasterChannel)
				m_pMasterChannel->RemoveAuditor(this);

			// Cast to correct type
			m_pMasterChannel = 
				(i_pMasterChannel) ? dynamic_cast<xxxChannel*>(i_pMasterChannel) : NULL;

			// Add auditor to this new channel. The auditor
			// will tell us when the channel is deleted
			if (m_pMasterChannel)
				m_pMasterChannel->AddAuditor(this);
		}

		//------------------------------------------------------------------------
		// This function is called when the master channel is being deleted.
		// RemoveAuditor() does not need to be called in this function, 
		// the object is being deleted anyway.
		//------------------------------------------------------------------------
		void AuditorNotify(envAuditable* i_pSubject)
		{
			m_pMasterChannel = NULL;
			this->MarkDirty();
		}

		//--------------------------------------------------------------------
		//	Override base class to have this driver's Operate called at end.
		//--------------------------------------------------------------------
		virtual bool NeedsDelayedOperate()
		{
			return true;
		}

protected:
		//--------------------------------------------------------------------
		//	search through the list of objects to find the match for this
		//	property
		//--------------------------------------------------------------------
		void FindAndSetMasterChannel()
		{
			// first, get the name from the nameMgr in case the name changed.
			//
			std::string mo_name_string;
			nameMgr::GetNameString( m_MasterObjectName.GetUID(), mo_name_string );
			//DBG_LOG5("(-%s-%d) == -%s- [%s-%d]", prty_name.GetString().c_str(), prty_name.GetUID(), mo_name_string.c_str(), m_MasterObjectName.GetString().c_str(), m_MasterObjectName.GetUID() );

			//	if the names are different, then set the names to be correct.
			//
			if ((m_MasterObjectName.GetString() != mo_name_string) && (mo_name_string.size() > 0))
			{
				m_MasterObjectName.SetString( mo_name_string );
			}

			//DBG_LOG1("Property Changed (%s)", pPrtyName->GetValue().GetString().c_str());
			std::vector<tmlnScriptObject*> objects;
			tmlnTimelineMgr::GetObjects( objects );
			for (int i=0; i<objects.size(); i++)
			{
				//DBG_LOG3("  obj %02d. %s == %s", i, objects[i]->GetDisplayName().c_str(), i_pProperty->GetValue().GetString().c_str());

				if (   (objects[i] != NULL)
					&& (strcmp(objects[i]->GetDisplayName().c_str(), mo_name_string.c_str()) == 0))
				{
					//DBG_LOG2("  obj %02d. %s", i, objects[i]->GetDisplayName().c_str());
					tmlnChannelSet& chnl_set = objects[i]->ChannelSet();
					for (int j = 0; j < chnl_set.GetNumChannels(); ++j)
					{
						//DBG_LOG3("     chnl %02d. %s vs %s", j, chnl_set.GetChannel(j).GetName().c_str(), m_ChannelName.c_str() );
						if (strcmp(chnl_set.GetChannel(j).GetName().c_str(), m_ChannelName.c_str()) == 0)
						{
							//DBG_LOG0("         ---setting channel");
							SetMasterChannel(&(chnl_set.Channel(j)));
							return;
						}
					}
				}
			}
		};

		//--------------------------------------------------------------------
		//	check to make sure master name and channel are valid
		//	if not, try to correct it.
		//--------------------------------------------------------------------
		void CheckMasterNameAndChannel()
		{
			//	if there is no UID for this name, then find it and set it.
			//
			if (m_MasterObjectName.GetUID() == nameString::e_InvalidUID)
			{
				nameUID nuid = nameMgr::GetNameUIDFromString( m_MasterObjectName.GetValue() );
				m_MasterObjectName.SetUID( nuid );
			}
			else
			{
				//	FIX - very inefficient.  we need a way to do this only when the
				//	name changes and not check this every frame!
				//
				//	if there is a UID for this name, then find it and verify the name
				//
				//std::string name_string;
				//nameUID nuid = nameMgr::GetNameString( m_MasterObjectName.GetUID(), name_string );
				//if (m_MasterObjectName.GetString() != name_string)
				//	m_MasterObjectName.SetString(name_string);
			}

			//	if there is no channel for this driver, then find it and hook
			//	if up.
			//
			if (   (m_pMasterChannel == NULL)
				&& (m_MasterObjectName.GetUID() != nameString::e_InvalidUID))
			{
				FindAndSetMasterChannel();
			}
		}

private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void MasterObjectNameChanged(prtyProperty* i_pProperty, bool i_bDirty)
		{
			this->MarkDirty();

			//	hook-up the MasterChannel
			//
			SetMasterChannel(NULL);
			FindAndSetMasterChannel();
		};

protected:
	prtyName			m_MasterObjectName;
	std::string			m_ChannelName;
	xxxChannel*			m_pMasterChannel;
	xxxChannel&			m_Channel;

	chDefs::Name		m_ChunkName;
};
