/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


/*
 * "casa   [r1] 0x80, r2, r0"  and
 * "casxa  [r1] 0x80, r2, r0"  do the following:
 *
 *     if ([r1] == r2) {
 *         swap(r0, [r1]);
 *     } else {
 *         r0 = [r1];
 *     }
 *
 * so "r0 == r2" means that the operation was successful.
 *
 *
 * The "r" means the general register.
 * The "+r" means the general register used for both input and output.
 */


#if (QQ_PTR_SIZE == 4)
#define QQ_CASA  "casa"
#else
#define QQ_CASA  "casxa"
#endif


static inline qq_atomic_uint_t
qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
    qq_atomic_uint_t set)
{
    __asm__ volatile (

    QQ_CASA " [%1] 0x80, %2, %0"

    : "+r" (set) : "r" (lock), "r" (old) : "memory");

    return (set == old);
}


static inline qq_atomic_int_t
qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add)
{
    qq_atomic_uint_t  old, res;

    old = *value;

    for ( ;; ) {

        res = old + add;

        __asm__ volatile (

        QQ_CASA " [%1] 0x80, %2, %0"

        : "+r" (res) : "r" (value), "r" (old) : "memory");

        if (res == old) {
            return res;
        }

        old = res;
    }
}


#if (QQ_SMP)
#define qq_memory_barrier()                                                  \
            __asm__ volatile (                                                \
            "membar #LoadLoad | #LoadStore | #StoreStore | #StoreLoad"        \
            ::: "memory")
#else
#define qq_memory_barrier()   __asm__ volatile ("" ::: "memory")
#endif

#define qq_cpu_pause()
