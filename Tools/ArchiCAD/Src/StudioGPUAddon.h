/**********************************************************************

							STUDIOGPU ADDON

 	File:		StudioGPU.h

	Purpose:	Interface for a class to represent the StudioGPU addon

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#ifndef STUDIOGPU_STUDIOGPU_ADDON
#define STUDIOGPU_STUDIOGPU_ADDON

#include "Addon.h"

namespace studiogpu {
	
	/*!	Report a serious, unexpected failure to the user (prior to bailout) */
	void reportAddonFailure();
	
	/// A class to represent the StudioGPU addon
	class StudioGPUAddon : public encina::Addon {
	public:
		class ToolList;
		
		/*!	Start the StudioGPU addon
			@return An error code */
		static ResultCode startup();
		/*!	Shutdown the StudioGPU addon
			@return An error code */
		static ResultCode shutdown();
		
		/*!	Report a serious, unexpected failure to the user (prior to bailout) */
		virtual void reportUnexpectedFailure()	{ reportAddonFailure(); }
		
	protected:
		/*!	Startup the tools for this addon */
		virtual void startTools();
		
	private:
		/*!	Constructor */
		StudioGPUAddon();
		/*!	Copy Constructor */
		StudioGPUAddon(const StudioGPUAddon& source);	//No copy constructor
		/*!	Destructor */
		virtual ~StudioGPUAddon();
		
		ToolList* m_tool;
	};

}

#endif	//STUDIOGPU_STUDIOGPU_ADDON
