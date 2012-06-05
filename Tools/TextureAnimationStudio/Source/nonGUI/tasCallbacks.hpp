/********************************************************************************************\
**  tasCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when particle data changes
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#ifdef	TAS_CALLBACKS_HPP
#error	tasCallbacks.hpp included recursively.
#endif
#define	TAS_CALLBACKS_HPP


//--------------------------------------------------------------------------------------------
// for receiving selection callbacks when user clicks on material with mouse
//--------------------------------------------------------------------------------------------
class tasAnimatedTextureChangedCallback
{
public:
	virtual void AnimatedTextureChanged(int i_Index) = 0;
};

