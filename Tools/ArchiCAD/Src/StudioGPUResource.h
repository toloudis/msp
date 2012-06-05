/**********************************************************************

							STUDIOGPU ADDON

 	File:		StudioGPUResource.h

	Purpose:	Interface for the StudioGPU addon resources

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#ifndef STUDIOGPU_STUDIOGPU_RESOURCE
#define STUDIOGPU_STUDIOGPU_RESOURCE

namespace studiogpu {
	
	const short StudioGPUAddonResourceId = 32500;
	
	//Object owner type
	const OSType studioGPUOwner = 'SGPU';
	
	
		//Addon resource ID's
	enum AddonResource {
		studioGPUExportMenu = 32500
	};
	
	
	//Dialog resource ID's
	enum DialogResource {
		progressDialog = 32510
	};
	
	
		//Event sources
	const OSType fromStudioGPU = 'SGPE';
	
		//Event types
	const OSType exportStudioGPUEvent = 'EXSG';

	
		//IOFile type IDs
	enum IOFileType {
		saveAsStudioGPU = 1
	};

	
		//String resource IDs
	enum StringResource {
		titleString = 32520,
		generalString,
		notifyString,
		warningString,
		errorString,
		prefString,
		undoString,
		alignString,
		applicationString
	};

	
		//SubType String resource IDs
	enum SubTypeStringResource {
		subTypeName = 32540
	};
	
	
		//Title strings
	enum TitleString {
		sgpuProgressTitleStr = 1,
		exportElementStr
	};
	
	
		//Application strings
	enum ApplicationString {
		addonFolderStr = 1,
		addonNameStr,
		addonDescStr,
		internalProblemStr
	};
	
	
		//General strings
	enum GeneralString {
	};
	
	
		//Notification strings
	enum NotifyString {
	};
	

		//Warning strings
	enum WarningString {
	};
	

		//Error strings
	enum ErrorString {
	};
	
	
		//Preference strings
	enum PreferenceString {
	};

		//Undo strings
	enum UndoString {
		undoUnfoldStr = 1
	};
	
}

#endif	//STUDIOGPU_STUDIOGPU_RESOURCE
