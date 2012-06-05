/*****************************************************************************
**	animImageButton.hpp
**
**		an image that will act as a button and process events when clicked.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_IMAGEBUTTON_HPP
#error animImageButton.hpp multiply included
#endif
#define ANIM_IMAGEBUTTON_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif

#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif


//============================================================================
//============================================================================
class animImageButton
{
public:
	//------------------------------------------------------------------------
	// constructor
	//------------------------------------------------------------------------
	animImageButton(fcmdModeTemplate* i_pParentMode, QWidget* i_pWidget);
	
	//------------------------------------------------------------------------
	// destructor
	//------------------------------------------------------------------------
	~animImageButton();

	//------------------------------------------------------------------------
	// Set the function callbacks
	//------------------------------------------------------------------------
	void SetHoverCallback();
	void SetLeftClickCallback();
	void SetRightClickCallback();
	void SetDragCallback();

private:
	fcmdModeTemplate* m_pParentMode;
	animPixmapItem* m_pAnimatedPixmap;
};
