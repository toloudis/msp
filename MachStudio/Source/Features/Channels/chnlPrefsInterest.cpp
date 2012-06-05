/****************************************************************************\
**	chnlPrefsInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlPrefsInterest.hpp"

#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"

//	library
//#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//	PrefsDataUpdated - the data was updated
//--------------------------------------------------------------------
//virtual 
void chnlPrefsInterest::PrefsDataUpdated(const PrefsData& i_Data)
{
#ifdef USE_WXWIDGETS
	chnlTraxDialog::FormInstance->UpdateScale();
#endif
}
