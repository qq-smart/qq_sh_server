/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_ATOMIC_H_INCLUDED_
#define _QQ_ATOMIC_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#if (QQ_HAVE_LIBATOMIC)

    #define AO_REQUIRE_CAS
    #include <atomic_ops.h>

    #define QQ_HAVE_ATOMIC_OPS  1

    typedef long                        qq_atomic_int_t;
    typedef AO_t                        qq_atomic_uint_t;
    typedef volatile qq_atomic_uint_t   qq_atomic_t;

    #if (QQ_PTR_SIZE == 8)
        #define QQ_ATOMIC_T_LEN             (sizeof("-9223372036854775808") - 1)
    #else
        #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)
    #endif

    #define qq_atomic_cmp_set(lock, old, new) \
        AO_compare_and_swap(lock, old, new)
    #define qq_atomic_fetch_add(value, add) \
        AO_fetch_and_add(value, add)
    #define qq_memory_barrier()        AO_nop()
    #define qq_cpu_pause()


#elif (QQ_DARWIN_ATOMIC)

    /*
     * use Darwin 8 atomic(3) and barrier(3) operations
     * optimized at run-time for UP and SMP
     */

    #include <libkern/OSAtomic.h>

    /* "bool" conflicts with perl's CORE/handy.h */
    #if 0
        #undef bool
    #endif


    #define QQ_HAVE_ATOMIC_OPS  1

    #if (QQ_PTR_SIZE == 8)
        typedef int64_t                     qq_atomic_int_t;
        typedef uint64_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-9223372036854775808") - 1)

        #define qq_atomic_cmp_set(lock, old, new) \
            OSAtomicCompareAndSwap64Barrier(old, new, (int64_t *) lock)

        #define qq_atomic_fetch_add(value, add) \
            (OSAtomicAdd64(add, (int64_t *) value) - add)

    #else
        typedef int32_t                     qq_atomic_int_t;
        typedef uint32_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)

        #define qq_atomic_cmp_set(lock, old, new) \
            OSAtomicCompareAndSwap32Barrier(old, new, (int32_t *) lock)

        #define qq_atomic_fetch_add(value, add) \
            (OSAtomicAdd32(add, (int32_t *) value) - add)

    #endif

    #define qq_memory_barrier()        OSMemoryBarrier()
    #define qq_cpu_pause()

    typedef volatile qq_atomic_uint_t  qq_atomic_t;


#elif (QQ_HAVE_GCC_ATOMIC)

    /* GCC 4.1 builtin atomic operations */

    #define QQ_HAVE_ATOMIC_OPS  1

    typedef long                        qq_atomic_int_t;
    typedef unsigned long               qq_atomic_uint_t;

    #if (QQ_PTR_SIZE == 8)
        #define QQ_ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)
    #else
        #define QQ_ATOMIC_T_LEN            (sizeof("-2147483648") - 1)
    #endif

    typedef volatile qq_atomic_uint_t  qq_atomic_t;


    #define qq_atomic_cmp_set(lock, old, set) \
        __sync_bool_compare_and_swap(lock, old, set)

    #define qq_atomic_fetch_add(value, add) \
        __sync_fetch_and_add(value, add)

    #define qq_memory_barrier()        __sync_synchronize()

    #if ( __i386__ || __i386 || __amd64__ || __amd64 )
        #define qq_cpu_pause()             __asm__ ("pause")
    #else
        #define qq_cpu_pause()
    #endif


#elif ( __i386__ || __i386 )

    typedef int32_t                     qq_atomic_int_t;
    typedef uint32_t                    qq_atomic_uint_t;
    typedef volatile qq_atomic_uint_t   qq_atomic_t;
    #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)


    #if ( __SUNPRO_C )

        #define QQ_HAVE_ATOMIC_OPS  1

        qq_atomic_uint_t
        qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
            qq_atomic_uint_t set);

        qq_atomic_int_t
        qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add);

        /*
         * Sun Studio 12 exits with segmentation fault on '__asm ("pause")',
         * so qq_cpu_pause is declared in src/os/qq_sunpro_x86.il
         */

        void
        qq_cpu_pause(void);
        
        /* the code in src/os/qq_sunpro_x86.il */

        #define qq_memory_barrier()        __asm (".volatile"); __asm (".nonvolatile")


    #else /* ( __GNUC__ || __INTEL_COMPILER ) */

        #define QQ_HAVE_ATOMIC_OPS  1

        #include "qq_gcc_atomic_x86.h"

    #endif


