#pragma once

// Compiler detection
#if defined(_MSC_VER)
#define _NV_COMPILER_MSVC 1
#else
#define _NV_COMPILER_MSVC 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define _NV_COMPILER_GCC 1
#else
#define _NV_COMPILER_GCC 0
#endif

#if defined(__clang__)
#define _NV_COMPILER_CLANG 1
#else
#define _NV_COMPILER_CLANG 0
#endif

// C++ Standard detection
#if defined(_MSVC_LANG)
#define _NV_CPP_STD _MSVC_LANG
#else
#define _NV_CPP_STD __cplusplus
#endif

#define _NV_HAS_CXX11 (_NV_CPP_STD >= 201103L)
#define _NV_HAS_CXX14 (_NV_CPP_STD >= 201402L)
#define _NV_HAS_CXX17 (_NV_CPP_STD >= 201703L)
#define _NV_HAS_CXX20 (_NV_CPP_STD >= 202002L)
#define _NV_HAS_CXX23 (_NV_CPP_STD >= 202302L)

// Attributes
#if _NV_COMPILER_MSVC
#define _NV_ALWAYS_INLINE __forceinline
#elif _NV_COMPILER_GCC || _NV_COMPILER_CLANG
#define _NV_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define _NV_ALWAYS_INLINE inline
#endif

#if _NV_HAS_CXX17
#define _NV_NODISCARD [[nodiscard]]
#else
#define _NV_NODISCARD
#endif
