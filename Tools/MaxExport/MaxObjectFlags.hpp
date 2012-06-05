/*****************************************************************************
**  MaxObjectFlags.hpp
**	class which sets/gets user defined properties for objects
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MAXOBJECTFLAGS_HPP
#error MAXEXP_MAXOBJECTFLAGS_HPP multuply defined!!
#endif
#define MAXEXP_MAXOBJECTFLAGS_HPP

#if !defined( MAXEXP_MAXEXPORTERUTILS_HPP )
#include "MaxExportUtils.hpp"
#endif
#if !defined(MAXEXP_MAXCOMMON_HPP )
#include "MaxCommon.hpp"
#endif

#include "max.h"
#include "resource.h"
#include <sstream>
#include <map>
#include <vector>

class fxXMLWriter;
namespace MaxExp
{
	class ExportLogger;
}


namespace MaxExp
{

	//========================================================================
	//enum for supporting the different types of object flag values
	//========================================================================
	typedef enum { eObjectFlagBool, eObjectFlagInt } EObjectFlagParamType;


	//========================================================================
	//	defenition of an object flag
	//========================================================================
	struct TObjectFlagParamDef {
		EObjectFlagParamType	m_FlagType;
		std::wstring m_DefaultValue;
	};
	
	
	//========================================================================
	//class responsible for getting and setting object fags on a node
	//========================================================================
	class AllowedObjectFlags: private std::map< std::wstring, TObjectFlagParamDef >
	{
	public:
		typedef  std::map< std::wstring, TObjectFlagParamDef >  TParamMap;

		AllowedObjectFlags( ) {
			Init();
		}

		~AllowedObjectFlags(){
			clear();
		}

		//Get the value of the given object flag from the node
		//If the value is not explicitly present, a default value is got.
		//In the case of the paam 'low res',
		//the integer vallue of low res is returned (ie low res = 0, 1, or 2)
		//Returns false if no such object flag is supported
		template< typename TRetVal >
		static bool GetValue( INode *i_pCurNode, const std::wstring &i_FlagName , TRetVal &o_Val );

		//Set the value of the given object flag of the node to the vaue supplied.
		//If the value is the default value, then it is not explicitly stored in the object
		//In the case of 'low res' integer value should be supplied ( low res = 0, 1 or 2)
		//If  the input object flag is not supported, false is returned
		template< typename TRetVal >
		static bool SetValue( INode *i_pCurNode, const std::wstring &i_FlagName , const TRetVal &i_Val );
	private:
		void Init();

		//unserialize from string to value
		template <typename T >
		bool UnSerializeFromString( const std::wstring &i_Val, T &o_Val )
		{
			return false;
		}

		template<>
		bool UnSerializeFromString<bool> ( const std::wstring &i_Val, bool &o_Val )
		{
			o_Val = true;
			if ( i_Val == L"false" )
			{
				o_Val = false;
			} else if (i_Val == L"0" )
			{
				o_Val = false;
			}
			return true;
		}


		template<>
		bool UnSerializeFromString<int> ( const std::wstring &i_Val, int &o_Val )
		{
			std::wstringstream ss( i_Val );
			ss >> o_Val;
			return true;
		}

		//serialize value to a string
		template <typename T >
		bool SerializeToString( const T &i_Val, std::wstring &o_Val )
		{
			return false;
		}
		template<>
		bool SerializeToString<bool> ( const bool &i_Val, std::wstring &o_Val )
		{
			o_Val= ( i_Val ) ? L"true" : L"false";
			return true;
		}

		template<>
		bool SerializeToString<int> ( const int &i_Val, std::wstring &o_Val )
		{
			std::wstringstream ss;
			ss << i_Val;
			o_Val = ss.str();
			return true;
		}


		//get default value
		template< typename T >
		bool GetDefaultValue( const std::wstring &i_FlagName , T &o_Val )
		{
			bool bRetVal = false;
			TObjectFlagParamDef flagDef;
			bRetVal = GeTObjectFlagParamDef( i_FlagName, flagDef );
			if( bRetVal )
			{
				T tempOutVal;
				bRetVal = UnSerializeFromString< T > ( flagDef.m_DefaultValue, tempOutVal );
				if( bRetVal )
				{
					o_Val = tempOutVal;
				}
			}
			return bRetVal;
		}


		bool GeTObjectFlagParamDef( const std::wstring &i_FlagName, TObjectFlagParamDef &o_FlagDef );

		template< typename T >
		bool ConvertValue( const std::wstring &i_FlagName , const std::wstring &i_Val, T &o_Val );

		static AllowedObjectFlags theAllowedObjectFlags;
	};


	template< typename TRetVal >
	bool AllowedObjectFlags::GetValue( INode *i_pCurNode, const std::wstring &i_FlagName , TRetVal &o_Val )
	{
		const int wBufLen = 256;
		wchar_t wBuf[ wBufLen ];
		std::wstring flagNameCopy( i_FlagName );
		transform( flagNameCopy.begin(), flagNameCopy.end(), flagNameCopy.begin(), less_i<wchar_t>::Traits<wchar_t>()  );

		TRetVal defaultVal;
		bool bSuccesfulDefaultVal = AllowedObjectFlags::theAllowedObjectFlags.GetDefaultValue<TRetVal>( flagNameCopy, defaultVal );
		if( !bSuccesfulDefaultVal )
		{
			return bSuccesfulDefaultVal;
		}
		MSTR userDefinedProperties;
		assert( i_pCurNode );
		i_pCurNode->GetUserPropBuffer(userDefinedProperties);


		if( userDefinedProperties.Length()> 0 )
		{
			wstring wUserDefinedProperties;
			MbcsToUnicode( userDefinedProperties , wUserDefinedProperties );
			transform( wUserDefinedProperties.begin(), wUserDefinedProperties.end(), wUserDefinedProperties.begin(), less_i<wchar_t>::Traits<wchar_t>()  );
			std::wstringstream wss( wUserDefinedProperties );
			while( !wss.eof() )
			{
				std::vector< std::wstring > tokens;
				wss.getline( wBuf, wBufLen );
				wstring wLine( wBuf );
				tokenize< std::wstring> ( wLine, L"#=", tokens );
				if ( tokens.size() > 0 )
				{
					std::wstring flagName = tokens[0];
					strip( flagName );
					if (flagName == flagNameCopy )
					{
						if (tokens.size() > 1 )
						{
							std::wstring flagVal = tokens[1];

							strip( flagVal );
							TObjectFlagParamDef flagDef;
							bool bIsFlagPresent = AllowedObjectFlags::theAllowedObjectFlags.GeTObjectFlagParamDef( flagNameCopy, flagDef );
							DBG_ASSERT( bIsFlagPresent, "param:  should be present" );
							TRetVal tempOutVal;
							bool bWasFlagProperlyConverted = AllowedObjectFlags::theAllowedObjectFlags.UnSerializeFromString< TRetVal >( flagVal, tempOutVal ); 
							DBG_ASSERT( bWasFlagProperlyConverted, "flagName was not properly converted" );
							if( bWasFlagProperlyConverted )
							{
								if( flagNameCopy == L"low res" )
								{
									o_Val = ( tempOutVal ) ? 1: 2;
								} else 
								{
									o_Val = tempOutVal;
								}
								return bWasFlagProperlyConverted;
							}
						}
					}
				}
			}//while

		}
		o_Val = defaultVal;
		return true;
	}



	template< typename TRetVal >
	bool AllowedObjectFlags::SetValue( INode *i_pCurNode, const std::wstring &i_FlagName , const TRetVal &i_Val )
	{	
		const int wBufLen = 256;
		wchar_t wBuf[ wBufLen ];
		std::wstring flagNameCopy( i_FlagName );
		transform( flagNameCopy.begin(), flagNameCopy.end(), flagNameCopy.begin(), less_i<wchar_t>::Traits<wchar_t>()  );

		TRetVal defaultVal;


		TObjectFlagParamDef flagDef;
		bool bIsFlagPresent = AllowedObjectFlags::theAllowedObjectFlags.GeTObjectFlagParamDef( flagNameCopy, flagDef );
		if( !bIsFlagPresent )
		{
			return false;
		}
		bool bSuccesfulDefaultVal = AllowedObjectFlags::theAllowedObjectFlags.GetDefaultValue<TRetVal>( flagNameCopy, defaultVal );
		DBG_ASSERT( bSuccesfulDefaultVal, "param: should be present" );

		std::wstring newVal;
		AllowedObjectFlags::theAllowedObjectFlags.SerializeToString< TRetVal >( i_Val, newVal );

		MSTR userDefinedProperties;
		DBG_ASSERT( i_pCurNode, "i_pCurNode: has to be non-NULL" );
		i_pCurNode->GetUserPropBuffer(userDefinedProperties);

		//Return immediately if
		//no user defined properties can be found

		std::wstringstream wss_out( std::ios_base::out );
		bool bIfFound = false;
		if( userDefinedProperties.Length()> 0 )
		{
			wstring wUserDefinedProperties;
			MbcsToUnicode( userDefinedProperties , wUserDefinedProperties );
			transform( wUserDefinedProperties.begin(), wUserDefinedProperties.end(), wUserDefinedProperties.begin(), less_i<wchar_t>::Traits<wchar_t>()  );
			std::wstringstream wss( wUserDefinedProperties, std::ios_base::in );
			bool bWasChanged = false;
			while( !wss.eof() )
			{
				std::vector< std::wstring > tokens;
				wss.getline( wBuf, wBufLen );
				wstring wLine( wBuf );
				tokenize< std::wstring> ( wLine, L"#=", tokens );
				if ( tokens.size() > 0 )
				{
					std::wstring flagName = tokens[0];
					strip( flagName );
					if (flagName == flagNameCopy )
					{
						bIfFound = true;
						if (tokens.size() > 1 )
						{
							std::wstring flagVal = tokens[1];
							strip( flagVal );
							//need to strip param val
							TRetVal tempOutVal;
							bool bWasFlagProperlyConverted = AllowedObjectFlags::theAllowedObjectFlags.UnSerializeFromString< TRetVal >( flagVal, tempOutVal ); 
							DBG_ASSERT( bWasFlagProperlyConverted, "flagName was not properly converted" );
							if( bWasFlagProperlyConverted )
							{

								if( flagNameCopy == L"low res" )
								{
									tempOutVal = (tempOutVal) ? 1 : 2;
								}

								if( tempOutVal != i_Val )
								{
									bWasChanged = true;
									if ( i_Val != defaultVal )
									{
										if( flagNameCopy == L"low res" )
										{
											if( i_Val == defaultVal )
											{
												wLine = L"";
											} else
											{
												wLine = L"#" + flagNameCopy + L"=" + ( ( i_Val==1) ? L"1" : L"0" );
											}

										} else
										{
											wLine = L"#" + flagNameCopy + L"=" + newVal;
										}
									} else
									{
										wLine = L"";

									}
								}
							}
						}

					}
				}
				strip( wLine );
				if ( !wLine.empty() ){
					wss_out << wLine;
					wss_out << std::endl;
				}
			}//while
		} 
		if( !bIfFound )
		{
			if ( i_Val != defaultVal )
			{
				std::wstring wLine;
				if(  flagNameCopy == L"low res" )
				{
					newVal = ( newVal == L"2" ) ? L"0" : L"1";
					wLine = L"#" + flagNameCopy + L"=" + newVal;
				} else 
				{
					wLine = L"#" + flagNameCopy + L"=" + newVal + L"\n";
				}
				wss_out << wLine;
				wss_out << std::endl;
			}
		}
		MSTR newUserDefinedProp;
		UnicodeToMbcs( wss_out.str(), newUserDefinedProp );
		i_pCurNode->SetUserPropBuffer( newUserDefinedProp );
		return true;
	}

}