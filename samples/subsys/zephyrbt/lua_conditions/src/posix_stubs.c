/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Minimal stubs for POSIX functions referenced by Lua's
 * io/os libraries (liolib.c, loslib.c). These libraries
 * are compiled by lua_zephyr but never used by ZephyrBT
 * condition scripts. The stubs satisfy the linker.
 */

#include <errno.h>
#include <sys/types.h>

int open(const char *path, int flags, ...)
{
	errno = ENOSYS;
	return -1;
}

int close(int fd)
{
	errno = ENOSYS;
	return -1;
}

ssize_t read(int fd, void *buf, size_t count)
{
	errno = ENOSYS;
	return -1;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	errno = ENOSYS;
	return -1;
}

off_t lseek(int fd, off_t offset, int whence)
{
	errno = ENOSYS;
	return -1;
}

int rename(const char *old, const char *new)
{
	errno = ENOSYS;
	return -1;
}

int unlink(const char *path)
{
	errno = ENOSYS;
	return -1;
}

long times(void *buf)
{
	errno = ENOSYS;
	return -1;
}
