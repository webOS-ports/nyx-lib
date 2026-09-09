// Copyright (c) 2010-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
/**
 *******************************************************************************
 * @file nyx_file_io.c
 *
 * @brief nyx file io implementation
 *
 *******************************************************************************
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>

#include <nyx/module/nyx_log.h>
#include "msgid.h"

#define READ_BUFFER_SIZE 20

int32_t nyx_utils_read_value(char *path)
{
	int32_t val = -1;
	int32_t converted;
	char buffer[READ_BUFFER_SIZE];
	char *endptr = NULL;
	ssize_t r = -1;

	if (!path)
	{
		return -1;
	}

	int fd = open(path, O_RDONLY | O_CLOEXEC);

	if (fd >= 0)
	{
		do
		{
			r = read(fd, buffer, READ_BUFFER_SIZE - 1);
		}
		while (r < 0 && EINTR == errno);

		close(fd);

		if (r > 0)
		{
			// NUL_terminate the buffer and try to convert.
			buffer[r] = '\0';
			converted = (int32_t) strtol(buffer, &endptr, 10);

			if (endptr != buffer)
			{
				val = converted;
			}
		}
	}

	return val;
}

/**
 * Read a file into a caller-supplied buffer.
 *
 * Returns the number of bytes stored, or -1 on failure. The buffer is left
 * holding a valid string either way: callers pass uninitialised stack buffers,
 * and one that tested the result for truth rather than for -1 took the failure
 * return as success and ran strstr() over whatever the stack happened to hold.
 * Terminating up front means the worst such a caller can do is read "".
 */
int32_t nyx_utils_read(char *path, char *buf, size_t size)
{
	if (!buf || size == 0)
	{
		return -1;
	}

	buf[0] = '\0';

	if (!path)
	{
		return -1;
	}

	if (size == 1)
	{
		return 0;
	}

	int fd = open(path, O_RDONLY | O_CLOEXEC);

	if (fd == -1)
	{
		return -1;
	}

	ssize_t count;

	do
	{
		count = read(fd, buf, size - 1);
	}
	while (count < 0 && EINTR == errno);

	if (count > 0)
	{
		/* read() gave us at most size - 1, so this stays inside the buffer */
		while (count > 0 && buf[count - 1] == '\n')
		{
			count--;
		}

		buf[count] = '\0';
	}
	else
	{
		buf[0] = '\0';

		if (count < 0)
		{
			close(fd);
			return -1;
		}
	}

	close(fd);
	return (int32_t) count;
}

/**
 * Write the whole buffer, resuming where a short write left off.
 *
 * Returns 0 when everything was written, -1 otherwise.
 *
 * The loop this replaced compared a ssize_t against a size_t, so the -1 from a
 * failed write() converted to SIZE_MAX and compared greater than the length:
 * the retry stopped and, worse, the error test just below it read false, so
 * every failed write was reported as a success. It also re-sent the buffer from
 * the beginning on a partial write, duplicating whatever had already landed.
 */
static int write_all(int fd, const char *buf, size_t size)
{
	size_t offset = 0;

	while (offset < size)
	{
		ssize_t written = write(fd, buf + offset, size - offset);

		if (written < 0)
		{
			if (EINTR == errno)
			{
				continue;
			}

			return -1;
		}

		if (0 == written)
		{
			return -1;
		}

		offset += (size_t) written;
	}

	return 0;
}

void nyx_utils_write_value(char *path, int32_t val)
{
	if (!path)
	{
		return;
	}

	int fd = open(path, O_WRONLY | O_CLOEXEC);

	if (fd >= 0)
	{
		char buffer[READ_BUFFER_SIZE];
		snprintf(buffer, READ_BUFFER_SIZE, "%" PRIi32, val);

		if (write_all(fd, buffer, strlen(buffer)) < 0)
		{
			nyx_error(MSGID_NYX_UTIL_WRITE_VAL_ERR, 0,
			          "Could not write value %d to file/device at %s", val, path);
		}

		close(fd);
	}
}

void nyx_utils_write(char *path, char *buf, size_t size)
{
	if (!path || !buf || size == 0)
	{
		return;
	}

	int fd = open(path, O_WRONLY | O_CLOEXEC);

	if (fd == -1)
	{
		return;
	}

	if (write_all(fd, buf, size) < 0)
	{
		nyx_error(MSGID_NYX_UTIL_WRITE_ERR, 0, "Could not write to file/device at %s",
		          path);
	}

	close(fd);
}
