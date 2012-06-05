/*****************************************************************************
**	animTransitionMgr.hpp
**
**		Performs various transition animations on qt objects
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_TRANSITIONMGR_HPP
#error animTransitionMgr.hpp multiply included
#endif
#define ANIM_TRANSITIONMGR_HPP


//============================================================================
//============================================================================
class animTransitionMgr
{
public:
	//------------------------------------------------------------------------
	//access to the anim manager singleton
	//------------------------------------------------------------------------
	static animTransitionMgr* Instance();

public:
	//------------------------------------------------------------------------
	// constructor
	//------------------------------------------------------------------------
	animTransitionMgr();

	//------------------------------------------------------------------------
	// destructor
	//------------------------------------------------------------------------
	~animTransitionMgr();

	//------------------------------------------------------------------------
	// Rotate a given object with the given animation values
	//------------------------------------------------------------------------
	void RotateObject();

	//------------------------------------------------------------------------
	// Scale the object with the given values
	//------------------------------------------------------------------------
	void ScaleObject();

private:
	static animTransitionMgr* sm_Instance;
};


