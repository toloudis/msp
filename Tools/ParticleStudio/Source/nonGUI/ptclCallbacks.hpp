/********************************************************************************************\
**  ptclCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when particle data changes
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	PTCL_CALLBACKS_HPP
#error	ptclCallbacks.hpp included recursively.
#endif
#define	PTCL_CALLBACKS_HPP


//--------------------------------------------------------------------------------------------
// for receiving selection callbacks when user clicks on material with mouse
//--------------------------------------------------------------------------------------------
class ptclParticleChangedCallback
{
public:
	virtual void ParticleChanged(int i_Index) = 0;
};

