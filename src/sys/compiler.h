/*
 * $id: compiler.h                              Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __SYS_COMPILER_H__
#define __SYS_COMPILER_H__

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

typedef uint8_t      byte;         /* Exactly 8 bits, unsigned            */
typedef uint8_t      u8;           /* Exactly 8 bits, unsigned            */
typedef int8_t       s8;           /* Exactly 8 bits, signed              */
typedef uint16_t     u16;          /* Exactly 16 bits, unsigned           */
typedef int16_t      s16;          /* Exactly 16 bits, signed             */
typedef uint32_t     u32;          /* Exactly 32 bits, unsigned           */
typedef int32_t      s32;          /* Exactly 32 bits, signed             */
typedef uint64_t     u64;          /* Exactly 64 bits, unsigned           */
typedef int64_t      s64;          /* Exactly 64 bits, signed             */
typedef unsigned int uns;          /* Shorter type for unsigned int       */
typedef s64          timestamp_t;  /* Milliseconds since an unknown epoch */

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef u16 __bitwise le16;
typedef u16 __bitwise be16;
typedef u32 __bitwise le32;
typedef u32 __bitwise be32;
typedef u64 __bitwise le64;
typedef u64 __bitwise be64;

typedef u16 __bitwise __sum16;
typedef u32 __bitwise __wsum;

#if __GNUC__ >= 3
# undef  inline
# define inline         inline __attribute__ ((always_inline))
# define _noinline     __attribute__ ((noinline))
# define _pure         __attribute__ ((pure))
# define _const        __attribute__ ((const))
# define _noreturn     __attribute__ ((noreturn))
# define _malloc       __attribute__ ((malloc))
# define _must_check   __attribute__ ((warn_unused_result))
# define _deprecated   __attribute__ ((deprecated))
# define _used         __attribute__ ((used))
# define _unused       __attribute__ ((unused))
# define _packed       __attribute__ ((packed))
# define _align(x)     __attribute__ ((aligned (x)))
# define _align_max    __attribute__ ((aligned))
# define _sentinel     __attribute__ ((sentinel))
# define _format_check(x,y,z) __attribute__((format(x,y,z)))
/* branch prediction */ 
# define likely(x)      __builtin_expect (!!(x), 1)
# define unlikely(x)    __builtin_expect (!!(x), 0)
#else
# define _noinline     /* no noinline */
# define _pure         /* no pure */
# define _const        /* no const */
# define _noreturn     /* no noreturn */
# define _malloc       /* no malloc */
# define _must_check   /* no warn_unused_result */
# define _deprecated   /* no deprecated */
# define _used         /* no used */
# define _unused       /* no unused */
# define _packed       /* no packed */
# define _align(x)     /* no aligned */
# define _align_max    /* no align_max */
# define likely(x)      (x)
# define unlikely(x)    (x)
#endif

#ifndef _min
#define _min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef _max
#define _max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef _array_size
#define _array_size(a) (sizeof(a)/sizeof(*(a)))
#endif

/* Check that a pointer @x is of type @type. Fail compilation if not. **/
#define _check_ptr_type(x, type) ((x)-(type)(x) + (type)(x))             
/* Return OFFSETOF() in form of a pointer. **/
#define _ptr_to(s, i) &((s*)0)->i                                       
/* Offset of item @i from the start of structure @s */
#define _offsetof(s, i) ((uint)offsetof(s, i))                         
/* 
 * Given a pointer @p to item @i of structure @s, return a pointer to the start
 * of the struct. 
 */
#define _skip_back(s, i, p) ((s *)((char *)p - OFFSETOF(s, i)))       
/*
 * Align an integer @s to the nearest higher multiple of @a (which should be
 * a power of two) 
 */
#define _align_to(s, a) (((s)+a-1)&~(a-1))
/** Align a pointer @p to the nearest higher multiple of @s. **/
#define _align_ptr(p, s) ((uintptr_t)(p) % (s) ? \
	(typeof(p))((uintptr_t)(p) + (s) - (uintptr_t)(p) % (s)) : (p))
#define _unaligned_part(ptr, type) (((uintptr_t) (ptr)) % sizeof(type))

#define __barrier()   __asm__ __volatile__ ("" : : : "memory")

/*
 * Instruct the compiler to perform only a single access to a variable
 * (prohibits merging and refetching). The compiler is also forbidden to reorder
 * successive instances of __access_once(), but only when the compiler is aware of
 * particular ordering. Compiler ordering can be ensured, for example, by
 * putting two __access_once() in separate C statements.
 *
 * This macro does absolutely -nothing- to prevent the CPU from reordering,
 * merging, or refetching absolutely anything at any time.  Its main intended
 * use is to mediate communication between process-level code and irq/NMI
 * handlers, all running on the same CPU.
 */

#define __access_once(x) (*(__volatile__  __typeof__(x) *)&(x))
#define __is_signed_type(type) ((type) -1 < (type) 0)

/*
 * Sign-extend to long if needed, and output type is unsigned long.
 */
#define __cast_long_keep_sign(v) \
	(__is_signed_type(__typeof__(v)) ? \
	(unsigned long) (long) (v) : \
	(unsigned long) (v))

/*
 * __container_of - Get the address of an object containing a field.
 *
 * @ptr: pointer to the field.
 * @type: type of the object.
 * @member: name of the field within the object.
 */

#define __container_of(ptr, type, member) \
({ \
	(type *)((byte *)ptr - offsetof(type, member)); \
})

#define __build_bug_on_zero(cond) (sizeof(struct { int:-!!(cond); }))
#define __build_bug_on(cond) ((void)__build_bug_on_zero(cond))

#define typeof __typeof__

#endif
