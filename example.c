#include <stdio.h>

#include "co.h"

#include "example_part.h"

typedef struct {
	int a;
	int b;
} ab_t;

static co_decl_task(ab_t, _coroutine_a, (const int a, const int b), private(
	int i;
)) {
	printf("param a %d\n", CO_P(a));
	printf("param b %d\n", CO_P(b));
	CO_V(i) = 0;
	printf("%d\n", CO_V(i));
	co_yield((ab_t){
		.a = CO_V(i)
	});
	CO_V(i) = 1;
	printf("param b %d\n", CO_P(b));
	printf("%d\n", CO_V(i));
	co_yield((ab_t){
		.a = CO_V(i)
	});
	co_return((ab_t){
		.a = 10
	});
} co_end()

static co_decl_task(int, _coroutine_b, (), private(
	co_routine_t* task_a;
	co_routine_t* task_c;
	ab_t a0;
	ab_t a1;
	ab_t a2;
)) {
	CO_V(task_a) = co_new(_coroutine_a, (12, 10));
	co_resume(CO_V(task_a), CO_V(a0));
	CO_V(task_c) = co_new(_coroutine_c, (CO_V(task_a)));
	co_resume(CO_V(task_c));
	co_resume(CO_V(task_a), CO_V(a1));
	co_resume(CO_V(task_a), CO_V(a2));
	printf("returned value %d %d %d\n", CO_V(a0).a, CO_V(a1).a, CO_V(a2).a);
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
