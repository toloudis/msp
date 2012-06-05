/*****************************************************************************
**  MaxCommon.cpp
**
**	Collection of all utility functions and classes, 
**	that glues Sgpu api and max api
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxCommon.hpp"
using namespace std;

namespace {
	const string sModelIntent("model");
	const string sVertexAnimationIntent("vertex animation");
	const string sCameraAnimationIntent("camera animation" );
	const string sParticleIntent("particle");
}
namespace MaxExp
{
	const std::string& IntentAsChar( Intent e)
	{
		switch( e )
		{
		case eVertAnim:
			return sVertexAnimationIntent;
		case eCameraAnim:
			return sCameraAnimationIntent;
		case eParticle:		
			return sParticleIntent;
		default: case eModel:
			return sModelIntent;
		}

	}
} //namespace SgpuExp