#ifndef LS153_H
#define LS153_H

#include "types.h"

struct ls153 {
	const bool *d[2][4];
	const bool *s[2];
	const bool *g[2];
	bool       *y[2];
};

struct ls153 *ls153_create(void);
void ls153_free(struct ls153 *c);
void ls153_tick(struct ls153 *c);

#endif
