//===-- sanitizer_internal_defs.h -------------------------------*- C++ -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is shared between AddressSanitizer and ThreadSanitizer.
// It contains macro used in run-time libraries code.
//===----------------------------------------------------------------------===//
#ifndef SANITIZER_DEFS_H
#define SANITIZER_DEFS_H

#include "sanitizer_platform.h"

#ifndef SANITIZER_DEBUG
# define SANITIZER_DEBUG 0
#endif

// Only use SANITIZER_*ATTRIBUTE* before the function return type!
#if SANITIZER_WINDOWS
# define SANITIZER_INTERFACE_ATTRIBUTE __declspec(dllexport)
// FIXME find out what we need on Windows, if anything.
# define SANITIZER_WEAK_ATTRIBUTE
#elif SANITIZER_GO
# define SANITIZER_INTERFACE_ATTRIBUTE
# define SANITIZER_WEAK_ATTRIBUTE
#else
# define SANITIZER_INTERFACE_ATTRIBUTE __attribute__((visibility("default")))
# define SANITIZER_WEAK_ATTRIBUTE  __attribute__((weak))
#endif

#if (SANITIZER_LINUX || SANITIZER_WINDOWS) && !SANITIZER_GO
# define SANITIZER_SUPPORTS_WEAK_HOOKS 1
#else
# define SANITIZER_SUPPORTS_WEAK_HOOKS 0
#endif

// We can use .preinit_array section on Linux to call sanitizer initialization
// functions very early in the process startup (unless PIC macro is defined).
// FIXME: do we have anything like this on Mac?
#if SANITIZER_LINUX && !SANITIZER_ANDROID && !defined(PIC)
# define SANITIZER_CAN_USE_PREINIT_ARRAY 1
#else
# define SANITIZER_CAN_USE_PREINIT_ARRAY 0
#endif

// GCC does not understand __has_feature
#if !defined(__has_feature)
# define __has_feature(x) 0
#endif

