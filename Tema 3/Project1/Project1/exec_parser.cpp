#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "exec_parser.h"
#include "choosing_so.h"

#if defined windows_system

static BOOL SolveImports(uintptr_t loadBase, IMAGE_NT_HEADERS* peHdr);

static int readHeader(HANDLE hFile, DWORD offset, char* buf, DWORD size)
{
	DWORD bytesRead, totalBytesRead;
	BOOL bRet;

	if (SetFilePointer(hFile, offset, NULL, FILE_BEGIN) ==
		INVALID_SET_FILE_POINTER) {
		fprintf(stderr, "Couldn't change offset: %d\n", GetLastError());
		goto error;
	}

	totalBytesRead = 0;
	do {
		bRet = ReadFile(hFile, buf + totalBytesRead,
			size - totalBytesRead, &bytesRead, NULL);
		if (bRet == FALSE) {
			fprintf(stderr, "Couldn't read enough bytes\n");
			goto error;
		}
		totalBytesRead += bytesRead;
	} while (totalBytesRead < size);

	return 0;

error:
	CloseHandle(hFile);
	return -1;
}

so_exec_t* so_parse_exec(char* path)
{
	so_exec_t* exec = NULL;
	IMAGE_DOS_HEADER dosHdr;
	IMAGE_NT_HEADERS peHdr;
	IMAGE_SECTION_HEADER* secHdr;
	int i, nr_sections;
	so_seg_t* seg;
	HANDLE hFile;

	hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Couldn't open file %s: %d\n",
			path, GetLastError());
		return NULL;
	}

	if (readHeader(hFile, 0, (char*)&dosHdr,
		sizeof(IMAGE_DOS_HEADER)) < 0) {
		fprintf(stderr, "Couldn't read PE DOS header\n");
		goto out;
	}

	if (dosHdr.e_magic != 0x5a4d) {
		fprintf(stderr, "Couldn't find the DOS magic number\n");
		goto out;
	}

	if (readHeader(hFile, dosHdr.e_lfanew,
		(char*)&peHdr, sizeof(IMAGE_NT_HEADERS)) < 0) {
		fprintf(stderr, "Couldn't read PE DOS header\n");
		goto out;
	}

	if (peHdr.Signature != 0x4550) {
		fprintf(stderr, "Couldn't find the PE magic number\n");
		goto out;
	}

	nr_sections = peHdr.FileHeader.NumberOfSections;
	secHdr = (IMAGE_SECTION_HEADER*)malloc(nr_sections * sizeof(IMAGE_SECTION_HEADER));
	if (!secHdr) {
		fprintf(stderr, "Couldn't allocate space for sections\n");
		goto out;
	}

	if (readHeader(hFile, dosHdr.e_lfanew + sizeof(IMAGE_NT_HEADERS),
		(char*)secHdr, nr_sections *
		sizeof(IMAGE_SECTION_HEADER)) < 0) {
		fprintf(stderr, "Couldn't read PE DOS header\n");
		goto out_free;
	}

	exec = (so_exec_t*)malloc(sizeof(*exec));
	if (!exec) {
		fprintf(stderr, "Out of memory\n");
		goto out_free;
	}

	exec->base_addr = peHdr.OptionalHeader.ImageBase;
	exec->entry = peHdr.OptionalHeader.ImageBase +
		peHdr.OptionalHeader.AddressOfEntryPoint;
	exec->segments_no = peHdr.FileHeader.NumberOfSections +
		1 /* an extra segment for headers */;
	exec->segments =
		(so_seg_t*)malloc(exec->segments_no * sizeof(so_seg_t));

	i = 0;
	for (i = 0; i < peHdr.FileHeader.NumberOfSections; i++) {
		IMAGE_SECTION_HEADER* sec = &secHdr[i];

		seg = &exec->segments[i];
		seg->vaddr = peHdr.OptionalHeader.ImageBase +
			sec->VirtualAddress;
		seg->offset = sec->PointerToRawData;
		seg->file_size = sec->SizeOfRawData;
		seg->mem_size = sec->Misc.VirtualSize;
		seg->data = 0;

		seg->perm = 0;
		if (sec->Characteristics & IMAGE_SCN_MEM_READ)
			seg->perm |= PERM_R;
		if (sec->Characteristics & IMAGE_SCN_MEM_WRITE)
			seg->perm |= PERM_W;
		if (sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)
			seg->perm |= PERM_X;
	}

	/* last segment is the header */
	seg = &exec->segments[i++];
	seg->vaddr = exec->base_addr;
	seg->offset = 0;
	seg->file_size = peHdr.OptionalHeader.SizeOfHeaders;
	seg->mem_size = peHdr.OptionalHeader.SizeOfHeaders;
	seg->data = 0;
	seg->perm = PERM_R;

