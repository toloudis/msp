/*****************************************************************************\
**	api3dSubdiv.hpp
**
**		Manages the current subdiv level for the scene.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_SUBDIV_HPP
#error api3dSubdiv.hpp multiply included
#endif
#define API3D_SUBDIV_HPP

#include <string>


//============================================================================
//	Forward References
//============================================================================
class api3dSubdivInterest;


//============================================================================
//============================================================================
class api3dSubdivInterest
{
	public:
		//--------------------------------------------------------------------
		//	SubdivLevelChanged - notification that global level has changed.
		//--------------------------------------------------------------------
		virtual void SubdivLevelChanged( int i_Level ) = 0;
};


//============================================================================
//============================================================================
namespace api3dSubdiv
{
	//--------------------------------------------------------------------
	//	SubdivLevel - the current subdivision level for models
	//--------------------------------------------------------------------
	void SetSubdivLevel( int i_Level );
	int GetSubdivLevel();

	//--------------------------------------------------------------------
	//	RegisterSubdivInterest() - add a Subdiv interest to the system
	//--------------------------------------------------------------------
	void RegisterSubdivInterest( api3dSubdivInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterSubdivInterest() - remove a Subdiv interest from the system.
	//
	//	Note: this will NOT delete the Subdiv interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterSubdivInterest( api3dSubdivInterest* i_pInterest );

};