#elif ( __amd64__ || __amd64 )

    typedef int64_t                     qq_atomic_int_t;
    typedef uint64_t                    qq_atomic_uint_t;
    typedef volatile qq_atomic_uint_t   qq_atomic_t;
    #define QQ_ATOMIC_T_LEN             (sizeof("-9223372036854775808") - 1)


    #if ( __SUNPRO_C )

        #define QQ_HAVE_ATOMIC_OPS  1

        qq_atomic_uint_t
        qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
            qq_atomic_uint_t set);

        qq_atomic_int_t
            qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add);

        /*
         * Sun Studio 12 exits with segmentation fault on '__asm ("pause")',
         * so qq_cpu_pause is declared in src/os/qq_sunpro_amd64.il
         */

        void
        qq_cpu_pause(void);

        /* the code in src/os/qq_sunpro_amd64.il */

        #define qq_memory_barrier()        __asm (".volatile"); __asm (".nonvolatile")


    #else /* ( __GNUC__ || __INTEL_COMPILER ) */

        #define QQ_HAVE_ATOMIC_OPS  1

        #include "qq_gcc_atomic_amd64.h"

    #endif


#elif ( __sparc__ || __sparc || __sparcv9 )

    #if (QQ_PTR_SIZE == 8)

        typedef int64_t                     qq_atomic_int_t;
        typedef uint64_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-9223372036854775808") - 1)

    #else

        typedef int32_t                     qq_atomic_int_t;
        typedef uint32_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)

    #endif

    typedef volatile qq_atomic_uint_t  qq_atomic_t;


    #if ( __SUNPRO_C )

        #define QQ_HAVE_ATOMIC_OPS  1

        #include "qq_sunpro_atomic_sparc64.h"


    #else /* ( __GNUC__ || __INTEL_COMPILER ) */

        #define QQ_HAVE_ATOMIC_OPS  1

        #include "qq_gcc_atomic_sparc64.h"

    #endif


#elif ( __powerpc__ || __POWERPC__ )

    #define QQ_HAVE_ATOMIC_OPS  1

    #if (QQ_PTR_SIZE == 8)

        typedef int64_t                     qq_atomic_int_t;
        typedef uint64_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-9223372036854775808") - 1)

    #else

        typedef int32_t                     qq_atomic_int_t;
        typedef uint32_t                    qq_atomic_uint_t;
        #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)

    #endif

    typedef volatile qq_atomic_uint_t   qq_atomic_t;


    #include "qq_gcc_atomic_ppc.h"

#endif


#if !(QQ_HAVE_ATOMIC_OPS)

    #define QQ_HAVE_ATOMIC_OPS  0

    typedef int32_t                     qq_atomic_int_t;
    typedef uint32_t                    qq_atomic_uint_t;
    typedef volatile qq_atomic_uint_t   qq_atomic_t;
    #define QQ_ATOMIC_T_LEN             (sizeof("-2147483648") - 1)


    static inline qq_atomic_uint_t
    qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
        qq_atomic_uint_t set)
    {
        if (*lock == old) {
            *lock = set;
            return 1;
        }

        return 0;
    }


    static inline qq_atomic_int_t
    qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add)
    {
        qq_atomic_int_t  old;

        old = *value;
        *value += add;

        return old;
    }

    #define qq_memory_barrier()
    #define qq_cpu_pause()

#endif


void qq_spinlock(qq_atomic_t *lock, qq_atomic_int_t value, qq_uint_t spin);

#define qq_trylock(lock)  (*(lock) == 0 && qq_atomic_cmp_set(lock, 0, 1))
#define qq_unlock(lock)    *(lock) = 0


#endif /* _QQ_ATOMIC_H_INCLUDED_ */
