/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "complex_arg.h"
#include <unity.h>


void __push_complex_arg(complex_arg_q_t * q, complex_arg_t * e)
{
	complex_arg_t *i = *q;

	if (i == NULL) {
		*q = e;
	} else {
		/* Find the tail of the queue */
		while (i->fifo_reserved != NULL) {
			i = i->fifo_reserved;
		}
		/* Push the new element to the tail */
		i->fifo_reserved = e;
	}

	/* Make sure that the new element is the last one. */
	e->fifo_reserved = NULL;
}

int assert_complex_arg(complex_arg_q_t * q, void * arg)
{
	complex_arg_t *e = *q;

	if (e != NULL) {
		/* Pop element from the head */
		*q = e->fifo_reserved;

		/* Execute the assertion or return the expected return value */
		if (e->assert != NULL) {
			e->assert(e->exp_arg, arg);
		} else {
			return ((intptr_t)(e->exp_arg));
		}
		return -1;
	}

	TEST_FAIL_MESSAGE("The complex argument queue is too short");
	return -1;
}
