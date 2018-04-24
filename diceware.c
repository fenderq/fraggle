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

#include <sys/queue.h>

#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diceware.h"

SLIST_HEAD(dwlist, dwentry);
struct dwentry {
	SLIST_ENTRY(dwentry) entries;
	char *digit;
	char *word;
};

struct dwfile {
	char **words;
	char fname[PATH_MAX];
	int items;
	struct dwlist head;
};

static struct dwfile *dwdata;

static void	 dwfile_free(struct dwfile *);
static int	 dwfile_load(struct dwfile *, char *);
static int	 dwfile_print(struct dwfile *, int);

static int	 dwlist_add(struct dwlist *, char *);
static void	 dwlist_free(struct dwlist *);

/*
 * Generate and print diceware passphrase.
 * http://world.std.com/~reinhold/diceware.html
 * https://www.eff.org/deeplinks/2016/07/new-wordlists-random-passphrases
 */
int
diceware(char *fname, int num)
{
	/* Deallocate existing data when file name is different. */
	if (dwdata && strcmp(fname, dwdata->fname) != 0) {
		dwfile_free(dwdata);
		free(dwdata);
		dwdata = NULL;
	}

	/* Allocate new file data. */
	if (dwdata == NULL) {
		if ((dwdata = calloc(1, sizeof(struct dwfile))) == NULL)
			err(1, NULL);
		SLIST_INIT(&dwdata->head);
		dwfile_load(dwdata, fname);
	}

	dwfile_print(dwdata, num);

	return 0;
}

static void
dwfile_free(struct dwfile *dw)
{
	dwlist_free(&dw->head);
	free(dw->words);
	dw->words = NULL;
	dw->items = 0;
}

static int
dwfile_load(struct dwfile *dw, char *fname)
{
	FILE *fp;
	char line[LINE_MAX];
	int i;
	struct dwentry *np;

	strlcpy(dw->fname, fname, sizeof(dw->fname));

	/* Read file, add items. */
	if ((fp = fopen(dw->fname, "r")) == NULL)
		err(1, "%s", dw->fname);
	while (fgets(line, sizeof(line), fp) != NULL) {
		dwlist_add(&dw->head, line);
		dw->items++;
	}
	if (ferror(fp) != 0)
		errx(1, "error reading file %s", dw->fname);
	fclose(fp);

	/* Create pointer array of words. */
	if ((dw->words = calloc(dw->items, sizeof(char *))) == NULL)
		err(1, NULL);
	i = dw->items;
	SLIST_FOREACH(np, &dw->head, entries)
		dw->words[--i] = np->word;

	return 0;
}

static int
dwfile_print(struct dwfile *dw, int num)
{
	char ch;
	const char *p;
	int i;
	int n;

	for (i = 0; i < num; i++) {
		n = arc4random_uniform(dw->items);
		p = dw->words[n];
		ch = i < num - 1 ? ' ' : '\n';
		printf("%s%c", p, ch);
	}

	return 0;
}

/*
 * Line must be in format:
 * DIGIT\tWORD\n
 */
static int
dwlist_add(struct dwlist *head, char *line)
{
	char *p1;
	char *p2;
	size_t len;
	struct dwentry *np;

	if ((np = malloc(sizeof(struct dwentry))) == NULL)
		err(1, NULL);

	/* Get the digit. */
	p1 = line;
	if ((p2 = strchr(p1, '\t')) == NULL)
		errx(1, "missing tab: %s", line);
	len = p2 - p1;
	np->digit = strndup(p1, len);

	/* Get the word. */
	p1 = ++p2;
	if ((p2 = strchr(p1, '\n')) == NULL)
		errx(1, "missing newline: %s", line);
	len = p2 - p1;
	np->word = strndup(p1, len);

	/* Add the entry. */
	SLIST_INSERT_HEAD(head, np, entries);

	return 0;
}

static void
dwlist_free(struct dwlist *head)
{
	struct dwentry *np;

	while (!SLIST_EMPTY(head)) {
		np = SLIST_FIRST(head);
		SLIST_REMOVE_HEAD(head, entries);
		free(np->digit);
		free(np->word);
		free(np);
	}
}
