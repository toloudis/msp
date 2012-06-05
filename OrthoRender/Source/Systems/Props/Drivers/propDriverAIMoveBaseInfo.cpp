/*****************************************************************************
**	propDriverAIMoveBaseInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "propDriverAIMoveBaseInfo.hpp"

#include "propDriverAIMoveBaseParser.hpp"
#include "propDriverAnimationFullParser.hpp"
#include "tmlnDriverSplineParser.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAIMoveBaseInfo::propDriverAIMoveBaseInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
	m_pAnimFullInfo = new propDriverAnimationFullInfo( propDriverAnimationFullParser::GetChunkName() );
	m_pSplineInfo	= new tmlnDriverSplineInfo( propDriverAIMoveBaseParser::GetSplineChunkName() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAIMoveBaseInfo::~propDriverAIMoveBaseInfo()
{
	delete m_pAnimFullInfo;
	delete m_pSplineInfo;
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* propDriverAIMoveBaseInfo::Clone()
{
	propDriverAIMoveBaseInfo* pDAIMBI = new propDriverAIMoveBaseInfo(*this);
	pDAIMBI->m_pAnimFullInfo	= new propDriverAnimationFullInfo(*(this->m_pAnimFullInfo));
	pDAIMBI->m_pSplineInfo		= new tmlnDriverSplineInfo(*(this->m_pSplineInfo));
	return pDAIMBI;
}
