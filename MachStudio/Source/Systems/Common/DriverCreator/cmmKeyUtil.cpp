/*****************************************************************************
**  cmmKeyUtil.cpp
**
**		see .hpp
**
**	Each key is made as the Key command makes one
**	(tmlnCreator::CreateKeyForChannel: the channel's own key driver, at
**	the current time), with the object's overlap check off, and then moved
**	to its time and given its value and blend. Keys are zero-length
**	drivers at distinct times, so they never overlap. As the Key command
**	does, the channel editor is shown the object again when it is the
**	selected one, so its new keys appear.
**
**	Extra Large Technology
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmKeyUtil.hpp"
#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Drivers/Color/tmlnDriverColor.hpp"
#include "Drivers/Color/tmlnDriverColorInfo.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
#include "Drivers/Position/tmlnDriverPosition.hpp"
#include "Drivers/Position/tmlnDriverPositionInfo.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif

#include <functional>
#include <memory>

namespace cmmKeyUtil
{
	namespace
	{
		//--------------------------------------------------------------------
		// the object's overlap check off while it lives
		//--------------------------------------------------------------------
		struct ConflictCheckOff
		{
			tmlnScriptObject& m_Object;
			bool m_bPrevious;
			ConflictCheckOff(tmlnScriptObject& io_Object)
				: m_Object(io_Object), m_bPrevious(io_Object.GetCheckForConflicts())
			{
				m_Object.SetCheckForConflicts(false);
			}
			~ConflictCheckOff() { m_Object.SetCheckForConflicts(m_bPrevious); }
		};

		//--------------------------------------------------------------------
		// the driver's info (a copy) of type xxxInfo, its value set by
		// i_Set, back into the driver; false when the info isn't that type
		//--------------------------------------------------------------------
		template <class xxxDriver, class xxxInfo>
		bool set_info(xxxDriver& io_Driver, const std::function<void (xxxInfo&)>& i_Set)
		{
			std::unique_ptr<tmlnDriverInfo> pInfo(io_Driver.GetDriverInfo());
			xxxInfo* pTyped = dynamic_cast<xxxInfo*>(pInfo.get());
			if (!pTyped)
				return false;
			i_Set(*pTyped);
			io_Driver.SetDriverInfo(*pTyped);
			return true;
		}

		//--------------------------------------------------------------------
		// the key's value into the driver, through its info (as a saved key
		// is read); false for another kind of driver
		//--------------------------------------------------------------------
		bool set_value(tmlnDriver& io_Driver, const cmmKey& i_Key)
		{
			const float* v = i_Key.m_Value;
			if (tmlnDriverFloat* pFloat = dynamic_cast<tmlnDriverFloat*>(&io_Driver))
				return set_info<tmlnDriverFloat, tmlnDriverFloatInfo>(*pFloat, [v](tmlnDriverFloatInfo& io_Info)
				{
					io_Info.m_Value = v[0];
				});
			if (tmlnDriverPosition* pPosition = dynamic_cast<tmlnDriverPosition*>(&io_Driver))
				return set_info<tmlnDriverPosition, tmlnDriverPositionInfo>(*pPosition, [v](tmlnDriverPositionInfo& io_Info)
				{
					io_Info.m_Value = maPoint3d(v[0], v[1], v[2]);
				});
			if (tmlnDriverColor* pColor = dynamic_cast<tmlnDriverColor*>(&io_Driver))
				return set_info<tmlnDriverColor, tmlnDriverColorInfo>(*pColor, [v](tmlnDriverColorInfo& io_Info)
				{
					io_Info.m_Value = maFloatRGBA(v[0], v[1], v[2], v[3]);
				});
			if (tmlnDriverOrientation* pOrientation = dynamic_cast<tmlnDriverOrientation*>(&io_Driver))
				return set_info<tmlnDriverOrientation, tmlnDriverOrientationInfo>(*pOrientation, [v](tmlnDriverOrientationInfo& io_Info)
				{
					// the Euler angles the driver reads back
					maRotation rotation(v[0], v[1], v[2], v[3]);
					rotation.Normalize();
					rotation.GetEuler(io_Info.m_X, io_Info.m_Y, io_Info.m_Z);
				});
			return false;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool AddKeys(tmlnScriptObject* io_pObject, tmlnChannel* io_pChannel, const std::vector<cmmKey>& i_Keys, bool i_bStep)
	{
		if (!io_pObject || !io_pChannel || i_Keys.empty())
			return false;

		maTime first = i_Keys.front().m_Time, last = i_Keys.front().m_Time;
		{
			ConflictCheckOff check_off(*io_pObject);
			for (const cmmKey& key : i_Keys)
			{
				tmlnDriver* pDriver = tmlnCreator::CreateKeyForChannel(io_pObject, io_pChannel);
				if (!pDriver)
					return false;
				if (!set_value(*pDriver, key))
				{
					// the first key tells: this channel's keys aren't ours to fill
					io_pObject->RemoveDriver(pDriver);
					delete pDriver;
					return false;
				}
				pDriver->SetBeginTime(key.m_Time);
				pDriver->SetEndTime(key.m_Time);
				pDriver->SetBlendType(i_bStep ? tmlnDriver::e_NoBlending : tmlnDriver::e_Previous);
				undoUndoMgr::AddOperation(new cmmAddDriverOperation(io_pObject->CreateReferenceToSelf(), pDriver));
				if (key.m_Time < first)
					first = key.m_Time;
				if (key.m_Time > last)
					last = key.m_Time;
			}
		}
		io_pObject->NotifyDriverChanged();

		// the channel editor lists the selected object's channels as they were when it was shown
		if (io_pObject == tmlnSelectionUtil::GetSelectedScriptObject())
			chnlDialogUtil::ObjectSelected(io_pObject);

		// show the keys
		maTime min_time = tmlnTimeLine::GetMinimum();
		maTime max_time = tmlnTimeLine::GetMaximum();
		if (first < min_time || last > max_time)
			tmlnTimeLine::SetTimeRange(first < min_time ? first : min_time, last > max_time ? last : max_time);
		return true;
	}
}
