#include <stdio.h>

#include "example_part.h"

static co_decl(_coroutine_d, ());

co_task(_coroutine_c, (co_routine_t* const other), private(
	co_routine_t* task_d;
)) {
	printf("wait for task %p\n", CO_P(other));
	co_await(CO_P(other));
	printf("resumed because task a is done\n");
	CO_V(task_d) = co_new(_coroutine_d, ());
	co_resume(CO_V(task_d));
	printf("resumed because task d is done\n");
	co_free(CO_V(task_d));
	co_return();
} co_end()

co_task(_coroutine_d, (), private(
)) {
	printf("this is coroutine d\n");
} co_end()
