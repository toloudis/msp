/*****************************************************************************
**  vkdVma.hpp
**
**      Includes Vulkan Memory Allocator configured for volk (functions are
**	fetched at run time). Include this, never vk_mem_alloc.h directly.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDVMA_HPP
#error vkdVma.hpp multiply included
#endif
#define GVK_VKDVMA_HPP

#include <volk.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vma/vk_mem_alloc.h>
