/****************************************************************************\
**  g2dWindow.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dWindow.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dWindow::g2dWindow() 
:	m_Width(0), 
	m_Height(0), 
	m_BitDepth(0),
	m_VirtualWidth(800), 
	m_VirtualHeight(600)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dWindow::~g2dWindow()
{
}

//------------------------------------------------------------------------
//	IsWindowed returns true if we are in windowed mode
//------------------------------------------------------------------------
bool g2dWindow::IsWindowed() const
{
	return m_bIsWindowed;
}

//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the screen.
//------------------------------------------------------------------------
const g2dPFD& g2dWindow::GetPixelFormat() const
{
	return m_PFD;
}

//------------------------------------------------------------------------
//	Return the pixel format of the backbuffer surface.
//------------------------------------------------------------------------
const g2dPFD& g2dWindow::GetBackBufferPixelFormat() const
{
	return m_backBufPFD;
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the screen.
//------------------------------------------------------------------------
void g2dWindow::GetDimensions(int& o_Width, int& o_Height) const
{
	o_Width = m_Width;
	o_Height = m_Height;
}

//------------------------------------------------------------------------
//	GetBitDepth returns the BitDepth of the screen.
//------------------------------------------------------------------------
void g2dWindow::GetBitDepth(int& o_BitDepth) const
{
	o_BitDepth = m_BitDepth;
}

//------------------------------------------------------------------------
//	SetRenderResolution is not supported by default
//------------------------------------------------------------------------
//virtual
void g2dWindow::SetRenderResolution(int i_Width, int i_Height)
{
}

//------------------------------------------------------------------------
//	GetPresentRect returns where the rendered image is drawn in the window
//------------------------------------------------------------------------
//virtual
void g2dWindow::GetPresentRect(int& o_X, int& o_Y, int& o_Width, int& o_Height) const
{
	o_X = 0;
	o_Y = 0;
	o_Width = m_Width;
	o_Height = m_Height;
}

//------------------------------------------------------------------------
//	SetVirtualResolution sets the resolution that the gui scales itself to
//------------------------------------------------------------------------
void g2dWindow::SetVirtualResolution( int i_nWidth, int i_nHeight )
{
	m_VirtualWidth = i_nWidth;
	m_VirtualHeight = i_nHeight;
}

//------------------------------------------------------------------------
//	GetVirtualResolution gets the resolution that the gui scales itself to
//------------------------------------------------------------------------
void g2dWindow::GetVirtualResolution( int& o_nWidth, int& o_nHeight ) const
{
	o_nWidth = m_VirtualWidth;
	o_nHeight = m_VirtualHeight;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dWindow::SetWindowProperties(bool i_Windowed, const g2dPFD &i_PFD,
									const g2dPFD& i_backPFD,
									int i_Width, int i_Height, int i_BitDepth)
{
	m_bIsWindowed = i_Windowed;
	m_PFD = i_PFD;
	m_backBufPFD = i_backPFD;
	m_Width = i_Width;
	m_Height = i_Height;
	m_BitDepth = i_BitDepth;
}