// For portability reasons we do not include stddef.h, stdint.h or any other
// system header, but we do need some basic types that are not defined
// in a portable way by the language itself.
namespace __sanitizer {

#if defined(_WIN64)
// 64-bit Windows uses LLP64 data model.
typedef unsigned long long uptr;  // NOLINT
typedef signed   long long sptr;  // NOLINT
#else
typedef unsigned long uptr;  // NOLINT
typedef signed   long sptr;  // NOLINT
#endif  // defined(_WIN64)
#if defined(__x86_64__)
// Since x32 uses ILP32 data model in 64-bit hardware mode, we must use
// 64-bit pointer to unwind stack frame.
typedef unsigned long long uhwptr;  // NOLINT
#else
typedef uptr uhwptr;   // NOLINT
#endif
typedef unsigned char u8;
typedef unsigned short u16;  // NOLINT
typedef unsigned int u32;
typedef unsigned long long u64;  // NOLINT
typedef signed   char s8;
typedef signed   short s16;  // NOLINT
typedef signed   int s32;
typedef signed   long long s64;  // NOLINT
#if SANITIZER_WINDOWS
// On Windows, files are HANDLE, which is a synonim of void*.
// Use void* to avoid including <windows.h> everywhere.
typedef void* fd_t;
typedef unsigned error_t;
#else
typedef int fd_t;
typedef int error_t;
#endif
typedef int pid_t;

// WARNING: OFF_T may be different from OS type off_t, depending on the value of
// _FILE_OFFSET_BITS. This definition of OFF_T matches the ABI of system calls
// like pread and mmap, as opposed to pread64 and mmap64.
// FreeBSD, Mac and Linux/x86-64 are special.
#if SANITIZER_FREEBSD || SANITIZER_MAC || \
  (SANITIZER_LINUX && defined(__x86_64__))
typedef u64 OFF_T;
#else
typedef uptr OFF_T;
#endif
typedef u64  OFF64_T;

#if (SANITIZER_WORDSIZE == 64) || SANITIZER_MAC
typedef uptr operator_new_size_type;
#else
# if defined(__s390__) && !defined(__s390x__)
// Special case: 31-bit s390 has unsigned long as size_t.
typedef unsigned long operator_new_size_type;
# else
typedef u32 operator_new_size_type;
# endif
#endif


// ----------- ATTENTION -------------
// This header should NOT include any other headers to avoid portability issues.

// Common defs.
#define INLINE inline
#define INTERFACE_ATTRIBUTE SANITIZER_INTERFACE_ATTRIBUTE
#define SANITIZER_WEAK_DEFAULT_IMPL \
  extern "C" SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE NOINLINE
#define SANITIZER_WEAK_CXX_DEFAULT_IMPL \
  extern "C++" SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE NOINLINE

// Platform-specific defs.
#if defined(_MSC_VER)
# define ALWAYS_INLINE __forceinline
// FIXME(timurrrr): do we need this on Windows?
# define ALIAS(x)
# define ALIGNED(x) __declspec(align(x))
# define FORMAT(f, a)
# define NOINLINE __declspec(noinline)
# define NORETURN __declspec(noreturn)
# define THREADLOCAL   __declspec(thread)
# define LIKELY(x) (x)
# define UNLIKELY(x) (x)
# define PREFETCH(x) /* _mm_prefetch(x, _MM_HINT_NTA) */ (void)0
#else  // _MSC_VER
# define ALWAYS_INLINE inline __attribute__((always_inline))
# define ALIAS(x) __attribute__((alias(x)))
// Please only use the ALIGNED macro before the type.
// Using ALIGNED after the variable declaration is not portable!
# define ALIGNED(x) __attribute__((aligned(x)))
# define FORMAT(f, a)  __attribute__((format(printf, f, a)))
# define NOINLINE __attribute__((noinline))
# define NORETURN  __attribute__((noreturn))
# define THREADLOCAL   __thread
# define LIKELY(x)     __builtin_expect(!!(x), 1)
# define UNLIKELY(x)   __builtin_expect(!!(x), 0)
# if defined(__i386__) || defined(__x86_64__)
// __builtin_prefetch(x) generates prefetchnt0 on x86
#  define PREFETCH(x) __asm__("prefetchnta (%0)" : : "r" (x))
# else
#  define PREFETCH(x) __builtin_prefetch(x)
# endif
#endif  // _MSC_VER

#if !defined(_MSC_VER) || defined(__clang__)
# define UNUSED __attribute__((unused))
# define USED __attribute__((used))
#else
# define UNUSED
# define USED
#endif

#if !defined(_MSC_VER) || defined(__clang__) || MSC_PREREQ(1900)
# define NOEXCEPT noexcept
#else
# define NOEXCEPT throw()
#endif

// Unaligned versions of basic types.
typedef ALIGNED(1) u16 uu16;
typedef ALIGNED(1) u32 uu32;
typedef ALIGNED(1) u64 uu64;
typedef ALIGNED(1) s16 us16;
typedef ALIGNED(1) s32 us32;
typedef ALIGNED(1) s64 us64;

#if SANITIZER_WINDOWS
}  // namespace __sanitizer
typedef unsigned long DWORD;  // NOLINT
namespace __sanitizer {
typedef DWORD thread_return_t;
# define THREAD_CALLING_CONV __stdcall
#else  // _WIN32
typedef void* thread_return_t;
# define THREAD_CALLING_CONV
#endif  // _WIN32
typedef thread_return_t (THREAD_CALLING_CONV *thread_callback_t)(void* arg);

// NOTE: Functions below must be defined in each run-time.
void NORETURN Die();

// FIXME: No, this shouldn't be in the sanitizer interface.
SANITIZER_INTERFACE_ATTRIBUTE
void NORETURN CheckFailed(const char *file, int line, const char *cond,
                          u64 v1, u64 v2);

// Check macro
#define RAW_CHECK_MSG(expr, msg) do { \
  if (UNLIKELY(!(expr))) { \
    RawWrite(msg); \
    Die(); \
  } \
} while (0)

#define RAW_CHECK(expr) RAW_CHECK_MSG(expr, #expr)

#define CHECK_IMPL(c1, op, c2) \
  do { \
    __sanitizer::u64 v1 = (u64)(c1); \
    __sanitizer::u64 v2 = (u64)(c2); \
    if (UNLIKELY(!(v1 op v2))) \
      __sanitizer::CheckFailed(__FILE__, __LINE__, \
        "(" #c1 ") " #op " (" #c2 ")", v1, v2); \
  } while (false) \
/**/

#define CHECK(a)       CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b) CHECK_IMPL((a), ==, (b))
#define CHECK_NE(a, b) CHECK_IMPL((a), !=, (b))
#define CHECK_LT(a, b) CHECK_IMPL((a), <,  (b))
#define CHECK_LE(a, b) CHECK_IMPL((a), <=, (b))
#define CHECK_GT(a, b) CHECK_IMPL((a), >,  (b))
#define CHECK_GE(a, b) CHECK_IMPL((a), >=, (b))

#if SANITIZER_DEBUG
#define DCHECK(a)       CHECK(a)
#define DCHECK_EQ(a, b) CHECK_EQ(a, b)
#define DCHECK_NE(a, b) CHECK_NE(a, b)
#define DCHECK_LT(a, b) CHECK_LT(a, b)
#define DCHECK_LE(a, b) CHECK_LE(a, b)
#define DCHECK_GT(a, b) CHECK_GT(a, b)
#define DCHECK_GE(a, b) CHECK_GE(a, b)
#else
#define DCHECK(a)
#define DCHECK_EQ(a, b)
#define DCHECK_NE(a, b)
#define DCHECK_LT(a, b)
#define DCHECK_LE(a, b)
#define DCHECK_GT(a, b)
#define DCHECK_GE(a, b)
#endif

#define UNREACHABLE(msg) do { \
  CHECK(0 && msg); \
  Die(); \
} while (0)

