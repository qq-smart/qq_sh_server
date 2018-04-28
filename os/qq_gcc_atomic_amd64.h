/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#if (QQ_SMP)
#define QQ_SMP_LOCK  "lock;"
#else
#define QQ_SMP_LOCK
#endif


/*
 * "cmpxchgq  r, [m]":
 *
 *     if (rax == [m]) {
 *         zf = 1;
 *         [m] = r;
 *     } else {
 *         zf = 0;
 *         rax = [m];
 *     }
 *
 *
 * The "r" is any register, %rax (%r0) - %r16.
 * The "=a" and "a" are the %rax register.
 * Although we can return result in any register, we use "a" because it is
 * used in cmpxchgq anyway.  The result is actually in %al but not in $rax,
 * however as the code is inlined gcc can test %al as well as %rax.
 *
 * The "cc" means that flags were changed.
 */

static inline qq_atomic_uint_t
qq_atomic_cmp_set(qq_atomic_t *lock, qq_atomic_uint_t old,
    qq_atomic_uint_t set)
{
    u_char  res;

    __asm__ volatile (

         QQ_SMP_LOCK
    "    cmpxchgq  %3, %1;   "
    "    sete      %0;       "

    : "=a" (res) : "m" (*lock), "a" (old), "r" (set) : "cc", "memory");

    return res;
}


/*
 * "xaddq  r, [m]":
 *
 *     temp = [m];
 *     [m] += r;
 *     r = temp;
 *
 *
 * The "+r" is any register, %rax (%r0) - %r16.
 * The "cc" means that flags were changed.
 */

static inline qq_atomic_int_t
qq_atomic_fetch_add(qq_atomic_t *value, qq_atomic_int_t add)
{
    __asm__ volatile (

         QQ_SMP_LOCK
    "    xaddq  %0, %1;   "

    : "+r" (add) : "m" (*value) : "cc", "memory");

    return add;
}


#define qq_memory_barrier()    __asm__ volatile ("" ::: "memory")

#define qq_cpu_pause()         __asm__ ("pause")
