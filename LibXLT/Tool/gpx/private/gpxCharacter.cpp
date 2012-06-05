/*****************************************************************************
**	gpxCharacter.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxCharacter.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxCharacter::gpxCharacter(smdlSubdivCharacter &i_Character)
:	m_Character(i_Character)
{
#if USE_PROXIES
	m_SubdivLevel = i_Character.GetCurrentSubdivLevel();
	m_JointDisplay = i_Character.GetJointDisplay();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxCharacter::~gpxCharacter()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxCharacter::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	PROXY_SET_OR_STORE(m_Character, SetCurrentSubdivLevel, m_SubdivLevel, i_SubdivLevel);
}
void gpxCharacter::SetJointDisplay(smdlSubdivCharacter::JointDisplay i_Display)
{
	PROXY_SET_OR_STORE(m_Character, SetJointDisplay, m_JointDisplay, i_Display);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
int gpxCharacter::GetCurrentSubdivLevel() const
{
	return PROXY_GET(m_Character, GetCurrentSubdivLevel, m_SubdivLevel);
}
smdlSubdivCharacter::JointDisplay gpxCharacter::GetJointDisplay() const
{
	return PROXY_GET(m_Character, GetJointDisplay, m_JointDisplay);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxCharacter::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Character.SetCurrentSubdivLevel( m_SubdivLevel );
	m_Character.SetJointDisplay( m_JointDisplay );

	this->SetNeedsUpdate(false);
#endif

	return true;
}
