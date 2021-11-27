/*
 * Operating System Executable Loader header
 *
 * 2019, Operating Systems
 */

#ifndef SO_STDIO_H
#define SO_STDIO_H


#if defined(__linux__)
#define FUNC_DECL_PREFIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#define linux_system
#elif defined(_WIN32)
#include <Windows.h>
#define windows_system


#define DLL_EXPORTS


#ifdef DLL_EXPORTS
#define FUNC_DECL_PREFIX __declspec(dllexport)
#else
#define FUNC_DECL_PREFIX __declspec(dllimport)
#endif

#else
#error "Unknown platform"
#endif

#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE  4096
#define _CRT_SECURE_NO_WARNINGS

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define SO_EOF (-1)


struct _so_file;

typedef struct _so_file SO_FILE;

// done
FUNC_DECL_PREFIX SO_FILE* so_fopen(const char* pathname, const char* mode)
{
	int mod;
	// 0 - r, 1 - r+, 2 - w, 3 - w+, 4 - a, 5 - a+
	if (strcmp(mode, "r") == 0) mod = 0;
	else if (strcmp(mode, "r+") == 0) mod = 1;
	else if (strcmp(mode, "w") == 0) mod = 2;
	else if (strcmp(mode, "w+") == 0) mod = 3;
	else if (strcmp(mode, "a") == 0) mod = 4;
	else if (strcmp(mode, "a+") == 0) mod = 5;
	else fprintf(stdout, "Wrong mode used");

#ifdef linux_system
	switch (mod)
	{
	case 0: // r
	{
		return (SO_FILE*)open(pathname, O_RDONLY);
	}
	case 1: // r+
	{
		return (SO_FILE*)open(pathname, O_RDWR);
	}
	case 2: // w
	{
		return (SO_FILE*)open(pathname, O_WRONLY);
	}
	case 3: // w+
	{
		return (SO_FILE*)open(pathname, O_RDWR);
	}
	case 4: // a
	{
		return (SO_FILE*)open(pathname, O_APPEND);
	}
	case 5: // a+
	{
		return (SO_FILE*)open(pathname, O_APPEND);
	}
	}
#elif defined windows_system
	int optiune_deschidere;
	LONG desired_access;
	switch (mod)
	{
	case 0: // r
	{
		optiune_deschidere = OPEN_EXISTING;
		desired_access = GENERIC_READ;
	}
	case 1: // r+
	{
		optiune_deschidere = OPEN_EXISTING;
		desired_access = GENERIC_READ | GENERIC_WRITE;
	}
	case 2: // w
	{
		optiune_deschidere = CREATE_ALWAYS;
		desired_access = GENERIC_WRITE;
	}
	case 3: // w+
	{
		optiune_deschidere = CREATE_ALWAYS;
		desired_access = GENERIC_READ | GENERIC_WRITE;
	}
	case 4: // a
	{
		optiune_deschidere = OPEN_ALWAYS;
		desired_access = GENERIC_READ | GENERIC_WRITE;
	}
	case 5: // a+
	{
		optiune_deschidere = OPEN_ALWAYS;
		desired_access = GENERIC_READ | GENERIC_WRITE;
	}
	}

	HANDLE h = CreateFileA(pathname, desired_access, FILE_SHARE_READ, NULL, optiune_deschidere, FILE_ATTRIBUTE_NORMAL, NULL);
	return (SO_FILE*)h;
#endif
}
// done
FUNC_DECL_PREFIX int so_fclose(SO_FILE* stream)
{
	//return fclose((FILE*)stream);
#ifdef linux_system
	return close(so_fileno(stream));
#elif defined windows_system
	return CloseHandle(so_fileno(stream));
#endif
}

#if defined(__linux__) // done
FUNC_DECL_PREFIX int so_fileno(SO_FILE* stream)
{
	int nr = fileno(stream);
	return nr;
}

#elif defined(_WIN32) // done
FUNC_DECL_PREFIX HANDLE so_fileno(SO_FILE* stream)
{
	HANDLE h = (HANDLE)fileno((FILE*)stream);
	return h;
}
#else // done
#error "Unknown platform"
#endif

// done
FUNC_DECL_PREFIX int so_fflush(SO_FILE* stream)
{
	int rezultat = fflush((FILE*)stream);
	if (rezultat == EOF) return SO_EOF;
	else return rezultat;
}
// done
FUNC_DECL_PREFIX int so_fseek(SO_FILE* stream, long offset, int whence)
{
	return fseek((FILE*)stream, offset, whence);
}
// done
FUNC_DECL_PREFIX long so_ftell(SO_FILE* stream)
{
	return ftell((FILE*)stream);
}

// done
FUNC_DECL_PREFIX
size_t so_fread(void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
	//return fread(ptr, size, nmemb, (FILE*)stream);
	size_t total_citit = 0;

#ifdef linux_system
	total_citit = read(so_fileno(stream), ptr, nmemb);
	
#elif defined windows_system
	DWORD b_read = 0;
	BOOL b, done = FALSE;
	while (!done)
	{
		b = ReadFile(so_fileno(stream), ptr, size, &b_read, NULL);

		total_citit += b_read;

		if (total_citit == size) done = TRUE;
	}
#endif
	
	return total_citit;
}

// done
FUNC_DECL_PREFIX
size_t so_fwrite(const void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
	//return fwrite(ptr, size, nmemb, (FILE*)stream);
#ifdef linux_system
	int fd = so_fileno(stream);
	int scriere = write(fd, ptr, nmemb);
	if (scriere == 0) return so_feof(stream);
	else return scriere;
#elif defined windows_system
	HANDLE h = so_fileno(stream);
	DWORD b_written;
	int size = 1;
	BOOL b, done = FALSE;
	int dim = 0;

	while (!done)
	{
		b = WriteFile(h, ptr, size, &b_written, NULL);
		dim += b_written;
		if (dim == size) done = TRUE;
	}

	if (b == TRUE) return dim;
	else return so_feof(stream);
#endif

}

