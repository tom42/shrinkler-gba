/*
 * argp-standalone: standalone version of glibc's argp functions.
 * Copyright (C) 2020 Thomas Mathys
 *
 * argp-standalone is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * argp-standalone is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "argp-compat.h"

#if defined(HAVE_MEMPCPY) && !HAVE_MEMPCPY
void* argp_compat_mempcpy(void* out, const void* in, size_t n)
{
    memcpy(out, in, n);
    return (char*)out + n;
}
#endif

#if defined(HAVE_SLEEP) && !HAVE_SLEEP
#if defined(_WIN32)
void argp_compat_sleep(unsigned int seconds)
{
  for (; seconds; --seconds)
  {
    Sleep(1000);
  }
}
#else
#error No replacement for sleep() is available for the target platform
#endif
#endif

#if defined(HAVE_STRCASECMP) && !HAVE_STRCASECMP
#if defined(_WIN32)
int argp_compat_strcasecmp(const char* s1, const char* s2)
{
  return _stricmp(s1, s2);
}
#else
#error No replacement for strcasecmp() is available for the target platform
#endif
#endif

#if defined(HAVE_STRCHRNUL) && !HAVE_STRCHRNUL
const char* argp_compat_strchrnul(const char* s, int c)
{
  while (*s && (*s != c))
  {
    ++s;
  }
  return s;
}
#endif

#if defined (HAVE_STRNDUP) && !HAVE_STRNDUP
char* argp_compat_strndup(const char* s, size_t n)
{
  char* result;

  /* Find out how many chars to copy: entire string or n characters? */
  size_t s_len = strlen(s);
  if (s_len < n)
    n = s_len;

  /* Get memory, return 0 if insufficient memory. */
  result = malloc(n + 1);
  if (!result)
    return result;

  /* Copy (sub)string and terminate it */
  memcpy(result, s, n);
  result[n] = '\0';
  return result;
}
#endif
