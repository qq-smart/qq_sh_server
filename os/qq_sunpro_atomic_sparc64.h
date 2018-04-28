/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#if (QQ_PTR_SIZE == 4)
#define QQ_CASA  qq_casa
#else
#define QQ_CASA  qq_casxa
#endif


qq_atomic_uint_t
qq_casa(qq_atomic_uint_t set, qq_atomic_uint_t old, qq_atomic_t *lock);

qq_atomic_uint_t
qq_casxa(qq_atomic_uint_t set, qq_atomic_uint_t old, qq_atomic_t *lock);

/* the code in src/os/qq_sunpro_sparc64.il */


static inline qq_atomic_uint_t
qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
    qq_atomic_uint_t set)
{
    set = QQ_CASA(set, old, lock);

    return (set == old);
}


static inline qq_atomic_int_t
qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add)
{
    qq_atomic_uint_t  old, res;

    old = *value;

    for ( ;; ) {

        res = old + add;

        res = QQ_CASA(res, old, value);

        if (res == old) {
            return res;
        }

        old = res;
    }
}


#define qq_memory_barrier()                                                  \
        __asm (".volatile");                                                  \
        __asm ("membar #LoadLoad | #LoadStore | #StoreStore | #StoreLoad");   \
        __asm (".nonvolatile")

#define qq_cpu_pause()
