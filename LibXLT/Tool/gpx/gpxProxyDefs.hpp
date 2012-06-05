/*****************************************************************************
**	gpxProxyDefs.hpp
**
**	This header controls whether proxies should hold and buffer their
**	changes in a multithreaded framework, or just set their data directly.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_PROXYDEFS_HPP
#error gpxProxyDefs.hpp multiply included
#endif
#define GPX_PROXYDEFS_HPP


//============================================================================
// Comparisons on USE_PROXIES should be #if, not #ifdef.
// it's value will be either 0 or 1
//============================================================================
//#define USE_PROXIES 0
#define USE_PROXIES 1
