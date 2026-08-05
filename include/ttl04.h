#ifndef LS04_H
#define LS04_H

#include "types.h"

struct ls04 {
	const bool *a[6];
	bool       *y[6];
};

struct ls04 *ls04_create(void);
void ls04_free(struct ls04 *c);
void ls04_tick(struct ls04 *c);

#endif
