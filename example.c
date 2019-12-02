#include <stdio.h>

#include "co.h"

#include "example_part.h"

static co_decl_task(int, _coroutine_a, (const int a, const int b), private(
	int i;
)) {
	printf("param a %d\n", CO_P(a));
	printf("param b %d\n", CO_P(b));
	CO_V(i) = 0;
	printf("%d\n", CO_V(i));
	co_yield(CO_V(i));
	CO_V(i) = 1;
	printf("param b %d\n", CO_P(b));
	printf("%d\n", CO_V(i));
	co_yield(CO_V(i));
	co_return(10);
} co_end()

co_decl_task(int, _coroutine_b, (), private(
	co_routine_t* task_a;
	co_routine_t* task_c;
	int a0;
	int a1;
	int a2;
)) {
	CO_V(task_a) = co_new(_coroutine_a, (12, 10));
	co_resume(CO_V(task_a), CO_V(a0));
	CO_V(task_c) = co_new(_coroutine_c, (CO_V(task_a)));
	co_resume(CO_V(task_c));
	co_resume(CO_V(task_a), CO_V(a1));
	co_resume(CO_V(task_a), CO_V(a2));
	printf("returned value %d %d %d\n", CO_V(a0), CO_V(a1), CO_V(a2));
	co_free(CO_V(task_a));
	co_free(CO_V(task_c));
} co_end()

int main(void)
{
	co_scheduler_t* scheduler = co_scheduler_new();
	co_routine_t* const task = co_new(_coroutine_b, ());
	co_schedule(scheduler, task);
	co_free(task);
	co_scheduler_free(scheduler);
	return 0;
}
