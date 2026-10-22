/*****************************************************************************
**  vkdOcclusionQuery.hpp
**
**      vkdOcclusionQuery is D3D11's occlusion query (D3D11_QUERY_OCCLUSION)
**	for ported code: it counts the samples that pass the depth and stencil
**	tests between Begin and End. Depth-peeled transparency uses it to stop
**	peeling when a peel draws nothing.
**
**	- One query in a pool of its own, reset on the host (hostQueryReset)
**	  at each Begin; precise (occlusionQueryPrecise), so the count is
**	  exact, as D3D11's is.
**	- Begin and End go into the context's command stream between rendering
**	  scopes (the context ends the current one), so the count covers every
**	  draw between them, whatever targets they bind: D3D11's query spans
**	  target changes too.
**	- GetResult submits the context's work and waits for it: the stall
**	  DX11's GetData loop has. Read the result before the next Begin.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDOCCLUSIONQUERY_HPP
#error vkdOcclusionQuery.hpp multiply included
#endif
#define GVK_VKDOCCLUSIONQUERY_HPP

#include <volk.h>

#include <cstdint>

namespace gvk
{
	class vkdContext;
	class vkdDevice;

	class vkdOcclusionQuery
	{
		public:
			explicit vkdOcclusionQuery(vkdDevice& i_Device);
			~vkdOcclusionQuery();

			void Begin(vkdContext& i_Context);
			void End(vkdContext& i_Context);

			//	Submits, waits, and returns the samples that passed
			uint64_t GetResult(vkdContext& i_Context);

		private:
			vkdOcclusionQuery(const vkdOcclusionQuery&) = delete;
			vkdOcclusionQuery& operator = (const vkdOcclusionQuery&) = delete;

			vkdDevice&	m_Device;
			VkQueryPool	m_Pool = VK_NULL_HANDLE;
			uint64_t	m_Value = 0;			// timeline value of the submission holding End
			bool		m_bActive = false;		// between Begin and End
			bool		m_bPending = false;		// ended, result not read yet
	};
}
