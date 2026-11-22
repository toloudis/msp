/*****************************************************************************
**  cuExternalSemaphore.hpp
**
**      cuExternalSemaphore is a Vulkan timeline semaphore imported into
**	CUDA: Wait puts a wait for a value on the OptiX stream (work queued
**	after it starts once Vulkan has signalled that value), Signal puts a
**	signal there (Vulkan work waiting for the value starts once the
**	stream's earlier work is done). The semaphore must have been created
**	exportable (VkExportSemaphoreCreateInfo, the platform handle type),
**	as vkdDevice's own timeline is with external memory.
**
**	The OptiX context must be current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CUEXTERNALSEMAPHORE_HPP
#error cuExternalSemaphore.hpp multiply included
#endif
#define GOX_CUEXTERNALSEMAPHORE_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <volk.h>

#include <cstdint>

namespace gox
{
	class cuExternalSemaphore
	{
		public:
			cuExternalSemaphore();
			~cuExternalSemaphore();
			cuExternalSemaphore(const cuExternalSemaphore&) = delete;
			cuExternalSemaphore& operator=(const cuExternalSemaphore&) = delete;

			//--------------------------------------------------------------------
			//	Import - returns false (logged) if CUDA can't import it
			//--------------------------------------------------------------------
			bool Import(VkDevice i_Device, VkSemaphore i_Semaphore);
			void Release();
			bool IsImported() const { return m_Semaphore != NULL; }

			//--------------------------------------------------------------------
			//	Wait, Signal - on optxContext's stream
			//--------------------------------------------------------------------
			bool Wait(uint64_t i_Value);
			bool Signal(uint64_t i_Value);

		private:
			CUexternalSemaphore m_Semaphore;
	};
}