out_free:
	free(secHdr);
out:
	CloseHandle(hFile);

	return exec;
}

void so_start_exec(so_exec_t* exec, char* argv[])
{
	IMAGE_NT_HEADERS* peHdr = (IMAGE_NT_HEADERS*)
		(((IMAGE_DOS_HEADER*)(exec->base_addr))->e_lfanew +
			(DWORD)(exec->base_addr));
	SolveImports(exec->base_addr, peHdr);
	((void(*)(void))exec->entry)();
}

static BOOL SolveImports(uintptr_t loadBase, IMAGE_NT_HEADERS* peHdr)
{
	IMAGE_DATA_DIRECTORY* dir =
		&peHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	DWORD* oft;
	DWORD* ft;
	IMAGE_IMPORT_BY_NAME* impName;
	DWORD impOrd;
	HANDLE hModule;
	char* dllName;
	DWORD funcAddr;
	IMAGE_IMPORT_DESCRIPTOR* impDesc;

	if (!dir->Size)
		return TRUE;

	impDesc = (IMAGE_IMPORT_DESCRIPTOR*)(loadBase + dir->VirtualAddress);
	while (impDesc->OriginalFirstThunk) {
		dllName = (char*)((DWORD)loadBase + impDesc->Name);

		hModule = LoadLibrary((LPCWSTR)dllName);
		if (!hModule) {
			fprintf(stderr, "Failed to load %s: %d\n",
				dllName, GetLastError());
			return FALSE;
		}

		oft = (DWORD*)((DWORD)loadBase + impDesc->OriginalFirstThunk);
		ft = (DWORD*)((DWORD)loadBase + impDesc->FirstThunk);
		for (; *oft; oft++, ft++) {
			if (*oft & 0x80000000) {
				impOrd = *oft;
				impOrd &= 0xffff;
				funcAddr = (DWORD)
					GetProcAddress((HMODULE)hModule, (LPCSTR)impOrd);
				if (!funcAddr) {
					fprintf(stderr,
						"Couldn't find the function %d: %d\n",
						impOrd, GetLastError());
					return FALSE;
				}
				*ft = funcAddr;
			}
			else {
				impName = (IMAGE_IMPORT_BY_NAME*)
					((DWORD)loadBase + *oft);
				funcAddr = (DWORD)
					GetProcAddress((HMODULE)hModule, impName->Name);
				if (!funcAddr) {
					fprintf(stderr,
						"Couldn't find the function %s: %d\n",
						impName->Name,
						GetLastError());
					return FALSE;
				}
				*ft = funcAddr;
			}
		}

		impDesc++;
	}

	return TRUE;
}
#elif defined linux_system

#define BUFSIZE 1024

static void fix_auxv(uintptr_t base, char* envp[])
{
	Elf32_auxv_t* auxv;
	Elf32_Ehdr* ehdr;
	Elf32_Phdr* phdr;

	ehdr = (Elf32_Ehdr*)base;
	phdr = (Elf32_Phdr*)((uintptr_t)ehdr + ehdr->e_phoff);

	while (*envp)
		envp++;

	auxv = (Elf32_auxv_t*)(++envp);

	while (*envp)
		envp++;

	while (auxv->a_type != AT_NULL) {
		switch (auxv->a_type) {
		case AT_PHDR:
			auxv->a_un.a_val = (uint32_t)(unsigned long)phdr;
			break;
		case AT_BASE:
			auxv->a_un.a_val = 0;
			break;
		case AT_ENTRY:
			auxv->a_un.a_val = ehdr->e_entry;
			break;
		case AT_EXECFN:
			auxv->a_un.a_val = 0;
			break;
		}
		auxv++;
	}

}


