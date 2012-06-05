/****************************************************************************\
**	rpnPanelViewer.hpp
**
**	Derived type of render viewer in order to set up things for
**	this panel before rendering.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_PANELVIEWER_HPP
#error rpnPanelViewer.hpp multiply included
#endif
#define RPN_PANELVIEWER_HPP

#ifndef G3D_VIEWER_HPP
#include "Graphics/g3d/g3dViewer.hpp"
#endif
#ifndef G3D_LAYERCONTAINER_HPP
#include "Graphics/G3d/g3dLayerContainer.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================
class fsLocator;
class itString;
class maFloatRGBA;
class scrText;
class camsDirectorsCut;
class g3dSceneNode;
class cmpsWorldAxis;
class gpxText;


//============================================================================
//============================================================================
class rpnPanelViewer : public g3dViewer, public g3dLayerContainer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rpnPanelViewer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~rpnPanelViewer();

	//--------------------------------------------------------------------
	// Return the layer index for this viewer's icons.
	//--------------------------------------------------------------------
	int GetIconLayerIndex() const;

	//--------------------------------------------------------------------
	//	Prepare viewer´s layers for rendering
	//--------------------------------------------------------------------
	virtual void Think( float i_fSimTime );

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//--------------------------------------------------------------------
	virtual void Render( float i_fSimTime, 
						 bool i_bClear = true );

	//------------------------------------------------------------------------
	// CreateTextLabel - give font to use 
	//------------------------------------------------------------------------
	void CreateTextLabel(const fsLocator &i_FontPath);

	//------------------------------------------------------------------------
	// SetTextVisible - turn display of text on/off
	//------------------------------------------------------------------------
	void SetTextVisible(bool i_bShow);

	//------------------------------------------------------------------------
	// SetTextColor - set color of the camera name text label
	//------------------------------------------------------------------------
	void SetTextColor(const maFloatRGBA& i_Color);

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
	int					m_IconLayerIndex;
	scrText*			m_pTextLabel;
	gpxText*			m_pTextLabelProxy;
	//bool				m_bTextVisible;
	std::string			m_TextString;
	cmpsWorldAxis*		m_pWorldAxis;
	bool				m_bAllowShadows;
	camsDirectorsCut*	m_pDirectorsCut;
	std::vector<g3dSceneNode*> m_LayerRoots;

};

