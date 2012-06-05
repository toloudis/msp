/********************************************************************************************\
** pfxData.hpp
**
**		Data relating to the post effect in each renderlayer
**
**  studio|gpu
\********************************************************************************************/
#pragma once

#ifdef PFX_DATA_HPP
#error pfxData.hpp multiply included
#endif
#define PFX_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

class effShaderParams;

class pfxData
{
public:
	enum UIType
	{
		e_ViewPort = 0,
		e_RenderLayer,
		e_UITypeNum
	};
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	pfxData();

	prtyBoolean		m_bActive;
	prtyFilePath	m_Name;

	shared_ptr<effShaderParams> m_pShaderParams;
};
