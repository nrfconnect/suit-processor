/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef _COMPLEX_ARG_H
#define _COMPLEX_ARG_H


#define COMPLEX_ARG_Q_DEFINE(name) complex_arg_q_t name = NULL

/** @brief Construct a complex arg structure and add it to the queue with expected argument values.
 *
 * @note This API allocates the memory for the argument structure as a static variable.
 *
 * @param arg  The pointer to the argument value that should be verified.
 * @param arg_assert  The assert function that will compare arguments, based on the pointers to their values.
 * @param queue  The complex arg queue to use.
 */
#define push_complex_arg(arg, arg_assert, queue) { \
	static complex_arg_t __arg; \
	__arg.exp_arg = arg; \
	__arg.assert = arg_assert; \
	__push_complex_arg(&queue, &__arg); \
	} \

/** @brief Construct a complex arg structure holding return value and add it to the queue with expected argument values.
 *
 * @note This API allocates the memory for the argument structure as a static variable.
 *
 * @param retval  The value to be returned.
 * @param queue  The complex arg queue to use.
 */
#define push_retval_arg(retval, queue) { \
	static complex_arg_t __arg; \
	__arg.exp_arg = (void *)retval; \
	__arg.assert = NULL; \
	__push_complex_arg(&queue, &__arg); \
	} \


/** @brief Function prototype for the complex argument assert function.
 *
 * @param p1  Pointer to the expected value.
 * @param p1  Pointer to the asserted value.
 */
typedef void (*complex_arg_assert_t)(void * p1, void * p2);

/** @brief Definition of a single element of the complex argument eueue. */
typedef struct {
	void *fifo_reserved; ///! The pointer to the next element on the queue.
	void *exp_arg; ///! The pointer to the expected value of the complex argument.
	complex_arg_assert_t assert; ///! The reference to the function, that is used to check argument the against the expected value.
} complex_arg_t;

/** @brief Generic type of the queue storing complex arguments. */
typedef complex_arg_t* complex_arg_q_t;


/** @brief Add a complex arg to the queue with expected argument values.
 *
 * @note This API require to pass a pointer to the structure, that will remain
 *       allocated until the element is removed from the queue.
 *       To simplify the usage of this API, a macro push_complex_arg was created
 *       that allocates the memory for the argument as a static variable, allowing
 *       for a single-call API usage.
 *
 * @param q  The complex arg queue to use.
 * @param e  Pointer to the new complex arg structure.
 */
void __push_complex_arg(complex_arg_q_t * q, complex_arg_t * e);

/** @brief Get element from the head of the complex arg queue and execute
 *         assertion function on the passed and expected argument values.
 *
 * @param q  The complex arg queue to use.
 * @param arg  Pointer to the checked complex arg value
 *
 * @return The set, expected return value if the assert function is set to NULL,
 *         -1 otherwise.
 */
int assert_complex_arg(complex_arg_q_t * q, void * arg);

#endif /* _COMPLEX_ARG_H */
