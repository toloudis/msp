/*****************************************************************************
**	mslHeaderFooter.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslHeaderFooter.hpp"

#include "ShaderStrings.hpp"	// encoded templates

#include "Core/dbg/dbgMsg.hpp"

namespace mslHeaderFooter
{
	namespace
	{
		#define PLACES	1

		//----------------------------------------------------
		// decrypt()
		//----------------------------------------------------
		char decrypt(char i_c, int i_numPlaces)
		{
			if ( (i_c >= 31) && (i_c <= '~') )
			{
				return i_c + i_numPlaces;
			}
			return i_c;
		}

		//----------------------------------------------------
		// decryptArray()
		//----------------------------------------------------
		void decryptArray( std::string* i_encrypted , int i_size, std::string& o_decrypted )
		{
			for ( int i = 0 ; i < i_size ; i++ )
			{
				const char * encrypted = i_encrypted[i].c_str();
				std::string decrypted = "";
				for ( unsigned int j = 0 ; j < i_encrypted[i].size() ; j++ )
				{
					char encrypted_chr = encrypted[j];
					char decrypted_chr = decrypt( encrypted_chr , PLACES );
					decrypted.push_back(decrypted_chr);
				}
				o_decrypted.append(decrypted);
				o_decrypted.append("\n"); // "\r\n" ?
			}
		}
	}	// end of namespace


	//------------------------------------------------------------------------
	//	DecryptHeader
	//------------------------------------------------------------------------
	void DecryptHeader(std::string &o_Header)
	{
		decryptArray( g_Header, sizeof(g_Header)/sizeof(std::string), o_Header );

	}

	//------------------------------------------------------------------------
	//	DecryptFooter
	//------------------------------------------------------------------------
	void DecryptFooter(std::string &o_Footer)
	{
		decryptArray( g_Footer, sizeof(g_Footer)/sizeof(std::string), o_Footer );
	}

}	// end of namespace