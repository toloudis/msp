/*****************************************************************************
**  prtclSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Particles/prtclSystem.hpp"

#include "Systems/Particles/GUI/prtclAnimList.hpp"
#include "Systems/Particles/GUI/prtclCommands.hpp"
#include "Systems/Particles/GUI/prtclDialogInterest.hpp"
#include "Systems/Particles/GUI/prtclDialogUtil.hpp"
#include "Systems/Particles/Data/prtclDocumentInterest.hpp"
#include "Systems/Particles/Timeline/prtclDriverCreator.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmitParser.hpp"
#include "Systems/Particles/GUI/prtclGeomList.hpp"
#include "Systems/Particles/Object/prtclObjectMgr.hpp"
#include "Systems/Particles/Object/prtclSelectInterest.hpp"
#include "Systems/Particles/GUI/prtclTextureList.hpp"
#include "Systems/Particles/Object/prtclTimeInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"

//	tools
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/vis/visMgr.hpp"

//	lib
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"


namespace prtclSystem
{

namespace
{
	// Interests implemented through templates
	typedef cmmNameInterestTemplate<prtclObjectMgr, prtclObject> prtclNameInterest;
	typedef cmmPickInterestTemplate<prtclObjectMgr> prtclPickInterest;
	typedef cmmVisibleInterestTemplate<prtclObjectMgr> prtclVisibleInterest;

	prtclNameInterest*		l_pParticlesNI = 0;
	prtclPickInterest*		l_pParticlesPI = 0;
	prtclSelectInterest*	l_pParticlesSI = 0;
	prtclTimeInterest*		l_pParticlesTI = 0;
	prtclVisibleInterest*	l_pParticlesVI = 0;
	prtclDialogInterest*	l_pParticlesDI = 0;
}

	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_ParticleDataDir)
	{
		prtclAnimList::Init( i_ParticleDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		prtclAnimList::SetAppDirectory( i_AppDir );
		prtclGeomList::Init( i_ParticleDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		prtclGeomList::SetAppDirectory( i_AppDir );
		// assign directories for textures
		prtclTextureList::Init( i_ParticleDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Textures)) );
		prtclTextureList::SetAppDirectory( i_AppDir );

		// Initialize namespaces
		prtclDialogUtil::Init();
		prtclObjectMgr::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new prtclDocumentInterest());
		prtclCommands::SetupMenu();

		// register the pick interest
		if ( l_pParticlesPI == 0 )
		{
			l_pParticlesPI = new prtclPickInterest();
			pick3dMgr::RegisterPickInterest( l_pParticlesPI );
		}

		// register the select interest
		if ( l_pParticlesSI == 0 )
		{
			l_pParticlesSI = new prtclSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pParticlesSI );
		}

		// register the name interest
		if ( l_pParticlesNI == 0 )
		{
			l_pParticlesNI = new prtclNameInterest();
			nameMgr::RegisterNameInterest( l_pParticlesNI );
		}

		// register the visible interest
		if ( l_pParticlesVI == 0 )
		{
			l_pParticlesVI = new prtclVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pParticlesVI );
		}

		// register the time interest
		if ( l_pParticlesTI == 0 )
		{
			l_pParticlesTI = new prtclTimeInterest();
			tmlnTimeLine::AddTimeInterest( l_pParticlesTI );
		}

		// register the dialog interest
		if ( l_pParticlesDI == 0 )
		{
			l_pParticlesDI = new prtclDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pParticlesDI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new prtclDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		prtclDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pParticlesPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pParticlesPI );
			delete l_pParticlesPI;
			l_pParticlesPI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pParticlesSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pParticlesSI );
			delete l_pParticlesSI;
			l_pParticlesSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pParticlesNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pParticlesNI );
			delete l_pParticlesNI;
			l_pParticlesNI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pParticlesVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pParticlesVI );
			delete l_pParticlesVI;
			l_pParticlesVI = 0;
		}

		// register the time interest
		if ( l_pParticlesTI != 0 )
		{
			tmlnTimeLine::RemoveTimeInterest( l_pParticlesTI );
			delete l_pParticlesTI;
			l_pParticlesTI = 0;
		}

		// register the dialog interest
		if ( l_pParticlesDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pParticlesDI );
			delete l_pParticlesDI;
			l_pParticlesDI = 0;
		}

		// Clean up namespaces
		prtclCommands::CleanUp();
		prtclObjectMgr::CleanUp();
		prtclDialogUtil::CleanUp();
		prtclTextureList::CleanUp();
		prtclGeomList::CleanUp();
		prtclAnimList::CleanUp();
	}

}	// end of namespace
