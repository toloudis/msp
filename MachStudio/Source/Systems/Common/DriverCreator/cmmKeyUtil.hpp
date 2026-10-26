/*****************************************************************************
**  cmmKeyUtil.hpp
**
**		cmmKeyUtil adds keys at given times to a channel of a script object:
**	the key drivers the channel's creator makes (float, position, colour,
**	orientation), each an undoable operation, without the overlap
**	question a key at the current time would ask.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef CMM_KEYUTIL_HPP
#error cmmKeyUtil.hpp multiply included
#endif
#define CMM_KEYUTIL_HPP

#ifndef MA_TIME_HPP
#include "Core/ma/maTime.hpp"
#endif

#include <vector>

class tmlnChannel;
class tmlnScriptObject;

//============================================================================
//	One key: its time and value (a float: [0]; a position: x y z; a
//	colour: r g b a; an orientation: quaternion x y z w)
//============================================================================
struct cmmKey
{
	maTime m_Time;
	float m_Value[4] = { 0, 0, 0, 0 };
};

//============================================================================
//============================================================================
namespace cmmKeyUtil
{
	//--------------------------------------------------------------------
	// Adds a zero-length key driver per key, blending linearly from the
	// key before (e_Previous), or holding the key before until it begins
	// (i_bStep: e_NoBlending). Widens the timeline to show them. False,
	// with nothing added, when the channel's keys aren't float, position,
	// colour or orientation drivers.
	//--------------------------------------------------------------------
	bool AddKeys(tmlnScriptObject* io_pObject, tmlnChannel* io_pChannel, const std::vector<cmmKey>& i_Keys, bool i_bStep);
}
