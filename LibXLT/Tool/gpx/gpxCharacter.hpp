/*****************************************************************************
**	gpxCharacter.hpp
**
**	This class is a thread-safe proxy for a smdlSubdivCharacter.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_CHARACTER_HPP
#error gpxCharacter.hpp multiply included
#endif
#define GPX_CHARACTER_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef SMDL_SUBDIVCHARACTER_HPP
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#endif 


//============================================================================
//============================================================================
class gpxCharacter : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxCharacter(smdlSubdivCharacter &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxCharacter();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetCurrentSubdivLevel(int i_SubdivLevel);
	void SetJointDisplay(smdlSubdivCharacter::JointDisplay i_Display);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	int GetCurrentSubdivLevel() const;
	smdlSubdivCharacter::JointDisplay GetJointDisplay() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	smdlSubdivCharacter &m_Character;

#if USE_PROXIES
	int m_SubdivLevel;
	smdlSubdivCharacter::JointDisplay m_JointDisplay;
#endif
};
