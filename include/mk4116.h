#ifndef MK4116_H
#define MK4116_H

#include "types.h"

struct mk4116 {
	const bool *a[7];
	const bool *ras;
	const bool *cas;
	const bool *we;
	const bool *din;
	bool       *dout;
	
	struct mk4116_priv *priv;
};

struct mk4116 *mk4116_create(void);
void mk4116_free(struct mk4116 *c);
void mk4116_tick(struct mk4116 *c);

#endif
