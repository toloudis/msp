/*****************************************************************************
**  optxBsdfProbeParams.h
**
**      The launch parameters of the BRDF probe (optxBsdfProbe.cu): one
**	launch of m_Count threads, each taking one optxBsdfQuery (a material's
**	values, a shading normal, a view and a light direction, and three
**	random numbers) and writing one optxBsdfResult: the BRDF and its pdf
**	at the light direction (optxBsdf.h's evaluate), and a direction
**	sampled with the random numbers (optxBsdf.h's sample). For tests of
**	the BRDF the programs use.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXBSDFPROBEPARAMS_H
#error optxBsdfProbeParams.h multiply included
#endif
#define GOX_OPTXBSDFPROBEPARAMS_H

namespace gox
{
	struct optxBsdfQuery
	{
		float			m_BaseColor[3];
		float			m_Metallic;
		float			m_SpecularColor[3];
		float			m_Roughness;
		float			m_Specular;
		float			m_Ior;
		float			m_U[3];			// [0, 1): lobe, then direction
		float			m_Normal[3];	// unit
		float			m_Wo[3];		// unit, towards the viewer
		float			m_Wi[3];		// unit, towards the light (evaluated)
	};

	struct optxBsdfResult
	{
		float			m_Diffuse[3];		// at m_Wi: (1 - F(n.wo)) (1 - F(n.wi)) diffuse / pi
		float			m_Pdf;				// at m_Wi: the sampling pdf
		float			m_Specular[3];		// at m_Wi: D V F
		unsigned int	m_Lobe;				// the sample's lobe (optxb::Lobe; 0: none)
		float			m_SampleWi[3];		// the sampled direction
		float			m_SamplePdf;		// its pdf
		float			m_SampleWeight[3];	// f cos / pdf
		float			m_Pad;
	};

	struct optxBsdfProbeParams
	{
		unsigned long long	m_Queries;		// device address of m_Count optxBsdfQuery
		unsigned long long	m_Results;		// device address of m_Count optxBsdfResult
		unsigned int		m_Count;
		unsigned int		m_Pad;
	};
}
