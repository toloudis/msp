/*****************************************************************************
**	prtclObjectMgr.hpp
**
**	Manages the 3d representation of the Particles in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_OBJECTMGR_HPP
#error prtclObjectMgr.hpp multiply included
#endif
#define PRTCL_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef PRTCL_SCRIPTDATA_HPP
#include "Systems/Particles/Data/prtclScriptData.hpp"
#endif
#ifndef PRTCL_SCRIPTOBJECT_HPP
#include "Systems/Particles/Object/prtclScriptObject.hpp"
#endif
#ifndef PRTCL_OBJECT_HPP
#include "Systems/Particles/Object/prtclObject.hpp"
#endif


//============================================================================
//============================================================================
class prtclObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create prtclScriptObject from prtclScriptData
	//--------------------------------------------------------------------
	static prtclScriptObject* Create(const prtclScriptData &i_Data);
};


//============================================================================
//============================================================================
class prtclObjectMgr
	: public cmmObjectMgrGeomTemplate<  class prtclScriptObject, 
										class prtclObject, 
										class prtclParticlesData, 
										class prtclScriptData, 
										class prtclData,
										class prtclObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	static void  CleanUp();
		
	//--------------------------------------------------------------------
	// Clear out already created particles
	//--------------------------------------------------------------------
	static void ResetGenerators();

	//--------------------------------------------------------------------
	//	Pause/Unpause all the generators
	//--------------------------------------------------------------------
	static bool Paused();
	static void Pause( bool i_bPause );

	//--------------------------------------------------------------------
	//	Update a template with new particle data
	//--------------------------------------------------------------------
	static void UpdateTemplateFromData( const prtclData &i_Data, prtParticleGeneratorTemplate* io_pTemplate );
};