#define UNIMPLEMENTED() UNREACHABLE("unimplemented")

#define COMPILER_CHECK(pred) IMPL_COMPILER_ASSERT(pred, __LINE__)

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

#define IMPL_PASTE(a, b) a##b
#define IMPL_COMPILER_ASSERT(pred, line) \
    typedef char IMPL_PASTE(assertion_failed_##_, line)[2*(int)(pred)-1]

// Limits for integral types. We have to redefine it in case we don't
// have stdint.h (like in Visual Studio 9).
#undef __INT64_C
#undef __UINT64_C
#if SANITIZER_WORDSIZE == 64
# define __INT64_C(c)  c ## L
# define __UINT64_C(c) c ## UL
#else
# define __INT64_C(c)  c ## LL
# define __UINT64_C(c) c ## ULL
#endif  // SANITIZER_WORDSIZE == 64
#undef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#undef INT32_MAX
#define INT32_MAX              (2147483647)
#undef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#undef INT64_MIN
#define INT64_MIN              (-__INT64_C(9223372036854775807)-1)
#undef INT64_MAX
#define INT64_MAX              (__INT64_C(9223372036854775807))
#undef UINT64_MAX
#define UINT64_MAX             (__UINT64_C(18446744073709551615))

enum LinkerInitialized { LINKER_INITIALIZED = 0 };

#if !defined(_MSC_VER) || defined(__clang__)
# define GET_CALLER_PC() (uptr)__builtin_return_address(0)
# define GET_CURRENT_FRAME() (uptr)__builtin_frame_address(0)
inline void Trap() {
  __builtin_trap();
}
#else
extern "C" void* _ReturnAddress(void);
extern "C" void* _AddressOfReturnAddress(void);
# pragma intrinsic(_ReturnAddress)
# pragma intrinsic(_AddressOfReturnAddress)
# define GET_CALLER_PC() (uptr)_ReturnAddress()
// CaptureStackBackTrace doesn't need to know BP on Windows.
# define GET_CURRENT_FRAME() (((uptr)_AddressOfReturnAddress()) + sizeof(uptr))

extern "C" void __ud2(void);
# pragma intrinsic(__ud2)
inline void Trap() {
  __ud2();
}
#endif

#define HANDLE_EINTR(res, f)                                       \
  {                                                                \
    int rverrno;                                                   \
    do {                                                           \
      res = (f);                                                   \
    } while (internal_iserror(res, &rverrno) && rverrno == EINTR); \
  }

// Forces the compiler to generate a frame pointer in the function.
#define ENABLE_FRAME_POINTER                                       \
  do {                                                             \
    volatile uptr enable_fp;                                       \
    enable_fp = GET_CURRENT_FRAME();                               \
    (void)enable_fp;                                               \
  } while (0)

}  // namespace __sanitizer

namespace __asan  { using namespace __sanitizer; }  // NOLINT
namespace __dsan  { using namespace __sanitizer; }  // NOLINT
namespace __dfsan { using namespace __sanitizer; }  // NOLINT
namespace __esan  { using namespace __sanitizer; }  // NOLINT
namespace __lsan  { using namespace __sanitizer; }  // NOLINT
namespace __msan  { using namespace __sanitizer; }  // NOLINT
namespace __tsan  { using namespace __sanitizer; }  // NOLINT
namespace __scudo { using namespace __sanitizer; }  // NOLINT
namespace __ubsan { using namespace __sanitizer; }  // NOLINT
namespace __xray  { using namespace __sanitizer; }  // NOLINT
namespace __interception  { using namespace __sanitizer; }  // NOLINT


#endif  // SANITIZER_DEFS_H
