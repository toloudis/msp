#pragma once

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

class testCameraScenePrty : public prtyObject
{
public:
	testCameraScenePrty(void);
	virtual ~testCameraScenePrty(void);

	prtyColor m_Color;
};
