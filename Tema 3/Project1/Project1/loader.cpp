#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

/*
 * Loader Implementation
 *
 * 2018, Operating Systems
 */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include "choosing_so.h"
#include "exec_parser.h"

static so_exec_t* exec;

#define default_size 1024


#if defined linux_system
static void sig_handler(int signum)
{
	if (signum == SIGSEGV)
		printf("Bad memory accessed\n");
	fflush(stdout);
}
#elif defined windows_system
int map_failed()
{
	fprintf(stderr, "Mapping failed");
	return -1;
}
#endif

int so_init_loader(void)
{
	/* TODO: initialize on-demand loader */
	so_seg_t* st = (so_seg_t*)malloc(sizeof(so_seg_t));
	so_exec_t* se = (so_exec_t*)malloc(sizeof(so_exec_t));

#if defined linux_system
	struct sigaction signal;
	sigset_t mask;

	sigemptyset(&mask);

	memset(&signal, 0, sizeof(struct sigaction));
	signal.sa_flags = SA_RESETHAND;
	signal.sa_mask = mask;

	signal.sa_handler = sig_handler;
	sigaction(SIGSEGV, &sa, NULL);

	for (int i = 1; i <= default_size; i++)
	{
		void* rez = mmap((void*)(i*default_size), default_size, PROT_READ, MAP_SHARED, fd, NULL);
		if (rez == MAP_FAILED)
		{
			fprintf(stderr, "Failed to map %d page size", i * default_size);
		}
	}

#elif defined windows_system
	HANDLE hFile = GetModuleHandleA;
	HANDLE map = CreateFileMappingA( hFile, NULL, PAGE_EXECUTE_READWRITE, 0x7fffffff, 0xffffffff, NULL);
	LPVOID lmap = MapViewOfFileEx(map, FILE_MAP_READ, 0x7fffffff, 0xffffffff, 0, NULL);

	if (lmap == NULL) return map_failed();
#endif
	return 0;
}

int so_execute(char* path, char* argv[])
{
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}