/*****************************************************************************
**  chParserMgr.cpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/ch/chParserMgr.hpp"

//========================================================================
// Constructor
//========================================================================
chParserMgr::chParserMgr()
{
}

//========================================================================
// Destructor
//========================================================================
//virtual 
chParserMgr::~chParserMgr()
{
	Parsers::iterator it = m_Parsers.begin();
	Parsers::iterator end = m_Parsers.end();
	for( ; it!=end; ++it )
	{
		delete it->second;
	}
	m_Parsers.clear();
}

//========================================================================
// AddParser will allow this and derived classes to add an instance of each
// parser.  Duplicate name entries will assert in the STL.  The base
// class does this on construction, and you should too.
// i_Name must be unique.  i_Parser must be non-NULL.
// Note:  The base class takes ownership and will destroy the parser object
// when its destructor is called.
//========================================================================
void chParserMgr::AddParser( chDefs::Name i_Name, chParser* i_Parser )
{
	DBG_ASSERT0(i_Parser, "Invalid NULL parser!");

	Parsers::iterator it = find_parser(i_Name);
	DBG_ASSERT0(m_Parsers.end() == it, "Invalid duplicate name!" );
	m_Parsers.push_back(std::pair<chDefs::Name, chParser*>(i_Name, i_Parser));
}

//========================================================================
// ReplaceParser will replace the given name with the given parser.  It 
// must already exist.  This is useful for children who want to override
// what gets created when a base-class chunk is read, but they don't want
// or need to create a chunk of their own.
//========================================================================
void chParserMgr::ReplaceParser( chDefs::Name i_Name, chParser* i_Parser )
{
	DBG_ASSERT0(i_Parser, "Invalid NULL Parser!");
	Parsers::iterator it = find_parser(i_Name);
	DBG_ASSERT0(m_Parsers.end() != it, "Invalid absence of parser to replace!");
	delete it->second;
	it->second = i_Parser;
}
