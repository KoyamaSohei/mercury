/*
 * Copyright (C) 2013 Argonne National Laboratory, Department of Energy,
 *                    UChicago Argonne, LLC and The HDF Group.
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
  typedef pthread_cond_t hg_thread_cond_t;
#endif

/* Initialize the condition */
int hg_thread_cond_init(hg_thread_cond_t *cond);

/* Destroy the condition */
int hg_thread_cond_destroy(hg_thread_cond_t *cond);

/* Wake one thread waiting for the condition to change */
int hg_thread_cond_signal(hg_thread_cond_t *cond);

/* Wait for the condition to change */
int hg_thread_cond_wait(hg_thread_cond_t *cond, hg_thread_mutex_t *mutex);

/* Wait timeout (ms) for the condition to change */
int hg_thread_cond_timedwait(hg_thread_cond_t *cond, hg_thread_mutex_t *mutex, unsigned int timeout);

#endif /* MERCURY_THREAD_CONDITION_H */