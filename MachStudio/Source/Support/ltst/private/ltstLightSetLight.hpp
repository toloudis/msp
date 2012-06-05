/********************************************************************************************\
**  ltstLightSetLight.hpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSETLIGHT_HPP
#error ltstLightSetLight.hpp multiply included
#endif
#define LTST_LIGHTSETLIGHT_HPP

#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dLight;
class ltstLightSet;
class nameObject;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class ltstLightSetLight
{
public:
	nameObject* m_pNameObj;
	g3dLight* m_pLight;
	ltstLightSet* m_pContainingLightSet;

	ltstLightSetLight(nameObject* i_pNameObj, g3dLight* i_pLight)
		: m_pNameObj(i_pNameObj), m_pLight(i_pLight), m_pContainingLightSet(NULL) {}
};