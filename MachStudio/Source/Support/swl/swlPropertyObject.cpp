/****************************************************************************\
**  swlPropertyObject.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/swl/swlPropertyObject.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Support/swl/swlData.hpp"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
swlPropertyObject::swlPropertyObject(const std::string& i_Name,
									   swlData& i_Data)
:	m_Name(i_Name), m_Data(i_Data)
{
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEnable), "Software Lighting", "Enable software lighting") );

	m_Data.m_bEnable.AddCallback(new prtyCallbackWrapper<swlPropertyObject>(this, &swlPropertyObject::UpdateFlags));
	// Added properties here
	/*AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bCastsShadow), "Surface Flags", "Casts shadows") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesShadow), "Surface Flags", "Receives light from shadow sources") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bShadowHull), "Surface Flags", "Fragment is invisible, but casts shadow") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDoubleSided), "Surface Flags", "Render both sides of triangles") );

	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseBakedTexture), "Baked Texture", "Enable baked texture");
	AddProperty( pPUII);

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAOInherit), "Ambient Occlusion", "Inherit from global settings"));
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesOcclusion), "Ambient Occlusion", "Set to have AO computed for this character"));
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bReceivesGI), "Global Illumination", "Set to have GI computed for this character"));
	m_Data.m_bCastsShadow.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bReceivesShadow.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bShadowHull.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));
	m_Data.m_bDoubleSided.AddCallback(new prtyCallbackWrapper<fgmtPropertyObject>(this, &fgmtPropertyObject::UpdateFlags));*/
}

swlPropertyObject::~swlPropertyObject()
{
	m_Interest.clear();
}


std::string swlPropertyObject::GetDisplayName() const
{
	return m_Name;	
}

//--------------------------------------------------------------------
//	Register interest
//--------------------------------------------------------------------
void swlPropertyObject::RegisterInterest(swlInterest* i_Interest)
{
	if (std::find(m_Interest.begin(), m_Interest.end(), i_Interest) == m_Interest.end())
		m_Interest.push_back(i_Interest);
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void swlPropertyObject::UpdateFlags(prtyProperty *i_pProperty, bool i_bDirty)
{
	for (int i = 0; i < m_Interest.size(); i++)
	{
		m_Interest[i]->GatherData(m_Data);
	}
	/*const int num_frags = m_FragmentProxies.size();
	for (int i=0; i<num_frags; i++)
	{
		m_FragmentProxies[i]->SetCastsShadow( m_Data.m_bCastsShadow.GetValue() );
		m_FragmentProxies[i]->SetReceivesShadow( m_Data.m_bReceivesShadow.GetValue() );
		m_FragmentProxies[i]->SetShadowHull( m_Data.m_bShadowHull.GetValue() );
		m_FragmentProxies[i]->SetDoubleSided( m_Data.m_bDoubleSided.GetValue() );
	}*/
}