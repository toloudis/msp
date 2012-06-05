/*****************************************************************************
**	animImageButton.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/animImageButton.hpp"


//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
animImageButton::animImageButton(fcmdModeTemplate* i_pParentMode, QWidget* i_pWidget)
: m_pParentMode(i_pParentMode)
{
	//instantiate the actual qt button for this object
	//m_pAnimatedPixmap = new animPixmapItem(i_pWidget);
}

//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
animImageButton::~animImageButton()
{
	if(m_pAnimatedPixmap)
		delete m_pAnimatedPixmap;
	m_pAnimatedPixmap = NULL;
}

//------------------------------------------------------------------------
// Set the function callbacks
//------------------------------------------------------------------------
void animImageButton::SetHoverCallback()
{}
void animImageButton::SetLeftClickCallback()
{}
void animImageButton::SetRightClickCallback()
{}
void animImageButton::SetDragCallback()
{}
