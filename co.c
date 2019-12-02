#include <assert.h>
#include "co.h"

static void _co_prepend_task(co_scheduler_t* const scheduler, co_routine_t* const task)
{
	if (scheduler->head)
	{
		scheduler->head->prev = task;
		task->next = scheduler->head;
	} else {
		scheduler->tail = task;
		task->next = 0;
	}
	scheduler->head = task;
	task->prev = 0;
}

static void _co_append_task(co_scheduler_t* const scheduler, co_routine_t* const task)
{
	if (scheduler->tail)
	{
		scheduler->tail->next = task;
		task->prev = scheduler->tail;
	} else {
		scheduler->head = task;
		task->prev = 0;
	}
	scheduler->tail = task;
	task->next = 0;
}

static void _co_delete_task(co_scheduler_t* const scheduler, co_routine_t* const task)
{
	if (task->prev)
		task->prev->next = task->next;
	else
		scheduler->head = task->next;
	if (task->next)
		task->next->prev = task->prev;
	else
		scheduler->tail = task->prev;

}

static void _co_done(co_routine_t* const task)
{
	if (task->notify_any)
	{
		co_routine_t* const notify_any = task->notify_any;
		task->notify_any = 0;
		_co_prepend_task(task->scheduler, notify_any);
		int i;
		const int other_size = notify_any->other_size;
		notify_any->other_size = 0;
		co_routine_t* const* const others = notify_any->others;
		for (i = 0; i < other_size; i++)
			if (others[i] != task)
			{
				assert(others[i]->notify_any == notify_any);
				others[i]->notify_any = 0;
			}
	}
}

void _co_resume(co_routine_t* const self, co_routine_t* const task)
{
	assert(!task->done);
	_co_prepend_task(self->scheduler, self);
	task->scheduler = self->scheduler;
	_co_prepend_task(self->scheduler, task);
}

int _co_await_any(co_routine_t* const self, co_routine_t* const* const tasks, const int task_size)
{
	assert(task_size > 0);
	if (task_size == 1) // Special casing this, no need to add to others list, which has life-cycle requirement for this list.
	{
		self->others = 0;
		self->other_size = 0;
		if (tasks[0]->done)
			return 1;
		tasks[0]->notify_any = self;
		return 0;
	}
	self->others = tasks;
	self->other_size = task_size;
	int i;
	int flag = 0;
	for (i = 0; !flag && i < task_size; i++)
	{
		flag = tasks[i]->done;
		assert(tasks[i]->notify_any == 0);
		tasks[i]->notify_any = self;
	}
	if (flag)
	{
		for (i = 0; i < task_size; i++)
			tasks[i]->notify_any = 0;
		return 1;
	}
	return 0;
}

void co_free(co_routine_t* const task)
{
	free(task);
}

int co_is_done(const co_routine_t* const task)
{
	return task->done;
}

static void _co_main(co_scheduler_t* const scheduler)
{
	for (;;)
	{
		if (scheduler->head == 0)
			break;
		co_routine_t* const task = scheduler->head;
		_co_delete_task(scheduler, task);
		const co_state_t state = task->fn(task, task + 1, task->privates);
		task->line = state.line;
		task->done = state.done;
		if (task->done)
			_co_done(task);
	}
}

co_scheduler_t* co_scheduler_new(void)
{
	co_scheduler_t* const scheduler = calloc(1, sizeof(co_scheduler_t));
	return scheduler;
}

void co_scheduler_free(co_scheduler_t* const scheduler)
{
	free(scheduler);
}

void co_schedule(co_scheduler_t* const scheduler, co_routine_t* const task)
{
	task->scheduler = scheduler;
	_co_append_task(scheduler, task);
	if (scheduler->active)
		return;
	_co_main(scheduler);
}
