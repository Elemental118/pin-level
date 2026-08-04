#ifndef A2CLOCK_H
#define A2CLOCK_H

struct a2clock {
	bool *ras;
	bool *ax;
	bool *cas;
	bool *phase0;
	bool *phase1;
	
	struct a2clock_priv *priv;
};

struct a2clock *a2clock_create(void);
void a2clock_free(struct a2clock *c);
void a2clock_tick(struct a2clock *c);

#endif
