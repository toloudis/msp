/*****************************************************************************
**	chtrDriverAnimationFull.hpp
**
**	Derived driver class for full object animation
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_DRIVERANIMATIONFULL_HPP
#error chtrDriverAnimationFull.hpp multiply included
#endif
#define CHTR_DRIVERANIMATIONFULL_HPP

#ifndef TMLN_DRIVERANIMATIONFULL_HPP
#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"
#endif

//============================================================================
//============================================================================
class chtrDriverAnimationFull : public tmlnDriverAnimationFull
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
							chDefs::Name i_ChunkName,
							const fsLocator &i_CharacterDir);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~chtrDriverAnimationFull();

	//--------------------------------------------------------------------
	//	Find the full locator for the given animation filename.
	//--------------------------------------------------------------------
	virtual fsLocator GetAnimationLocator( const itString &i_AnimFilename ) const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const fsLocator&	GetCharacterDir() const;

private:
	fsLocator	m_CharacterDir;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const fsLocator&	chtrDriverAnimationFull::GetCharacterDir() const
{
	return m_CharacterDir;
}
