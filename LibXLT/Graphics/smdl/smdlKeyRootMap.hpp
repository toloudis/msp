/****************************************************************************\
**	smdlKeyRootMap.hpp
**
**		smdlKeyRootMap is a typedef for a mapping from a name to
**	a root of an animation tree.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_KEYROOTMAP_HPP
#error smdlKeyRootMap.hpp multiply included
#endif
#define SMDL_KEYROOTMAP_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef SMDL_GEOANIMKEYS_HPP
#include "Graphics/smdl/smdlGeoAnimKeys.hpp"
#endif
#ifndef SMDL_TREE_HPP
#include "Graphics/smdl/smdlTree.hpp"
#endif

//#include <map>


//----------------------------------------------------------------------------
// This map is a mouthful, so a typedef is useful
//----------------------------------------------------------------------------
//typedef std::map<std::string, smdlTree<smdlGeoAnimKeys> > smdlKeyRootMap;
typedef std::map<std::string, shared_ptr< smdlTree<smdlGeoAnimKeys> > > smdlKeyRootMap;
