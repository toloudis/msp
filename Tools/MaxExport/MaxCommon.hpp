
/*****************************************************************************
**  MaxCommon.hpp
**
**	Common max header files and some preprocessor definitions
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef MAXEXP_MAXCOMMON_HPP
#define MAXEXP_MAXCOMMON_HPP

#include "max.h"
#include "iparamb2.h"
#include "modstack.h"
#include "polyobj.h"
#include "mesh.h"
#include "modstack.h"
#include "triobj.h"
#include "shaders.h"
#include "stdmat.h"
#include "MeshNormalSpec.h"

#include <string>
#define SGPU_USE_MAX_8 (MAX_VERSION_MAJOR >= 8)
//#define TIME_EXPORT_START 0

#if SGPU_USE_MAX_8
#else
#define SGPU_USE_MAX_7
#endif


#define SKIN_CLASS_ID_A 9815843
#define SKIN_CLASS_ID_B 87654
#define POINTCACHE_OSM_ID_A 0x270f1fe3
#define POINTCACHE_OSM_ID_B 0x3b14999
#define UNKNOWN_CLASS_ID 0 //no superclass ID

namespace MaxExp
{
		typedef enum {  eModel=0, eVertAnim, eCameraAnim, eParticle} Intent;		
		const std::string& IntentAsChar( Intent e);
}
#endif //MAXEXP_MAXCOMMON_HPP
