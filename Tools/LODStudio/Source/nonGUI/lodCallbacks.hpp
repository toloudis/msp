/********************************************************************************************\
**  lodCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when LOD data changes
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	LOD_CALLBACKS_HPP
#error	lodCallbacks.hpp included recursively.
#endif
#define	LOD_CALLBACKS_HPP


//--------------------------------------------------------------------------------------------
// for receiving selection callbacks when user clicks on material with mouse
//--------------------------------------------------------------------------------------------
class lodLODChangedCallback
{
public:
	virtual void LODChanged(int i_Index) = 0;
};

