#pragma once
#if defined(__linux__)
#include <elf.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <err.h>
#include <sys/mman.h>
#define linux_system
#elif defined(_WIN32)
#include <Windows.h>
#define sleep(x) Sleep(1000 * (x))
#include <Winternl.h>
#include <ctype.h>
#include <memoryapi.h>
#define windows_system
#endif