void so_start_exec(so_exec_t* exec, char* argv[])
{
	int* pargc;

	fix_auxv(exec->base_addr, __environ);
	/* fix argv to use the one from the main prog */
	argv--;

	pargc = (int*)argv - 1;

	pargc[1] = pargc[0] - 1;

	asm volatile(
		"mov %0, %%eax\n"
		"mov %1, %%ebx\n"
		"mov %%ebx, %%esp\n"
		"xor %%ebx, %%ebx\n"
		"xor %%ecx, %%ecx\n"
		"xor %%edx, %%edx\n"
		"xor %%ebp, %%ebp\n"
		"xor %%esi, %%esi\n"
		"xor %%edi, %%edi\n"
		"jmp *%%eax\n"
		::"m"(exec->entry), "m"(argv) : );
}

so_exec_t* so_parse_exec(char* path)
{
	so_exec_t* exec = NULL;
	so_seg_t* seg;
	char hdr[BUFSIZE];
	int ret;
	Elf32_Ehdr* ehdr;
	Elf32_Phdr* phdr;
	int i;
	int j;
	int num_load_phdr;
	int pagesz;
	int fd;
	size_t diff;

	pagesz = getpagesize();

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("open");
		goto out;
	}

	ret = read(fd, hdr, BUFSIZE);
	if (ret < 0) {
		perror("read");
		goto out_close;
	}

	if (ret < (sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr))) {
		fprintf(stderr, "file too small\n");
		goto out_close;
	}

	ehdr = (Elf32_Ehdr*)hdr;
	phdr = (Elf32_Phdr*)((intptr_t)ehdr + ehdr->e_phoff);

	/* allow only 32-bit ELF executables (no PIE) for i386 */
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr->e_ident[EI_MAG3] != ELFMAG3) {
		fprintf(stderr, "not an ELF file: invalid magic\n");
		goto out_close;
	}

	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
		fprintf(stderr, "not a 32-bit ELF file\n");
		goto out_close;
	}

	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		fprintf(stderr, "not a LSB ELF file\n");
		goto out_close;
	}

	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT) {
		fprintf(stderr, "invalid EI_VERSION\n");
		goto out_close;
	}

	if (ehdr->e_ident[EI_OSABI] != ELFOSABI_GNU &&
		ehdr->e_ident[EI_OSABI] != ELFOSABI_SYSV) {
		fprintf(stderr, "invalid ABI\n");
		goto out_close;
	}

	if (ehdr->e_type != ET_EXEC) {
		fprintf(stderr, "invalid executable type\n");
		goto out_close;
	}

	if (ehdr->e_machine != EM_386) {
		fprintf(stderr, "invalid machine\n");
		goto out_close;
	}

	if (ehdr->e_version != EV_CURRENT) {
		fprintf(stderr, "invalid version\n");
		goto out_close;
	}

	if (ret < (sizeof(Elf32_Ehdr) + ehdr->e_phnum * ehdr->e_phentsize)) {
		fprintf(stderr, "too many program headers\n");
		goto out_close;
	}

	exec = malloc(sizeof(*exec));
	if (!exec) {
		fprintf(stderr, "out of memory\n");
		goto out_close;
	}

	num_load_phdr = 0;
	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_type == PT_LOAD)
			num_load_phdr++;
	}

	exec->base_addr = 0xffffffff;
	exec->entry = ehdr->e_entry;
	exec->segments_no = num_load_phdr;
	exec->segments = (so_seg_t*)malloc(num_load_phdr * sizeof(so_seg_t));

	/* convert ELF phdrs to so_segments */
	j = 0;
	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			seg = &exec->segments[j];

			seg->vaddr = ALIGN_DOWN(phdr[i].p_vaddr, pagesz);
			diff = phdr[i].p_vaddr - seg->vaddr;
			seg->offset = phdr[i].p_offset - diff;
			seg->file_size = phdr[i].p_filesz + diff;
			seg->mem_size = phdr[i].p_memsz + diff;
			seg->perm = 0;

			if (phdr[i].p_flags & PF_X)
				seg->perm |= PERM_X;
			if (phdr[i].p_flags & PF_R)
				seg->perm |= PERM_R;
			if (phdr[i].p_flags & PF_W)
				seg->perm |= PERM_W;

			if (seg->vaddr < exec->base_addr)
				exec->base_addr = seg->vaddr;

			j++;
		}
	}

out_close:
	close(fd);
out:
	return exec;
}
#endif