/*
 * Copyright (C) 2013-2019 Argonne National Laboratory, Department of Energy,
 *                    UChicago Argonne, LLC and The HDF Group.
 * All rights reserved.
 *
 * The full copyright notice, including terms governing use, modification,
 * and redistribution, is contained in the COPYING file that can be
 * found at the root of the source code distribution tree.
 */

#ifndef MERCURY_THREAD_CONDITION_H
#define MERCURY_THREAD_CONDITION_H

#include "mercury_thread_mutex.h"

#ifdef _WIN32
typedef CONDITION_VARIABLE hg_thread_cond_t;
#else
#    if defined(HG_UTIL_HAS_PTHREAD_CONDATTR_SETCLOCK) &&                      \
        defined(HG_UTIL_HAS_TIME_H) && defined(HG_UTIL_HAS_CLOCK_GETTIME)
#        include "mercury_time.h"
#    elif defined(HG_UTIL_HAS_SYSTIME_H)
#        include <sys/time.h>
#    endif
#    include <stdlib.h>
typedef pthread_cond_t hg_thread_cond_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the condition.
 *
 * \param cond [IN/OUT]         pointer to condition object
 *
 * \return Non-negative on success or negative on failure
 */
HG_UTIL_PUBLIC int
hg_thread_cond_init(hg_thread_cond_t *cond);

/**
 * Destroy the condition.
 *
 * \param cond [IN/OUT]         pointer to condition object
 *
 * \return Non-negative on success or negative on failure
 */
HG_UTIL_PUBLIC int
hg_thread_cond_destroy(hg_thread_cond_t *cond);

/**
 * Wake one thread waiting for the condition to change.
 *
 * \param cond [IN/OUT]         pointer to condition object
 *
 * \return Non-negative on success or negative on failure
 */
static HG_UTIL_INLINE int
hg_thread_cond_signal(hg_thread_cond_t *cond);

/**
 * Wake all the threads waiting for the condition to change.
 *
 * \param cond [IN/OUT]         pointer to condition object
 *
 * \return Non-negative on success or negative on failure
 */
static HG_UTIL_INLINE int
hg_thread_cond_broadcast(hg_thread_cond_t *cond);

/**
 * Wait for the condition to change.
 *
 * \param cond [IN/OUT]         pointer to condition object
 * \param mutex [IN/OUT]        pointer to mutex object
 *
 * \return Non-negative on success or negative on failure
 */
static HG_UTIL_INLINE int
hg_thread_cond_wait(hg_thread_cond_t *cond, hg_thread_mutex_t *mutex);

/**
 * Wait timeout ms for the condition to change.
 *
 * \param cond [IN/OUT]         pointer to condition object
 * \param mutex [IN/OUT]        pointer to mutex object
 * \param timeout [IN]          timeout (in milliseconds)
 *
 * \return Non-negative on success or negative on failure
 */
static HG_UTIL_INLINE int
hg_thread_cond_timedwait(
    hg_thread_cond_t *cond, hg_thread_mutex_t *mutex, unsigned int timeout);

/*---------------------------------------------------------------------------*/
static HG_UTIL_INLINE int
hg_thread_cond_signal(hg_thread_cond_t *cond)
{
#ifdef _WIN32
    WakeConditionVariable(cond);
#else
    if (pthread_cond_signal(cond))
        return HG_UTIL_FAIL;
#endif

    return HG_UTIL_SUCCESS;
}

/*---------------------------------------------------------------------------*/
static HG_UTIL_INLINE int
hg_thread_cond_broadcast(hg_thread_cond_t *cond)
{
#ifdef _WIN32
    WakeAllConditionVariable(cond);
#else
    if (pthread_cond_broadcast(cond))
        return HG_UTIL_FAIL;
#endif

    return HG_UTIL_SUCCESS;
}

/*---------------------------------------------------------------------------*/
static HG_UTIL_INLINE int
hg_thread_cond_wait(hg_thread_cond_t *cond, hg_thread_mutex_t *mutex)
{
#ifdef _WIN32
    if (!SleepConditionVariableCS(cond, mutex, INFINITE))
        return HG_UTIL_FAIL;
#else
    if (pthread_cond_wait(cond, mutex))
        return HG_UTIL_FAIL;
#endif

    return HG_UTIL_SUCCESS;
}

/*---------------------------------------------------------------------------*/
static HG_UTIL_INLINE int
hg_thread_cond_timedwait(
    hg_thread_cond_t *cond, hg_thread_mutex_t *mutex, unsigned int timeout)
{
#ifdef _WIN32
    if (!SleepConditionVariableCS(cond, mutex, timeout))
        return HG_UTIL_FAIL;
#else
#    if defined(HG_UTIL_HAS_PTHREAD_CONDATTR_SETCLOCK) &&                      \
        defined(HG_UTIL_HAS_TIME_H) && defined(HG_UTIL_HAS_CLOCK_GETTIME)
    hg_time_t now;
#    else
    struct timeval now;
#    endif
    struct timespec abs_timeout;
    ldiv_t ld;

    /* Need to convert timeout (ms) to absolute time */
#    if defined(HG_UTIL_HAS_PTHREAD_CONDATTR_SETCLOCK) &&                      \
        defined(HG_UTIL_HAS_TIME_H) && defined(HG_UTIL_HAS_CLOCK_GETTIME)
    hg_time_get_current_ms(&now);

    /* Get sec / nsec */
    ld = ldiv(now.tv_nsec + timeout * 1000000L, 1000000L);
#    elif defined(HG_UTIL_HAS_SYSTIME_H)
    gettimeofday(&now, NULL);

    /* Get sec / nsec */
    ld = ldiv(now.tv_usec + timeout * 1000L, 1000000L);
#    endif
    abs_timeout.tv_sec = now.tv_sec + ld.quot;
    abs_timeout.tv_nsec = ld.rem * 1000L;

    if (pthread_cond_timedwait(cond, mutex, &abs_timeout))
        return HG_UTIL_FAIL;
#endif

    return HG_UTIL_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* MERCURY_THREAD_CONDITION_H */
