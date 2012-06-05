/**********************************************************************

							STUDIOGPU ADDON

 	File:		StudioGPUAddon.cpp

	Purpose:	Implementation of a class to represent the StudioGPU addon

	Target:		ArchiCAD 12

	Copyright:	Encina Ltd 2009

	Revision history:
	07-05-2009	RW	Created

 **********************************************************************/

#include "StudioGPUAddon.h"

#include "Alert.h"
#include "Environment.h"
#include "ExportStudioGPU.h"
#include "List.h"
#include "StudioGPUResource.h"

using namespace studiogpu;
using namespace encina;

namespace studiogpu {
	
	bool notified = false;
	
	typedef encina::List<encina::Subscriber> ToolListBase;
	
	class StudioGPUAddon::ToolList : public ToolListBase {
	public:
		ToolList() : ToolListBase()	{}
		virtual ~ToolList()	{}

	private:
		ToolList(const ToolList& source);
		
	};

}
	
/*--------------------------------------------------------------------
	Report a serious, unexpected failure to the user (prior to bailout)
  --------------------------------------------------------------------*/
void studiogpu::reportAddonFailure()
{
	ErrorAlert alert;
	alert.show(app.getResString(applicationString, internalProblemStr));
} //studiogpu::reportAddonFailure


/*--------------------------------------------------------------------
	Constructor
  --------------------------------------------------------------------*/
StudioGPUAddon::StudioGPUAddon() : Addon()
{
	m_tool = new StudioGPUAddon::ToolList();
} //StudioGPUAddon::StudioGPUAddon


/*--------------------------------------------------------------------
	Destructor
  --------------------------------------------------------------------*/
StudioGPUAddon::~StudioGPUAddon()
{
	delete m_tool;
} //StudioGPUAddon::~StudioGPUAddon


/*--------------------------------------------------------------------
	Start the StudioGPU tools
  --------------------------------------------------------------------*/
void StudioGPUAddon::startTools()
{
	m_tool->push_back(new StudioGPUService);
	m_tool->push_back(new ExportStudioGPU);
	for (ToolList::iterator i = m_tool->begin(); i != m_tool->end(); ++i)
		addSubscriber(*i);
} //StudioGPUAddon::startTools


/*--------------------------------------------------------------------
	Start the StudioGPU addon
	
	return: An error code
  --------------------------------------------------------------------*/
ResultCode StudioGPUAddon::startup()
{
	ResultCode result = Addon::startup();
	if (result == noErr) {
		StudioGPUAddon* mine = new StudioGPUAddon;
		addon = mine;
		mine->startTools();
	}
	return result;
} //StudioGPUAddon::startup


/*--------------------------------------------------------------------
	Shutdown the StudioGPU addon
	
	return: An error code
  --------------------------------------------------------------------*/
ResultCode StudioGPUAddon::shutdown()
{
	delete addon;
	addon = 0;
	return Addon::shutdown();
} //StudioGPUAddon::shutdown
