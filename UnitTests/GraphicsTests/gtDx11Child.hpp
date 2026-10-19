/*****************************************************************************
**  gtDx11Child.hpp
**
**      Runs a test's DX11 half in a process of its own. After a
**      GraphicsDX11 run that loaded the font bitmap (any window does), a
**      later GraphicsDX11 run in the same process can crash (depending on
**      what ran before; the cause is in GraphicsDX11, which MachStudio
**      starts once per process). gtInDx11 runs i_Function with DX11 up in
**      a gtest death-test child and hands back the bytes it returned.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <cstdint>
#include <functional>
#include <vector>

typedef std::function<std::vector<uint8_t>()> gtDx11Function;

//	What i_Function returns with GraphicsDX11 up, run in a child process;
//	a failure there fails the test (the child's output says why)
std::vector<uint8_t> gtInDx11(const gtDx11Function& i_Function);

//	The failures the current test has had so far
int gtFailures();
