/*****************************************************************************
**	xtraScriptObject.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraScriptObject.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/xtra/xtraPropertyData.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraScriptObject::xtraScriptObject()
: m_pObject(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraScriptObject::~xtraScriptObject()
{
	// shared_ptrs cleanu pup data,
	// m_Channels owned by tmlnScriptObject class
}


//--------------------------------------------------------------------
// Set pointer to the object that should receive the custom
//	properties.
//--------------------------------------------------------------------
void xtraScriptObject::SetExtraPropertyObject(prtyObject *i_pObject)
{
	m_pObject = i_pObject;
}

//--------------------------------------------------------------------
// Returns true if this object already has a property with the 
//	given name. Custom properties have to have unique names.
//--------------------------------------------------------------------
bool xtraScriptObject::HasPropertyWithName(const std::string& i_AttributeName)
{
	DBG_ASSERT(m_pObject, "Extra Property object needs to be set");
	if (m_pObject)
	{	
		if (m_pObject->GetProperty(i_AttributeName) != NULL)
			return true;
	}
	return false;
}

//--------------------------------------------------------------------
// Get index for custom property with given name.
//--------------------------------------------------------------------
int xtraScriptObject::GetIndexForName(const std::string& i_AttributeName)
{
	const int num_attrs = m_Data.size();
	for (int i=0; i<num_attrs; i++)
	{
		if (m_Data[i]->m_Name.GetValue() == i_AttributeName)
			return i;
	}

	return -1;
}

//--------------------------------------------------------------------
//	Adds new property to object according to given data
//--------------------------------------------------------------------
bool xtraScriptObject::AddCustomProperty(const xtraPropertyData &i_Data)
{
	DBG_ASSERT(m_pObject, "Extra Property object needs to be set");
	if (m_pObject)
	{
		if (!HasPropertyWithName(i_Data.m_Name.GetValue()))
		{
			shared_ptr<xtraPropertyData> prty_data(i_Data.Clone());
			shared_ptr<prtyPropertyUIInfo> custom_info;
			tmlnChannel* pChannel = NULL;

			if (i_Data.m_Type == e_Boolean)
			{
				xtraBooleanPropertyData *pBoolPrty = dynamic_cast<xtraBooleanPropertyData*>(prty_data.get());
				DBG_ASSERT(pBoolPrty, "Wrong data structure type for custom property");
				pBoolPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());
				custom_info.reset( 
					new prtyCheckBoxUIInfo(&pBoolPrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue()) );
				
				tmlnChannelBoolean *pBoolChannel = new tmlnChannelBooleanProperty(pBoolPrty->m_Value);
				pChannel = pBoolChannel;
				pBoolChannel->SetOriginalState( pBoolPrty->m_Value.GetValue() );

			}
			else if (i_Data.m_Type == e_Float)
			{
				xtraFloatPropertyData *pFloatPrty = dynamic_cast<xtraFloatPropertyData*>(prty_data.get());
				DBG_ASSERT(pFloatPrty, "Wrong data structure type for custom property");
				pFloatPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());

				prtyRangedFloatUIInfo *pRFUII = new prtyRangedFloatUIInfo(&pFloatPrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue());
				pRFUII->SetMinimum( pFloatPrty->m_Minimum.GetValue() );
				pRFUII->SetMaximum( pFloatPrty->m_Maximum.GetValue() );
				pRFUII->SetDecimalPlaces( pFloatPrty->m_DecimalPlaces.GetValue() );
				custom_info.reset( pRFUII );

				
				tmlnChannelFloat *pFloatChannel = new tmlnChannelFloatProperty(pFloatPrty->m_Value);
				pChannel = pFloatChannel;
				pFloatChannel->SetOriginalValue( pFloatPrty->m_Value.GetValue() );
			}
			else if (i_Data.m_Type == e_Color)
			{
				xtraColorPropertyData *pColorPrty = dynamic_cast<xtraColorPropertyData*>(prty_data.get());
				DBG_ASSERT(pColorPrty, "Wrong data structure type for custom property");
				pColorPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());

				if (pColorPrty->m_bShowAlpha.GetValue())
				{
					custom_info.reset(  new prtyColorRGBAEditUIInfo(&pColorPrty->m_Value, 
											   i_Data.m_Category.GetValue(), 
											   i_Data.m_Description.GetValue()) );
				}
				else
				{
					custom_info.reset(  new prtyColorRGBEditUIInfo(&pColorPrty->m_Value, 
											   i_Data.m_Category.GetValue(), 
											   i_Data.m_Description.GetValue()) );
				}
				
				tmlnChannelColor *pColorChannel = new tmlnChannelColorProperty(pColorPrty->m_Value);
				pChannel = pColorChannel;
				pColorChannel->SetOriginalColor( pColorPrty->m_Value.GetValue() );
			}
			else if (i_Data.m_Type == e_String)
			{
				xtraStringPropertyData *pStringPrty = dynamic_cast<xtraStringPropertyData*>(prty_data.get());
				DBG_ASSERT(pStringPrty, "Wrong data structure type for custom property");
				pStringPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());

				prtyTextBoxUIInfo *pTBUII = new prtyTextBoxUIInfo(&pStringPrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue());
				pTBUII->SetMultiline( pStringPrty->m_bMultiline.GetValue() );
				custom_info.reset( pTBUII );

				// Text properties cannot be animated
			}
			else if (i_Data.m_Type == e_Position)
			{
				xtraPositionPropertyData *pPositionPrty = dynamic_cast<xtraPositionPropertyData*>(prty_data.get());
				DBG_ASSERT(pPositionPrty, "Wrong data structure type for custom property");
				pPositionPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());
				custom_info.reset( 
					new prtyVector3dEditUpDownUIInfo(&pPositionPrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue()) );
				
				tmlnChannelPosition *pPositionChannel = new tmlnChannelPositionProperty(pPositionPrty->m_Value);
				pChannel = pPositionChannel;
				pPositionChannel->SetOriginalPosition( pPositionPrty->m_Value.GetValue() );
			}
			else if (i_Data.m_Type == e_Orientation)
			{
				xtraOrientationPropertyData *pRotationPrty = dynamic_cast<xtraOrientationPropertyData*>(prty_data.get());
				DBG_ASSERT(pRotationPrty, "Wrong data structure type for custom property");
				pRotationPrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());
				
				prtyVector3dEditUpDownUIInfo *pPVEUDUII = new prtyVector3dEditUpDownUIInfo(&pRotationPrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue());	
				pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
				pPVEUDUII->SetDecimalPlaces(1);
				custom_info.reset( pPVEUDUII );
				
				tmlnChannelOrientation *pRotationChannel = new tmlnChannelOrientationProperty(pRotationPrty->m_Value);
				pChannel = pRotationChannel;
				float x=0,y=0,z=0;
				pRotationPrty->m_Value.GetEuler(x,y,z);
				pRotationChannel->SetOriginalValue(x,y,z);
			}
			else if (i_Data.m_Type == e_Texture)
			{
				xtraTexturePropertyData *pTexturePrty = dynamic_cast<xtraTexturePropertyData*>(prty_data.get());
				DBG_ASSERT(pTexturePrty, "Wrong data structure type for custom property");
				pTexturePrty->m_Value.SetPropertyName(i_Data.m_Name.GetValue());
				
				prtyFileChooserUIInfo* pFCUII = 
					new prtyFileChooserUIInfo(&pTexturePrty->m_Value, 
										   i_Data.m_Category.GetValue(), 
										   i_Data.m_Description.GetValue());
				pFCUII->SetDirectoryCategory("Textures");
				custom_info.reset( pFCUII );
				
				tmlnChannelFilePath *pTextureChannel = new tmlnChannelFilePathProperty(pTexturePrty->m_Value);
				pChannel = pTextureChannel;
				pTextureChannel->SetOriginalValue( pTexturePrty->m_Value.GetValue() );
			}
			else
			{
				DBG_WARNING("Unrecognized property type: " << i_Data.m_Type);
				return false;
			}

			m_pObject->AddProperty( custom_info );
			m_UIInfos.push_back( custom_info );
			m_Data.push_back(prty_data);

			// Note, it is okay to have a property that does not animate,
			// in this case, the pointer in the m_Channels array will be NULL.
			if (pChannel)
			{
				pChannel->SetName(i_Data.m_Name.GetValue().c_str());
				this->AddChannel(pChannel);
			}
			m_Channels.push_back(pChannel);

			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
// Delete custom property with given index
//--------------------------------------------------------------------
void xtraScriptObject::DeleteCustomProperty(const std::string& i_PropertyName, 
									bool i_bDeletePropertyDrivers)
{
	int index = this->GetIndexForName(i_PropertyName);
	if (index >= 0)
		DeleteCustomProperty(index, i_bDeletePropertyDrivers);
}
void xtraScriptObject::DeleteCustomProperty(int i_Index, bool i_bDeletePropertyDrivers)
{
	DBG_ASSERT(m_pObject, "Extra Property object needs to be set");
	DBG_ASSERT(i_Index < m_Data.size(), "Index " << i_Index << " out of range");

	tmlnChannel *channel = m_Channels[i_Index];
	if (channel != NULL)
	{
		if (i_bDeletePropertyDrivers)
		{
			// Get all drivers from channel, and delete them since they are
			// attached to a channel that will not exist anymore
			const int num_drivers = channel->GetNumDrivers();
			std::vector<tmlnDriver*> drivers(num_drivers);
			for (int i=0; i<num_drivers; i++)
			{
				// gather drivers
				drivers[i] = &(channel->Driver(i));
			}
			for (int i=0; i<num_drivers; i++)
			{
				// remove drivers
				this->RemoveDriver(drivers[i]);
			}
			// delete drivers
			envSTLHelpers::DeleteContainer(drivers);
		}

		this->RemoveChannel(channel);
		delete channel;
	}

	shared_ptr<prtyPropertyUIInfo> custom_info = m_UIInfos[i_Index];
	m_pObject->RemoveProperty( custom_info );
	m_Data.erase(m_Data.begin() + i_Index);
	m_UIInfos.erase(m_UIInfos.begin() + i_Index);
	m_Channels.erase(m_Channels.begin() + i_Index);
}

//--------------------------------------------------------------------
// Get vector of custom properties in order to store info to file
//--------------------------------------------------------------------
void xtraScriptObject::GetCustomPropertyData(std::vector<shared_ptr<xtraPropertyData>> &o_Data) const
{
	int num_attrs = m_Data.size();
	o_Data.resize(num_attrs);
	for (int i=0; i<num_attrs; i++)
	{
		o_Data[i].reset(m_Data[i]->Clone());
	}
}

//--------------------------------------------------------------------
// Create custom properties to match data list
//--------------------------------------------------------------------
void xtraScriptObject::SetCustomPropertyData(const std::vector<shared_ptr<xtraPropertyData>> &i_Data)
{
	// remove old custom propertys
	int num_attrs = m_Data.size();
	for (int i=num_attrs-1; i>=0; i--)
		this->DeleteCustomProperty(i, false);

	num_attrs = i_Data.size();
	for (int i=0; i<num_attrs; i++)
		this->AddCustomProperty(*i_Data[i]);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int xtraScriptObject::GetNumCustomProperties() const
{
	return m_Data.size();
}

//--------------------------------------------------------------------
// Access to channel - this may return NULL if the property
// cannot be animated.
//--------------------------------------------------------------------
const tmlnChannel* xtraScriptObject::GetCustomChannel(int i_Index) const
{
	DBG_ASSERT(i_Index < m_Channels.size(), "Index out of range - " << i_Index);
	return m_Channels[i_Index];
}
tmlnChannel* xtraScriptObject::GetCustomChannel(int i_Index)
{
	DBG_ASSERT(i_Index < m_Channels.size(), "Index out of range - " << i_Index);
	return m_Channels[i_Index];
}

