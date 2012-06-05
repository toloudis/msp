/********************************************************************************************\
**  mtrCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when user clicks on material with mouse
**			2) when new model is loaded or cleared
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	MTR_CALLBACKS_HPP
#error	mtMaterialSelectCallback.hpp included recursively.
#endif
#define	MTR_CALLBACKS_HPP

//============================================================================================
// for receiving selection callbacks when user clicks on material with mouse
//============================================================================================
class mtrMaterialSelectCallback
{
public:
	virtual void SelectMaterial(int i_Index) = 0;
};

//============================================================================================
// for receiving callbacks when new model is loaded or cleared
//============================================================================================
class mtrModelChangeCallback
{
public:
	virtual void ModelChange() = 0;
};

