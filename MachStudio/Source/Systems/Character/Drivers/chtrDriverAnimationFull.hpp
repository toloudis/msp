/*****************************************************************************
**	chtrDriverAnimationFull.hpp
**
**	Derived driver class for full object animation
**
**	StudioGPU
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
	// i_SourceIdentity: the character's chtrData::m_SourceIdentity, the part
	// of its file its clips are built for
	chtrDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
							chDefs::Name i_ChunkName,
							const fsLocator &i_CharacterDir,
							const std::string &i_SourceIdentity);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~chtrDriverAnimationFull();

	//--------------------------------------------------------------------
	//	Find the full locator for the given animation filename.
	//--------------------------------------------------------------------
	//virtual fsLocator GetAnimationLocator( const itString &i_AnimFilename ) const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const fsLocator&	GetCharacterDir() const;

	//--------------------------------------------------------------------
	// The character's part of its file
	//--------------------------------------------------------------------
	virtual std::string GetAnimPart() const;

private:
	fsLocator	m_CharacterDir;
	std::string	m_SourceIdentity;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const fsLocator&	chtrDriverAnimationFull::GetCharacterDir() const
{
	return m_CharacterDir;
}
