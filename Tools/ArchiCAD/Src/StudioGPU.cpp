/**********************************************************************

							STUDIOGPU ADDON

 	File:		StudioGPU.cpp

	Purpose:	Implementation of the main class for the StudioGPU addon

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#include "StudioGPUAddon.h"

#include "Alert.h"
#include "Environment.h"
#include "Event.h"
#include "StudioGPUResource.h"
#include "Version.h"

using namespace studiogpu;
using namespace encina;


namespace {

	/*--------------------------------------------------------------------
		Report a serious, unexpected failure to the user (prior to bailout)
	  --------------------------------------------------------------------*/
	void reportUnexpectedFailure()
	{
		ErrorAlert alert;
		alert.show(app.getResString(applicationString, internalProblemStr));
	} //reportUnexpectedFailure

}

/*--------------------------------------------------------------------
	Required initialisation procedure

	return: An error code
  --------------------------------------------------------------------*/
ResultCode __ACENV_CALL Initialize()
{
	try {
		ResultCode result = noErr;
		result = StudioGPUAddon::startup();
		if ((result == noErr) && (addon != 0)) {
			addon->install();
		}
	}
	catch(...)	{
		reportAddonFailure();
	}
	return noErr;
} //Initialize


/*--------------------------------------------------------------------
	Required environment check procedure
	
	envir: The current environment parameters

	return: The addon type
  --------------------------------------------------------------------*/
API_AddonType __ACENV_CALL CheckEnvironment(API_EnvirParams* envir)
{
	API_AddonType addonType = APIAddon_DontRegister;
	try {
		ResultCode result = StudioGPUAddon::startup();
		if ((result == noErr) && (addon != 0)) {
			if (envir->serverInfo.serverApplication == APIAppl_ArchiCADID)
				 addonType = APIAddon_Normal;
			CadString text = app.getResString(applicationString, addonNameStr);
			text.copyTo(envir->addOnInfo.name, 256);
			text = app.getResString(applicationString, addonDescStr);
			text.copyTo(envir->addOnInfo.description, 1024);
			if ((addon != 0) && (addon->test()))
				addonType = APIAddon_Preload;
			StudioGPUAddon::shutdown();
		}
	} catch(...)	{
		reportAddonFailure();
	}
	return addonType;
} //CheckEnvironment


/*--------------------------------------------------------------------
	Required interface declaration procedure

	return: A result code
  --------------------------------------------------------------------*/
ResultCode __ACENV_CALL RegisterInterface(void)
{
	ResultCode result = noErr;
	try {
		result = StudioGPUAddon::startup();
		if ((result == noErr) && (addon != 0)) {
			addon->initialise();
			StudioGPUAddon::shutdown();
		}
	} catch(...)	{
		reportAddonFailure();
	}
	return result;
} //RegisterInterface


/*--------------------------------------------------------------------
	Required shutdown procedure

	return: An error code
  --------------------------------------------------------------------*/
ResultCode __ACENV_CALL FreeData(void)
{
	try {
		if (addon != 0)
			return StudioGPUAddon::shutdown();
	} catch(...)	{
		reportAddonFailure();
	}
	return noErr;
} //FreeData
