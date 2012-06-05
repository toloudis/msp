/********************************************************************************************\
**  chrCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when user clicks on material with mouse
**			2) when new model is loaded or cleared
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	CHR_CALLBACKS_HPP
#error	chrCallbacks.hpp included recursively.
#endif
#define	CHR_CALLBACKS_HPP


//============================================================================================
// for receiving callbacks when new model is loaded or cleared
//============================================================================================
class chrModelChangeCallback
{
public:
	virtual void ModelChange() = 0;
};

