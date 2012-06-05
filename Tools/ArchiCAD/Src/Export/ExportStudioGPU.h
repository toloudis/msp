/**********************************************************************

							STUDIOGPU ADDON

 	File:		ExportStudioGPU.h

	Purpose:	Interface for the StudioGPU export class

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#ifndef STUDIOGPU_EXPORT_STUDIOGPU
#define STUDIOGPU_EXPORT_STUDIOGPU

#include "CommandSubscriber.h"
#include "FileSubscriber.h"

namespace studiogpu {
	
	/// A  tool class to save StudioGPU files
	class ExportStudioGPU : public encina::File3DSubscriber {
	public:
		/*!	Constructor */
		ExportStudioGPU();
		/*!	Destructor */
		virtual ~ExportStudioGPU()	{}

			//Event handling
		/*!	Respond to a subscribed Event
			@param event The subscribed Event
			@return True if the event is solely handled by this subscriber */
		virtual bool handleEvent(const encina::File3DEvent& event);

	};
	
	
	/// A command class to provide StudioGPU export facilities to other add-ons
	class StudioGPUService : public encina::CommandSubscriber {
	public:
		/*!	Constructor */
		StudioGPUService();
		/*!	Destructor */
		virtual ~StudioGPUService()	{}

			//Event handling
		/*!	Respond to a subscribed Event
			@param event The subscribed Event
			@return True if the event is solely handled by this subscriber */
		virtual bool handleEvent(const encina::CommandEvent& event);
		
	};
	
}

#endif	//STUDIOGPU_EXPORT_STUDIOGPU
