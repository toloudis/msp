/*****************************************************************************
**	tmlnParser.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnParser.hpp"

// tool
#include "Support/tmln/tmlnDriverInfo.hpp"

// library
#include "Core/ch/chReader.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <map>

//============================================================================
//============================================================================
namespace tmlnParser
{
	namespace
	{
		// Chunk name for representing general parsers, not limited to scope of a system
		const chDefs::Name c_XXXX = chDefs::MakeName('X', 'X', 'X', 'X'); 

		//
		struct ScopedChunkName
		{
			chDefs::Name m_System;
			chDefs::Name m_Parser;

			ScopedChunkName(chDefs::Name i_System, chDefs::Name i_Parser)
				: m_System(i_System), m_Parser(i_Parser) {}
		};
		bool operator<( const ScopedChunkName& a, const ScopedChunkName& b )
		{
			return ( (a.m_System < b.m_System) 
				  || (a.m_System == b.m_System && a.m_Parser < b.m_Parser) );
		}

		//
		class DriverParserMap
		{
			public:
				typedef std::map<ScopedChunkName, tmlnDriverParser*> ParserMap;

				~DriverParserMap()
				{
					ParserMap::iterator it = m_Parsers.begin();
					ParserMap::iterator end = m_Parsers.end();
					for( ; it!=end; ++it )
					{
						delete it->second;
					}
					m_Parsers.clear();
				}
				void AddParser( chDefs::Name i_Name, 
								tmlnDriverParser* i_pParser )
				{
					// If no scope is given, use XXXX as the scope. Represents
					// "general" parsers that apply to more than one system.
					AddParser(c_XXXX, i_Name, i_pParser);
				}
				void AddParser( chDefs::Name i_SystemCode,
								chDefs::Name i_Name, 
								tmlnDriverParser* i_pParser )
				{
					ScopedChunkName name(i_SystemCode, i_Name);
					DBG_ASSERT2(m_Parsers.find(name) == m_Parsers.end(), "Overlap in parser codes: %s in scope %s", (const char*)(&i_Name), (const char*)(&i_SystemCode));
					m_Parsers[name] = i_pParser;
				}
				const tmlnDriverParser* GetParser( chDefs::Name i_SystemCode,
												   chDefs::Name i_Name ) const
				{
					ScopedChunkName name(i_SystemCode, i_Name);
					ParserMap::const_iterator it = m_Parsers.find(name);
					if (it == m_Parsers.end())
					{
						ScopedChunkName name2(c_XXXX, i_Name);
						it = m_Parsers.find(name2);
					}
					if (it == m_Parsers.end())
						return NULL;
					return it->second;
				}
			private:
				ParserMap m_Parsers;
		};

		DriverParserMap *l_pParserMgr = NULL;
	}


	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize()
	{
		l_pParserMgr = new DriverParserMap;
	}
	void DeInitialize()
	{
		delete l_pParserMgr;
	}

	//--------------------------------------------------------------------
	// Adds a method for creating parsers for a given driver type
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Adds a method for creating parsers for a given driver type.
	// The system code (the chunk name for the document) can be used to
	// limit the scope of the chunk name in order to prevent overlaps
	// in the chunk names. If not included, the chunk name will be
	// put in a general list and will be considered for all systems.
	//--------------------------------------------------------------------
	void AddDriverParser(chDefs::Name i_Name, 
						 tmlnDriverParser* i_pParser)
	{
		l_pParserMgr->AddParser(i_Name, i_pParser);
	}
	void AddDriverParser(chDefs::Name i_SystemCode,
						 chDefs::Name i_Name, 
						 tmlnDriverParser* i_pParser)
	{
		l_pParserMgr->AddParser(i_SystemCode, i_Name, i_pParser);
	}

	//--------------------------------------------------------------------
	// Reads driver info from given chunk name.  This may return NULL,
	// if no parser can handle the chunk.
	//--------------------------------------------------------------------
	tmlnDriverInfo* ReadDriver(chReader& i_Reader,
							chDefs::Name i_SystemCode, 
							chDefs::Name i_Name,
							chDefs::Version i_Version,
							chDefs::Size i_Size)
	{
		//DBG_LOG0("Attempting read driver");
		const tmlnDriverParser* pParser = l_pParserMgr->GetParser( i_SystemCode, i_Name );
		if (!pParser) return NULL;

		tmlnDriverInfo* pDriver = pParser->Create();
		if (pDriver)
		{
			//DBG_LOG0("Reading driver");
			pParser->Read(i_Reader, i_Version, i_Size, *pDriver);
		}
		return pDriver;
	}

	//--------------------------------------------------------------------
	// Writes driver to file, returning false if no parser can handle
	//	the driver's format.
	//--------------------------------------------------------------------
	bool WriteDriver( chWriter& i_Writer,
					  chDefs::Name i_SystemCode, 
					  const tmlnDriverInfo& i_Driver )
	{
		//DBG_LOG0("Attempting write driver");
		const tmlnDriverParser* pParser = l_pParserMgr->GetParser( i_SystemCode, 
																   i_Driver.GetBaseChunkName() );
		if (!pParser) return false;

		//DBG_LOG0("Writing driver");
		pParser->Write(i_Writer, i_Driver);
		return true;
	}

	//--------------------------------------------------------------------
	//   ReadDrivers - read drivers into vector
	//--------------------------------------------------------------------
	void ReadDrivers(	chReader& i_Reader,
						chDefs::Name i_SystemCode, 
						std::vector<tmlnDriverInfo*> &o_Drivers )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			tmlnDriverInfo * driver = tmlnParser::ReadDriver(i_Reader, i_SystemCode, name, version, size);
			if (driver)
			{
				o_Drivers.push_back(driver);
			}
			else
			{
				DBG_WARNING1("No parser found to read driver chunk: %s", (const char*)(&name));
			}
			i_Reader.FinishChunk();
		}
	}

	//--------------------------------------------------------------------
	//   WriteDrivers - write drivers from vector into file
	//--------------------------------------------------------------------
	void WriteDrivers( chWriter& o_Writer,
					   chDefs::Name i_SystemCode, 
					   const std::vector<tmlnDriverInfo*> &i_Drivers )
	{
		int num_drivers = i_Drivers.size();
		//DBG_LOG1("Num drivers writing: %d", num_drivers);
		for (int d=0; d<num_drivers; d++)
		{
			// Write driver info
			tmlnParser::WriteDriver( o_Writer, i_SystemCode, *i_Drivers[d] );
		}
	}

} // end of namespace
