/****************************************************************************\
**	rpnPanelViewer.hpp
**
**	Derived type of render viewer in order to set up things for
**	this panel before rendering.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_PANELVIEWER_HPP
#error rpnPanelViewer.hpp multiply included
#endif
#define RPN_PANELVIEWER_HPP

#ifndef G3D_VIEWER_HPP
#include "Graphics/g3d/g3dViewer.hpp"
#endif

class fsLocator;
class itString;
class scrText;
class camsDirectorsCut;

//============================================================================
//============================================================================
class rpnPanelViewer : public g3dViewer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rpnPanelViewer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rpnPanelViewer();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//--------------------------------------------------------------------
	virtual void Render( float i_fSimTime );

	//------------------------------------------------------------------------
	// CreateTextLabel - give font to use 
	//------------------------------------------------------------------------
	void CreateTextLabel(const fsLocator &i_FontPath);

	//------------------------------------------------------------------------
	// SetTextVisible - turn display of text on/off
	//------------------------------------------------------------------------
	void SetTextVisible(bool i_bShow);

	//------------------------------------------------------------------------
	// SetTextString - set string to display
	//------------------------------------------------------------------------
	void SetTextString(const std::string& i_String);
	const std::string& GetTextString() const;

	//------------------------------------------------------------------------
	// SetAllowShadows - allow shadows to be turned on for this viewer
	//------------------------------------------------------------------------
	void SetAllowShadows(bool i_bAllow);
	bool GetAllowShadows() const;

	//----------------------------------------------------------------------------
	// Set directors cut, this causes the camera to switch based 
	//	on the simulation time
	//----------------------------------------------------------------------------
	void SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut);
	camsDirectorsCut* GetDirectorsCut();
	
	//----------------------------------------------------------------------------
	// Returns true if the panel has a directors cut
	//----------------------------------------------------------------------------
	bool HasDirectorsCut() const;

private:
	scrText*			m_pTextLabel;
	bool				m_bTextVisible;
	std::string			m_TextString;
	bool				m_bAllowShadows;
	camsDirectorsCut*	m_pDirectorsCut;

};

