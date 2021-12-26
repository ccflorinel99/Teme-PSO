#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "so_scheduler.h"


#ifdef __linux__
#define linux_system
#elif defined(_WIN32)
#define windows_system
#endif

#define MAX_IO_DEVICES 256

typedef struct scheduler
{
	unsigned int time_quantum = 0;
	unsigned int* io_devices = NULL;
	int* vector_tid = NULL;
}scheduler;

#if defined linux_system
#define semnatura void*
#define semnatura_arg void*
#elif defined windows_system
#define semnatura DWORD WINAPI
#define semnatura_arg void*
#endif

// functie pt thread
semnatura ThreadFunc(semnatura_arg arg)
{
	if ((int)arg == 1)
	{
#if defined linux_system
		fprintf(stdout, "Thread with id %d, preparing for exit\n", pthread_self());
		pthread_exit();
#elif defined windows_system
		ExitThread(0);
#endif
	}
}

int index = 0;
scheduler* s = NULL;
tid_t start_tid;
tid_t tcount = 1;

/*
	 * creates and initializes scheduler
	 * + time quantum for each thread
	 * + number of IO devices supported
	 * returns: 0 on success or negative on error
	 */
DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io)
{
	s = (scheduler*)malloc(sizeof(scheduler));

	if (s == NULL)
	{
		fprintf(stderr, "Cannnot allocate memory for scheduler\n");
		return -1;
	}
	else
	{
		if (io >= MAX_IO_DEVICES)
		{
			fprintf(stderr, "Number of IO devices exceeds the maximum number (%d)\n", MAX_IO_DEVICES);
			return -1;
		}
		else
		{
			if (index >= MAX_IO_DEVICES)
			{
				fprintf(stderr, "Cannnot add IO devices. The index number is equal to or exceeds the maximum number (%d)\n", MAX_IO_DEVICES);
				return -1;
			}
			else
			{
				s->io_devices = (unsigned int*)malloc(MAX_IO_DEVICES * sizeof(unsigned int));
				for (int i = 0; i < MAX_IO_DEVICES; i++) s->io_devices[i] = 0;
				
				s->vector_tid = (int*)malloc(MAX_IO_DEVICES * sizeof(int));

				s->io_devices[index] = io;
#if defined linux_system
				start_tid = pthread_self();
				s->vector_tid[index] = start_tid;
#elif defined windows_system
				start_tid = GetCurrentThreadId();
				s->vector_tid[index] = start_tid;
#endif
				index++;
				s->time_quantum = time_quantum;

				return 0; // SUCCESS
			}
		}
	}
}

/*
 * creates a new so_task_t and runs it according to the scheduler
 * + handler function
 * + priority
 * returns: tid of the new task if successful or INVALID_TID
 */
DECL_PREFIX tid_t so_fork(so_handler* func, unsigned int priority)
{
	if (priority > SO_MAX_PRIO)
	{
		fprintf(stderr, "The chosen priority (%d) is greater than maximum priority allowed (%d)\n", priority, SO_MAX_PRIO);
		return INVALID_TID;
	}
	else
	{
		tid_t new_id = start_tid + tcount;
		tcount++;
		
		s->vector_tid[index] = (int)new_id;
		index++;

#if defined linux_system
		int id = pthread_create(&new_id, NULL, &ThreadFunc, (semnatura_arg)1);
		pthread_setschedprio((tid_t)id, priority);

		int rez = pthread_join((tid_t)id, NULL);

		if (rez != 0)
		{
			fprintf(stderr, "Problems at pthread_join\n");
			return INVALID_TID;
		}
#elif defined windows_system
		HANDLE hthread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE) ThreadFunc,
			(semnatura_arg)1,
			0,
			&new_id
		);

		SetThreadPriority(hthread, priority);

		WaitForSingleObject(hthread, INFINITE);
#endif

		return new_id;
	}
}

/*
 * waits for an IO device
 * + device index
 * returns: -1 if the device does not exist or 0 on success
 */
DECL_PREFIX int so_wait(unsigned int io)
{
	int gasit = 0;

	for (int i = 0; i < index; i++)
	{
		if (s->io_devices[i] == io) gasit = 1;
	}

	if (gasit == 1) return 0;
	else
	{
		fprintf(stderr, "Could not find the device\n");
		return -1;
	}
}

/*
 * signals an IO device
 * + device index
 * return the number of tasks woke or -1 on error
 */
DECL_PREFIX int so_signal(unsigned int io)
{
	// idk how to do it
	// improvising....

	// verific daca io se afla in vectorul de index-uri pt dispozitive io
	int gasit = 0;

	for (int i = 0; i < MAX_IO_DEVICES; i++)
	{
		if (s->io_devices[i] == io) gasit = 1;
	}

	if (gasit == 0) return -1;
	else return (int)tcount - 1;
}

/*
 * does whatever operation
 */
DECL_PREFIX void so_exec(void)
{
	tid_t id;
#if defined linux_system
	id = pthread_self();
#elif defined windows_system
	id = GetCurrentThreadId();
#endif

	fprintf(stdout, "Thread with id %d here. It's executing time!!!!\n", id);
	
	int trash = 0;
	for (int i = 0; i < INT_MAX; i++)
		trash += i;
	
	fprintf(stdout, "Thread with id %d has finished executing\n", id);
}

/*
 * destroys a scheduler
 */
DECL_PREFIX void so_end(void)
{
	free(s->io_devices);
	free(s->vector_tid);
	free(s);
	s = NULL;
}