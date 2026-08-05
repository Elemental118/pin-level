#ifndef LS139_H
#define LS139_H

#include "types.h"

struct ls139 {
	const bool *s[2][2];
	const bool *g[2];
	bool       *y[2][4];
};

struct ls139 *ls139_create(void);
void ls139_free(struct ls139 *c);
void ls139_tick(struct ls139 *c);

#endif
