/*****************************************************************************
**	cmraDriverDirector.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverDirector.hpp"

#include "Systems/Cameras/Data/cmraData.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverDirectorParser.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
//#include "Support/tmln/tmlnScriptObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDirector::cmraDriverDirector(cmraChannelCapture& i_Channel)
:	cmraDriverCapture(i_Channel)
{
	this->SetRestoreOriginalValue(true);
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverDirector::GetDriverInfo() const
{
	cmraDriverCaptureInfo *pInfo = new cmraDriverCaptureInfo(cmraDriverDirectorParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	return pInfo;
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void cmraDriverDirector::Operate(const maTime& i_Time)
{
	cmraScriptObject* pDirCamera = NULL;
	cmraScriptObject* pCapCamera = NULL;
	const cmraDriverCapture* pCapDriver = NULL;

	//	Find the correct camera to view at i_Time
	std::vector<tmlnScriptObject*> objects;
	tmlnTimelineMgr::GetObjects( objects );
	for (int i=0; i < objects.size(); i++)
	{
		cmraScriptObject* pCSO = dynamic_cast<cmraScriptObject*>(objects[i]);
		if (pCSO != NULL)
		{
			int num_drivers = objects[i]->GetNumDrivers();
			if (num_drivers > 0)
			{
				for (int d=0; d < num_drivers; ++d)
				{
					const tmlnDriver* pDriver = &(objects[i]->GetDriver(d));
					//	if the driver is the director driver, store this camera
					//	since this is the director camera we want.
					if (dynamic_cast<const cmraDriverDirector*>(pDriver) == this)
					{
						pDirCamera = pCSO;
					}
					else
					{
						//	if the driver is within the time, and is a capture driver and
						//	ISN'T a director driver, then store this driver.
						//
						//	NOTE: This doesn't handle multiple cameras at the same time.
						//	It just picks the last one encountered.
						//
						if (   (objects[i]->GetDriver(d).IsWithin(i_Time))
							&& (dynamic_cast<const cmraDriverDirector*>(pDriver) == NULL)
							&& (dynamic_cast<const cmraDriverCapture*>(pDriver) != NULL))
						{
							pCapCamera = pCSO;
							pCapDriver = dynamic_cast<const cmraDriverCapture*>(pDriver);

							//DBG_TRACE(i << " - " << objects[i]->GetDisplayName().c_str() << " = " << d << " " << objects[i]->GetDriver(d).GetName().c_str());
						}
					}
				}
			}
		}
	}

	//	if all of these are not NULL then we have a camera we want to mimic.
	//
	if (   (pDirCamera != NULL)
		&& (pCapCamera != NULL)
		&& (pCapDriver != NULL))
	{
		//cmraCameraData dir_data = pDirCamera->GetBaseData();
		//DBG_TRACE("-------");
		//DBG_TRACE("dir Pos = " << dir_data.m_Position.GetValue());
		//DBG_TRACE("dir Tar = " << dir_data.m_Target.GetValue());

		const cmraCameraData new_data = pCapCamera->GetPickObject()->GetData();
		cmraCameraData updated_data = new_data;
		updated_data.m_Name.SetValue( pDirCamera->GetName() );
		pDirCamera->SetBaseData( updated_data );

		//dir_data = pDirCamera->GetBaseData();
		//DBG_TRACE("cap Pos = " << new_data.m_Position.GetValue());
		//DBG_TRACE("cap Tar = " << new_data.m_Target.GetValue());
		//DBG_TRACE("dir Pos = " << dir_data.m_Position.GetValue());
		//DBG_TRACE("dir Tar = " << dir_data.m_Target.GetValue());
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverDirector::GetClipFillColor() const
{
	return maFloatRGBA( 0.1f, 0.4f, 0.1f, 1.0f );
}

