/*****************************************************************************
**  optxRayQueryParams.h
**
**      The launch parameters of the ray query program (optxRayQuery.cu),
**	shared by the device program and the host code that launches it. The
**	program traces one ray per launch index against a traversable (an IAS
**	or a GAS) and writes where it hit: the distance, the instance ID and
**	the triangle. It is the building block of picking and of the BVH tests.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXRAYQUERYPARAMS_H
#error optxRayQueryParams.h multiply included
#endif
#define GOX_OPTXRAYQUERYPARAMS_H

#include <optix_types.h>

namespace gox
{
	//	One ray. Only geometry whose instance visibility mask shares a bit
	//	with m_Mask is seen.
	struct optxRayQueryRay
	{
		float			m_Origin[3];
		float			m_TMin;
		float			m_Direction[3];
		float			m_TMax;
		unsigned int	m_Mask;
		unsigned int	m_Pad[3];
	};

	//	What a ray hit; m_Hit is 0 for a miss (the rest is then undefined)
	struct optxRayQueryHit
	{
		float			m_T;
		unsigned int	m_InstanceId;
		unsigned int	m_PrimitiveIndex;
		unsigned int	m_Hit;
	};

	//	The launch is m_Count x 1 x 1
	struct optxRayQueryParams
	{
		OptixTraversableHandle		m_Handle;
		const optxRayQueryRay*		m_pRays;
		optxRayQueryHit*			m_pHits;
		unsigned int				m_Count;
	};
}
