/*
 * Copyright (c) 2016 Dmitry V. Levin <ldv@altlinux.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "tests.h"
#include <sys/syscall.h>
#ifdef HAVE_PRCTL
# include <sys/prctl.h>
#endif

#if defined HAVE_PRCTL && defined PR_SET_SECCOMP && defined __NR_exit

# include <assert.h>
# include <errno.h>
# include <stdio.h>
# include <unistd.h>

int
main(void)
{
# if defined __x86_64__ && defined __ILP32__
	/*
	 * Syscall numbers are hardcoded in
	 * kernel/seccomp.c:__secure_computing_strict(),
	 * but x32 syscall numbers are not supported.
	 */
	error_msg_and_skip("SECCOMP_MODE_STRICT is not supported on x32");
# else
	static const char text1[] =
		"prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT) = 0\n";
	static const char text2[] = "+++ exited with 0 +++\n";
	int rc = 0;

	assert(prctl(PR_SET_SECCOMP, -1L, 1, 2, 3) == -1);
	printf("prctl(PR_SET_SECCOMP, %#x /* SECCOMP_MODE_??? */, 0x1, 0x2, 0x3)"
	       " = -1 %s (%m)\n", -1, ENOSYS == errno ? "ENOSYS" : "EINVAL");
	fflush(stdout);

	if (prctl(PR_SET_SECCOMP, 1)) {
		printf("prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT) = -1 %s (%m)\n",
		       ENOSYS == errno ? "ENOSYS" : "EINVAL");
		fflush(stdout);
	} else {
		rc += write(1, text1, LENGTH_OF(text1)) != LENGTH_OF(text1);
	}

	rc += write(1, text2, LENGTH_OF(text2)) != LENGTH_OF(text2);
	return !!syscall(__NR_exit, rc);
# endif
}

#else

SKIP_MAIN_UNDEFINED("HAVE_PRCTL && PR_SET_SECCOMP && __NR_exit")

#endif