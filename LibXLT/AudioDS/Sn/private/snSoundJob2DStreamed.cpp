/****************************************************************************\
**  snSoundJob2DStreamed.cpp
**
**      snSoundJob2DStreamed.hpp defines the snSoundJob2DStreamed class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snSoundJob2DStreamed.hpp"
#include "AudioDS/sn/private/snSoundJob2DStreamedPAC.hpp"


//========================================================================
//	default constructors
//========================================================================
snSoundJob2DStreamed::snSoundJob2DStreamed()
:	snSoundJob2D( new snSoundJob2DStreamedPAC )
{
	Init();
}


//========================================================================
//========================================================================
snSoundJob2DStreamed::snSoundJob2DStreamed( const snSoundJob2DStreamed& i_CopyFrom )
: snSoundJob2D( new snSoundJob2DStreamedPAC )
{
	Init();

	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJob2DStreamed::~snSoundJob2DStreamed()
{
	int x = 0;
	x = x;
}


//========================================================================
//	InitData()
//========================================================================
void	
snSoundJob2DStreamed::InitData()
{
	if ( this->GetPAC() == NULL )
	{
		this->SetPAC( new snSoundJob2DStreamedPAC );
	}
}


//========================================================================
//	Init()
//========================================================================
void	
snSoundJob2DStreamed::Init()
{
	InitData();
}


