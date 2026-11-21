/*****************************************************************************
**  optxHostDevice.h
**
**      GOX_HD marks a function that both nvcc's device code and the host
**	compiler build (the sampling headers: optxRandom.h, optxOwen.h,
**	optxSobol.h, optxBlueNoise.h, optxPixelFilter.h, optxSampler.h), so
**	CPU tests check the code the GPU runs. Inlined everywhere: these sit in
**	the per-sample hot path.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXHOSTDEVICE_H
#error optxHostDevice.h multiply included
#endif
#define GOX_OPTXHOSTDEVICE_H

#if defined(__CUDACC__)
#define GOX_HD __host__ __device__ __forceinline__
#else
#define GOX_HD inline
#endif