// done
FUNC_DECL_PREFIX int so_fgetc(SO_FILE* stream)
{
	//return fgetc((FILE*)stream);
	int rezultat;
#ifdef linux_system
	int fd = so_fileno(stream);
	int citit = read(fd, &rezultat, 1);
	if (citit == 0) return so_feof(stream);
	else return citit;
#elif defined windows_system
	HANDLE h = so_fileno(stream);
	DWORD b_read;
	int size = 1;
	BOOL b, done = FALSE;
	while (!done)
	{
		b = ReadFile(h, &rezultat, size, &b_read, NULL);
		
		if (b_read == size) done = TRUE;
	}

	if (b == TRUE) return rezultat;
	else return so_feof(stream);
#endif
}
// done
FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE* stream)
{
	//return fputc(c, (FILE*)stream);
#ifdef linux_system
	int fd = so_fileno(stream);
	int scriere = write(fd, &c, 1);
	if (scriere == 0) return so_feof(stream);
	else return scriere;
#elif defined windows_system
	HANDLE h = so_fileno(stream);
	DWORD b_written;
	int size = 1;
	BOOL b, done = FALSE;
	int dim = 0;

	while (!done)
	{
		b = WriteFile(h, &c, size, &b_written, NULL);
		dim += b_written;
		if (dim == size) done = TRUE;
	}

	if (b == TRUE) return dim;
	else return so_feof(stream);
#endif
}
// done
FUNC_DECL_PREFIX int so_feof(SO_FILE* stream)
{
	return feof((FILE*)stream);
}
// done
FUNC_DECL_PREFIX int so_ferror(SO_FILE* stream)
{
	return ferror((FILE*)stream);
}

// TO DO
FUNC_DECL_PREFIX SO_FILE* so_popen(const char* command, const char* type)
{
	FILE* f = _popen(command, type);
	return (SO_FILE*)f;
	
#pragma region Incercare
/*
	// creez pipe
	// pt type (r - citire de la iesirea standard a procesului creat; w - scriere la intrarea standard a procesului creat)
	// creez proces
	// nu stiu sa rulez comenzi asa cum scrie in cerinta (sh -c command -> linux, cmd /C command -> windows)
#ifdef linux_system
	pid_t pid;
	int rc;
	int fds[2];

	rc = pipe(fds);
	pid = fork();

	switch (pid) {
	case -1:	// error
	{
		close(fds[PIPE_READ]);
		close(fds[PIPE_WRITE]);
		return NULL;
		break;
	}
	case 0:		// child process
	{
		if (type == "r") close(fds[PIPE_WRITE]);
		else if (type == "w") close(fds[PIPE_READ]);
		
		SO_FILE* f = so_fopen(command, type);
		return f;
		break;
	}

	default:	// parent process
		wait_ret = waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			printf("Child process (pid %d) terminated normally, "
				"with exit code %d\n",
				pid, WEXITSTATUS(status));
	}



#elif defined windows_system
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = TRUE;
	si.cb = sizeof(si);
	HANDLE hReadPipe, hWritePipe;

	HANDLE parent_process = GetCurrentProcess();

	CreatePipe(
		&hReadPipe,
		&hWritePipe,
		&sa,        //pentru moștenire sa.bInheritHandle=TRUE
		0           //dimensiunea default pentru pipe
	);

	BOOL bRes = CreateProcess(
		NULL,          // No module name
		(LPWSTR)command,  // Command line
		NULL,          // Process handle not inheritable
		NULL,          // Thread handle not inheritable
		FALSE,         // Set handle inheritance to false
		0,             // No creation flags
		NULL,          // Use parent's environment block
		NULL,          // Use parent's starting directory
		&si,           // Pointer to STARTUPINFO structure
		&pi            // Pointer to PROCESS_INFORMATION structure
	);

	if (bRes == 0) return NULL;
	else
	{
		if (GetCurrentProcess() == parent_process)
		{
			DWORD rez;
			rez = WaitForSingleObject(hReadPipe, INFINITE);
			
			if (rez == WAIT_FAILED) return NULL;
			
			rez = WaitForSingleObject(hWritePipe, INFINITE);
			if (rez == WAIT_FAILED) return NULL;

			LPDWORD lpExitCode;
			GetExitCodeProcess(parent_process, lpExitCode);
			return lpExitCode;
		}
		else // copilul
		{
			if (type == "r") CloseHandle(hWritePipe);
			else if (type == "w") CloseHandle(hReadPipe);

			SO_FILE* f = so_fopen(command, type);
			return f;
		}
	}



#endif
*/
#pragma endregion
}
// done
FUNC_DECL_PREFIX int so_pclose(SO_FILE* stream)
{
	return _pclose((FILE*)stream);
#pragma region Incercare
/*

#ifdef linux_system
	int* status;
	so_fclose(stream);
	waitpid(-1, &status, 0);
	if (status == NULL) return -1;
	else return &status;

#elif defined windows_system
	so_fclose(stream);
	HANDLE hProcess = so_fileno(stream);
	DWORD cod;
	DWORD dwRes = WaitForSingleObject(hProcess, INFINITE);
	if (dwRes == WAIT_FAILED) return -1;
	else
	{
		GetExitCodeProcess(hProcess, &cod);
		return cod;
	}
#endif
*/
#pragma endregion
}

#endif /* SO_STDIO_H */