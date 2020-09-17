#ifndef MNEMO_H
#define MNEMO_H 1

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <unistd.h>

#include "mnemo/utils/version.h"


////////////////////////////////////////////////////////////////////////////////

/*
 * Reuse Distance Manager: enough to compute the reuse distance of a bunch of
 * accesses.
 */

/*
 * Opaque handle to a object that can be used to compute reuse distances across
 * a stream of accesses.
 */
struct mnemo_reusedm;

/*
 * Allocate and initialize a new reuse distance manager.
 * @param[in] max the maximum number of keys the trace will contain, 0 if
 * unknown.
 * @return a new opaque handle.
 */
struct mnemo_reusedm *mnemo_reusedm_init(size_t max);

/*
 * Add an access to a given key as part of the trace being analyzed.
 * @param[in] key a unique identifier for an element of a trace
 * @param[inout] r an handle to an initialized reuse distance manager.
 * @return the reuse distance of this access
 */
int mnemo_reusedm_add(struct mnemo_reusedm *r, unsigned long long key);

/*
 * Reinitialize a reuse distance manager.
 */
void mnemo_reusedm_reset(struct mnemo_reusedm *r);

/*
 * Frees a reuse distance manager.
 */
void mnemo_reusedm_fini(struct mnemo_reusedm *r);

#endif
