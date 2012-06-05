/*
	File:		QTSound.h

	Contains:	Prototypes for QTSound.c file
	
	Written by:	Scott Kuechle
				(based heavily on QuickTime sample code in Inside Macintosh:QuickTime)
	 
	Copyright:	© 1998 by Apple Computer, Inc. All rights reserved
	  
	Change History (most recent first)
	   
		<1>		6/26/98		srk		first file
		
		 
*/

#ifndef MNM_QUICKTIMEUTIL_HPP
#include "Support/mnm/mnmQuickTimeUtil.hpp" // just for USE_QUICKTIME define
#endif 

#ifdef USE_QUICKTIME
#include "Movies.h"

//==============================================================================
//	library pragmas
//==============================================================================
#pragma comment(lib,"QTMLClient.lib")


//==============================================================================
//==============================================================================
namespace mnmQuickTimeSound
{
void QTSound_CreateMySoundTrack(Movie theMovie, const char* i_SoundFile);
}

#endif // USE_QUICKTIME
