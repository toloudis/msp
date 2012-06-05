/*****************************************************************************
**  setsNameInterest.hpp
**
**      the Select interest for the sets system.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_NAMEINTEREST_HPP
#error setsNameInterest.hpp multiply included
#endif
#define SETS_NAMEINTEREST_HPP

#include "Core/name/nameNameInterest.hpp"


//============================================================================
//============================================================================
class setsNameInterest : public nameNameInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		setsNameInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~setsNameInterest();

		//--------------------------------------------------------------------
		//	Returns object by name.  May return NULL.
		//--------------------------------------------------------------------
		virtual nameObject* GetObjectByName( const nameString& i_String );

		//--------------------------------------------------------------------
		//	Get a list of all the names
		//--------------------------------------------------------------------
		void GetNameList( nameList& io_NameList );

		//--------------------------------------------------------------------
		//	Get the name string of the object with the passed in UID.
		//--------------------------------------------------------------------
		void GetNameString( nameUID i_NameUID, std::string& o_NameString );
};
