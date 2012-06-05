/****************************************************************************\
**  entFragInfoSink.hpp
**
**      entFragInfoSink is an object which will be notified about all
**	fragment information which is received during a ent import.
**	This can be used to add triangles to collision detection structures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_FRAGINFOSINK_HPP
#error entFragInfoSink.hpp multiply included
#endif
#define ENT_FRAGINFOSINK_HPP

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;


//============================================================================
// nearly empty base class for implementations to derive from
// with specific fragment data
//============================================================================
class entFragInfo
{
public:
	virtual ~entFragInfo() {};
};


//============================================================================
//============================================================================
class entFragInfoSink
{
	public:
		//--------------------------------------------------------------------
		//	i_Fragments is an array of fragments that was created for the
		//	given frag info.  It is possible the array will be empty
		//	for single skin and jointed objects.
		//--------------------------------------------------------------------
		virtual void ReceiveFragInfo( const std::vector<g3dFragment*> &i_Fragments,
									  const entFragInfo& i_FragInfo ) = 0;
};
