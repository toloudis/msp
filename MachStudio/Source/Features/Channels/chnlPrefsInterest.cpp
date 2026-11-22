/****************************************************************************\
**	chnlPrefsInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlPrefsInterest.hpp"

#ifdef MSP_GUI_QT
#include "Features/Channels/qtGUI/chnlTraxDialog.hpp"
#else
#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"
#endif

//	library
//#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//	PrefsDataUpdated - the data was updated
//--------------------------------------------------------------------
//virtual 
void chnlPrefsInterest::PrefsDataUpdated(const PrefsData& i_Data)
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	chnlTraxDialog::FormInstance->UpdateScale();
#endif
}
