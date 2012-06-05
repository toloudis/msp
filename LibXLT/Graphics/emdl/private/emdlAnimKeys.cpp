/*****************************************************************************
**  emdlAnimKeys.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlAnimKeys.hpp"

#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlAnimKeys::emdlAnimKeys()
:	m_bAdditive(false), 
	m_bIgnoreJointOrientation(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlAnimKeys::~emdlAnimKeys()
{
	envSTLHelpers::DeleteContainer(m_TranslateChannels);
	envSTLHelpers::DeleteContainer(m_RotateChannels);
	envSTLHelpers::DeleteContainer(m_ScaleChannels);
	envSTLHelpers::DeleteContainer(m_VisibleChannels);
}

//--------------------------------------------------------------------
// Name of joint that is root of this subanimation
//--------------------------------------------------------------------
//void emdlAnimKeys::SetNameOfRoot(const std::string& i_Name)
//{
//	m_NameOfRoot = i_Name;
//}
//const std::string& emdlAnimKeys::GetNameOfRoot() const
//{
//	return m_NameOfRoot;
//}

//--------------------------------------------------------------------
// AdditiveAnimation is true if the transformations are deltas
//	from the base pose and can be used in additive subanimations.
//--------------------------------------------------------------------
void emdlAnimKeys::SetAdditiveAnimation(bool i_bDelta)
{
	m_bAdditive = i_bDelta;
}


//--------------------------------------------------------------------
// Some animation that isn't coming from Maya should ignore the
//	joint orientation built into the scene graph.
//--------------------------------------------------------------------
void emdlAnimKeys::SetIgnoreJointOrientation(bool i_bIgnore)
{
	m_bIgnoreJointOrientation = i_bIgnore;
}
