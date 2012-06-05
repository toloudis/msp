
/*****************************************************************************
**  MaxObjectFlags.cpp
**
**	class which holds the GUI and data-binding members of
**  the max export options
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxObjectFlags.hpp"

namespace MaxExp
{

	void AllowedObjectFlags::Init()
	{
		const std::wstring dvalTrue(L"true");
		const std::wstring dvalFalse(L"false");

		std::wstring flagName(L"double sided");

		TObjectFlagParamDef flagDef;


		flagDef.m_FlagType = eObjectFlagBool;
		flagDef.m_DefaultValue = dvalFalse;
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"doublesided";
		insert( TParamMap::value_type( flagName, flagDef ) );




		flagDef.m_DefaultValue = dvalTrue;
		flagName = L"casts shadow";
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"castsshadow";
		insert( TParamMap::value_type( flagName, flagDef ) );

		flagDef.m_DefaultValue = dvalTrue;
		flagName = L"receives shadow";
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"receivesshadow";
		insert( TParamMap::value_type( flagName, flagDef ) );


		flagDef.m_DefaultValue = dvalFalse;
		flagName = L"shadow hull";
		insert( TParamMap::value_type( flagName, flagDef ) );		
		flagName = L"shadowhull";
		insert( TParamMap::value_type( flagName, flagDef ) );


		flagDef.m_DefaultValue = dvalFalse;
		flagName = L"triangle sort";
		insert( TParamMap::value_type( flagName, flagDef ) );		
		flagName = L"trianglesort";
		insert( TParamMap::value_type( flagName, flagDef ) );


		flagName = L"cloth";
		flagDef.m_DefaultValue = dvalFalse;
		insert( TParamMap::value_type( flagName, flagDef ) );


		flagName = L"export as subdiv";
		flagDef.m_DefaultValue = dvalFalse;
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"exportassubdiv";
		insert( TParamMap::value_type( flagName, flagDef ) );



		flagName = L"visible anim";
		flagDef.m_DefaultValue = dvalFalse;
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"visibleanim";
		insert( TParamMap::value_type( flagName, flagDef ) );



		flagName = L"low res";
		flagDef.m_DefaultValue = L"0";
		flagDef.m_FlagType = eObjectFlagInt;
		insert( TParamMap::value_type( flagName, flagDef ) );
		flagName = L"lowres";
		insert( TParamMap::value_type( flagName, flagDef ) );


	}

	bool AllowedObjectFlags::GeTObjectFlagParamDef( const std::wstring &o_FlagName, TObjectFlagParamDef &o_FlagDef )
	{

#if defined(_DEBUG)
		std::wstring flagNameCopy( o_FlagName);		
		transform( flagNameCopy.begin(), flagNameCopy.end(), flagNameCopy.begin(), less_i<wchar_t>::Traits<wchar_t>()  );
		DBG_ASSERT( flagNameCopy == o_FlagName, " o_FlagName :  should be in lower case" );
#endif
		bool bRetVal = false;
		TParamMap::iterator fit = find( o_FlagName );
		if( fit != end() )
		{
			bRetVal = true;
			o_FlagDef = fit->second;
		}
		return bRetVal;
	}



	AllowedObjectFlags AllowedObjectFlags::theAllowedObjectFlags;

}