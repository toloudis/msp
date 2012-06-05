/*****************************************************************************
**  inDigitalDir.hpp
**
**      inDigitalDir defines a namespace inDigitalDir to hold an enumeration of
**		digital directions
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DIGITALDIR_HPP
#error inDigitalDir.hpp multiply included
#endif
#define IN_DIGITALDIR_HPP

namespace inDigitalDir
{

enum DigitalDir
{
	e_NONE = -1,
	e_N = 0,
	e_NE,
	e_E,
	e_SE,
	e_S,
	e_SW,
	e_W,
	e_NW,
	e_NUMDIRS,
	e_NUMDIRSANDNONE,
};

}