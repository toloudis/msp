/*****************************************************************************
**  swlData.h
**
**      The swlData stores all mray/rman environmental lighting information
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SWL_DATA_HPP
#error swlData.hpp multiply included
#endif
#define SWL_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#include <string>


class swlData
{
	public:
		
		swlData()
			:	m_bEnable("Enable", false),
				m_bEnableBG("Enable BG", false)
		{
		}

		//----------------------------------------------------------------------------
		// Name
		//----------------------------------------------------------------------------
		inline void SetName(const std::string& i_Name);
		inline const std::string& GetName() const;

		prtyBoolean m_bEnable;
		prtyBoolean m_bEnableBG;
private:
		std::string m_Name;	// object name that contains this swlData
};

//====================================================================
//	Name
//====================================================================
inline void swlData::SetName(const std::string& i_Name)
{
	m_Name = i_Name;
}
inline const std::string& swlData::GetName() const
{
	return m_Name;
}

