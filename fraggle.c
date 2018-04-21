/*
 * Copyright (c) 2018 Steven Roberts <sroberts@fenderq.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "diceware.h"

extern char *optarg;

__dead void usage(void);

int
main(int argc, char *argv[])
{
	char ch;
	char fname[FILENAME_MAX];
	const char *ep;
	int count;
	int words;

	if (pledge("rpath stdio", NULL) == -1)
		err(1, "pledge");

	count = 1;

	while ((ch = getopt(argc, argv, "c:")) != -1) {
		switch (ch) {
		case 'c':
			count = strtonum(optarg, 1, 1024 * 64, &ep);
			if (ep != NULL)
				errx(1, "%s %s", optarg, ep);
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 2)
		usage();

	strlcpy(fname, argv[0], sizeof(fname));
	words = strtonum(argv[1], 1, 32, &ep);
	if (ep != NULL)
		errx(1, "%s %s", argv[1], ep);

	while (count-- > 0)
		diceware(fname, words);

	exit(EXIT_SUCCESS);
}

void
usage(void)
{
	fprintf(stderr, "usage: %s [-c count] <file> <words>\n", getprogname());
	exit(EXIT_FAILURE);
}
