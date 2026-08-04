#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "a2clock.h"
#include "a2kbd.h"
#include "a2graphics.h"
#include "mcs6502.h"
#include "mk4116.h"
#include "rom2716.h"
#include "types.h"

#define ADDR(b)	((u16)*b->a[0] | ((u16)*b->a[1] << 1) | ((u16)*b->a[2] << 2) | ((u16)*b->a[3] << 3) |((u16)*b->a[4] << 4) | ((u16)*b->a[5] << 5) | \
		((u16)*b->a[6] << 6) | ((u16)*b->a[7] << 7) | ((u16)*b->a[8] << 8) | ((u16)*b->a[9] << 9) | ((u16)*b->a[10] << 10)                 | \
		((u16)*b->a[11] << 11) | ((u16)*b->a[12] << 12) | ((u16)*b->a[13] << 13) | ((u16)*b->a[14] << 14) | ((u16)*b->a[15] << 15))

#define DATA(b)	((u8)*b->d[0] | ((u8)*b->d[1] << 1) | ((u8)*b->d[2] << 2) | ((u8)*b->d[3] << 3) |((u8)*b->d[4] << 4) | ((u8)*b->d[5] << 5) | \
		((u8)*b->d[6] << 6) | ((u8)*b->d[7] << 7))

#define SET_ADDR(b, val) *b->a[0] = val & 1; *b->a[1] = (val >> 1) & 1; *b->a[2] = (val >> 2) & 1; *b->a[3] = (val >> 3) & 1; \
			 *b->a[4] = (val >> 4) & 1; *b->a[5] = (val >> 5) & 1; *b->a[6] = (val >> 6) & 1; *b->a[7] = (val >> 7) & 1; \
			 *b->a[8] = (val >> 8) & 1; *b->a[9] = (val >> 9) & 1; *b->a[10] = (val >> 10) & 1; *b->a[11] = (val >> 11) & 1; \
			 *b->a[12] = (val >> 12) & 1; *b->a[13] = (val >> 13) & 1; *b->a[14] = (val >> 14) & 1; *b->a[15] = (val >> 15) & 1;

#define SET_DATA(b, val) *b->d[0] = val & 1; *b->d[1] = (val >> 1) & 1; *b->d[2] = (val >> 2) & 1; *b->d[3] = (val >> 3) & 1; \
			 *b->d[4] = (val >> 4) & 1; *b->d[5] = (val >> 5) & 1; *b->d[6] = (val >> 6) & 1; *b->d[7] = (val >> 7) & 1;

struct board {
	// DEVICES
	struct a2clock    *clock;
	struct mk4116     *ram[3][8];
	struct rom2716    *rom[6];
	struct a2kbd      *kbd;
	struct mcs6502    *cpu;
	struct a2graphics *graphics;

	// BUSES
	bool *a[16];
	bool *d[8];

	// DECODER
	bool *ax;
	bool *clk_cas;
	bool *ra[7];

	// RAM
	bool *ras;
	bool *cas[3];
	bool *we;

	// ROM
	bool *ce[6];
	bool *oe[6];

	// KEYBOARD
	bool *kbd_data_sel;
	bool *kbd_strobe_sel;

	// CPU
	bool *phase0;
	bool *rdy;
	bool *irq;
	bool *nmi;
	bool *res;
	bool *cpu_rw;
	bool *cpu_d[8];

	// GRAPHICS
	bool *phase1;
	bool *va[16];
	bool *graphics_d[8];
};

bool *nets[10000];
int net_i = 0;

static struct termios tty_original;
static struct termios tty_raw;

void sigint_handler(int sig)
{
	write(1, "\033[?25h", 6);		// Handler-safe
	tcsetattr(0, TCSANOW, &tty_original);	// TCSANOW makes changes immediate
	if (sig == -1) {			// Success sentinel
		_exit(0);			// Handler-safe
	}
	signal(sig, SIG_DFL);
	raise(sig);
}

bool *net_create(void)
{
	nets[net_i] = calloc(1, sizeof(bool));
	return nets[net_i++];
}

void net_free_all(void)
{
	for (int i = net_i; i > 0; i--) {
		free(nets[i - 1]);
	}
}

u8 mux_row(bool *a[16])
{
	return (u8)*a[7] | ((u8)*a[2]<<1) | ((u8)*a[0]<<2) | ((u8)*a[8]<<3) | ((u8)*a[3]<<4) | ((u8)*a[1]<<5) | ((u8)*a[12]<<6);
}


u8 mux_col(bool *a[16])
{
	return (u8)*a[10] | ((u8)*a[4]<<1) | (( u8)*a[5]<<2) | ((u8)*a[11]<<3) | ((u8)*a[9]<<4) | ((u8)*a[6]<<5) | ((u8)*a[13]<<6);
}

struct board *board_create(bool trace)
{
	struct board *b = calloc(1, sizeof(*b));

	// DEVICES	
	b->clock = a2clock_create();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			b->ram[i][j] = mk4116_create();
		}
	}
	for (int i = 0; i < 6; i++) {
		b->rom[i] = rom2716_create(i + 1);
	}
	b->kbd      = a2kbd_create();
	b->cpu      = mcs6502_create(trace);
	b->graphics = a2graphics_create();

	// BUSES
	for (int i = 0; i < 16; i++) {
		b->a[i]  = net_create();
	}
	for (int i = 0; i < 8; i++) {
		b->d[i] = net_create();
	}

	// DECODER
	b->ax         = net_create();
	b->clk_cas    = net_create();
	b->clock->ax  = b->ax;
	b->clock->cas = b->clk_cas;
	for (int i = 0; i < 7; i++) {
		b->ra[i] = net_create();
	}

	// RAM
	b->ras = net_create();
	b->we = net_create();
	b->clock->ras = b->ras;
	*b->we = true;

	for (int i = 0; i < 3; i++) {
		b->cas[i]  = net_create();
		*b->cas[i] = true;
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 7; k++) {
				b->ram[i][j]->a[k] = b->ra[k];
			}
			b->ram[i][j]->ras  = b->ras;
			b->ram[i][j]->cas  = b->cas[i];
			b->ram[i][j]->we   = b->we;
			b->ram[i][j]->din  = b->d[j];
			b->ram[i][j]->dout = b->d[j];
		}
	}

	// ROM
	for (int i = 0; i < 6; i++) {
		b->ce[i] = net_create();
		b->oe[i] = net_create();
		b->rom[i]->ce  = b->ce[i];
		b->rom[i]->oe  = b->oe[i];
		for (int j = 0; j < 11; j++) {
			b->rom[i]->a[j] = b->a[j];
		}
		for (int j = 0; j < 8;  j++) {
		b->rom[i]->d[j] = b->d[j];
		}
	}

	// KEYBOARD
	for (int i = 0; i < 8; i++) {
		b->kbd->d[i] = b->d[i];
	}

	b->kbd_data_sel = net_create();
	b->kbd_strobe_sel = net_create();
	b->kbd->data_sel = b->kbd_data_sel;
	b->kbd->strobe_sel = b->kbd_strobe_sel;

	// CPU
	for (int i = 0; i < 16; i++) {
		b->cpu->a[i] = b->a[i];
	}
	for (int i = 0; i < 8; i++) {
		b->cpu_d[i] = net_create();
		b->cpu->d[i] = b->cpu_d[i];
	}

	b->irq  = net_create();
	b->nmi  = net_create();
	b->res  = net_create();
	b->rdy  = net_create();
	b->phase0 = net_create();
	b->cpu_rw = net_create();
	b->clock->phase0 = b->phase0;
	b->cpu->phi0 = b->phase0;
	b->cpu->irq = b->irq;
	b->cpu->nmi = b->nmi;
	b->cpu->res = b->res;
	b->cpu->rdy = b->rdy;
	b->cpu->rw  = b->cpu_rw;
	*b->irq = false;
	*b->nmi = false;
	*b->res = false;
	*b->rdy = true;
	*b->cpu_rw = true;

	// GRAPHICS
	for (int i = 0; i < 16; i++) {
		b->va[i] = net_create();
		b->graphics->a[i] = b->va[i];
	}
	for (int i = 0; i < 8; i++) {
		b->graphics_d[i] = net_create();
		b->graphics->d[i] = b->graphics_d[i];
	}
	b->phase1 = net_create();
	b->clock->phase1    = b->phase1;
	b->graphics->phase1 = b->phase1;
	return b;
}

void board_free(struct board *b)
{
	net_free_all();
	a2clock_free(b->clock);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			mk4116_free(b->ram[i][j]);
		}
	}
	for (int i = 0; i < 6; i++) {
		rom2716_free(b->rom[i]);
	}
	a2kbd_free(b->kbd);
	mcs6502_free(b->cpu);
	a2graphics_free(b->graphics);
}

void board_tick(struct board *b)
{
	a2clock_tick(b->clock);

	// DECODER
	for (int j = 0; j < 3; j++) {
		*b->cas[j] = true;
	}
	
	for (int i = 0; i < 6; i++) {
		*b->ce[i] = true;
		*b->oe[i] = true;
	}
	*b->kbd_data_sel = true;
	*b->kbd_strobe_sel = true;
	*b->we = *b->phase0 ? *b->cpu_rw : true;
	if (*b->phase0 ? *b->a[15] : *b->va[15]) {
		if (*b->phase0 ? !*b->a[14] : !*b->va[14]) {
			*b->cas[2] = *b->clk_cas;
		} else if (*b->phase0) {
			if (!*b->a[13] && *b->a[12]  && !*b->a[11]) {
				*b->ce[0] = false;
				*b->oe[0] = false;
			} else if (!*b->a[13] && *b->a[12]  && *b->a[11])  {
				*b->ce[1] = false;
				*b->oe[1] = false;
			} else if (*b->a[13]  && !*b->a[12] && !*b->a[11]) {
				*b->ce[2] = false;
				*b->oe[2] = false;
			} else if (*b->a[13]  && !*b->a[12] && *b->a[11])  {
				*b->ce[3] = false;
				*b->oe[3] = false;
			} else if (*b->a[13]  && *b->a[12]  && !*b->a[11])  {
				*b->ce[4] = false;
				*b->oe[4] = false;
			} else if (*b->a[13]  && *b->a[12]  && *b->a[11])  {
				*b->ce[5] = false;
				*b->oe[5] = false;
			} else if (!*b->a[13] && !*b->a[12] && !*b->a[11] && !*b->a[10] && !*b->a[9] && 
				!*b->a[8]  && !*b->a[7]  && !*b->a[6]  && !*b->a[5]  && !*b->a[4]) {
				*b->kbd_data_sel = false;
			} else if (!*b->a[13] && !*b->a[12] && !*b->a[11] && !*b->a[10] && !*b->a[9] && 
				!*b->a[8]  && !*b->a[7]  && !*b->a[6]  && !*b->a[5]) {
				*b->kbd_strobe_sel = false;
			}
		}
	} else {
		if (*b->phase0 ? *b->a[14] : *b->va[14]) {
			*b->cas[1] = *b->clk_cas;
		} else {
			*b->cas[0] = *b->clk_cas;
		}
	}

	if (*b->phase0) {
		for (int i = 0; i < 8; i++) {
			if (*b->cpu_rw) {
				*b->cpu_d[i] = *b->d[i];
			} else {
				*b->d[i] = *b->cpu_d[i];
			}
		}
	}

	if (*b->phase1) {
		for (int i = 0; i < 8; i++) {
		*b->graphics_d[i] = *b->d[i];
		}
	}

	if (*b->ax) {
		u8 row = mux_row(*b->phase0 ? b->a : b->va);
		for (int i = 0; i < 7; i++) {
			*b->ra[i] = (row >> i) & 1;
		}
	} else {
		u8 col = mux_col(*b->phase0 ? b->a : b->va);
		for (int i = 0; i < 7; i++) {
			*b->ra[i] = (col >> i) & 1;
		}
	}

	// RAM
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j< 8; j++) {
			mk4116_tick(b->ram[i][j]);
		}
	}

	// ROM
	for (int i = 0; i < 6; i++) {
		rom2716_tick(b->rom[i]);
	}

	// KEYBOARD
	a2kbd_tick(b->kbd);

	// CPU
	mcs6502_tick(b->cpu);

	// GRAPHICS
	a2graphics_tick(b->graphics);
}

int main(int argc, char *argv[])
{
	bool write = false;
	int  write_i = 0;
	int  write_len = 0;
	int kbd_delay = 400000;
	unsigned char *write_buffer;
	bool trace = false;
	bool measure = false;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--measure")) {
			measure = true;
		} else if (!strcmp(argv[i], "--trace")) {
			trace = true;
		} else if (!strcmp(argv[i], "--write")) {
			write = true;
			write_buffer = malloc(100000 * sizeof(unsigned char));
			FILE *f = fopen("write.txt", "rb");
			write_len = fread(write_buffer, sizeof(unsigned char), 100000, f);
			fclose(f);
		} else {
			fprintf(stderr, "invalid flag '%s'", argv[i]);
			exit(1);
		}
	}

	srand(time(NULL));
	signal(SIGINT, sigint_handler);

	tcgetattr(0, &tty_original);
	tty_raw = tty_original;
	tty_raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);	// ICANON disables line buffering
								// ECHO disables hsot echoing
								// ISIG and IEXTEN disable host interpretation of control codes
	tty_raw.c_iflag &= ~ICRNL;				// Makes the Enter key send \r instead of \n
	tty_raw.c_cc[VMIN] = 1;					// Dsiables buffering
	tty_raw.c_cc[VTIME] = 0;				// Disables timeout
	tcsetattr(0, TCSANOW, &tty_raw);			// TCSANOW makes changes immediate
	printf("\033[?25l");					// Stops host cursor blinking
	fcntl(0, F_SETFL, O_NONBLOCK);				// Disables read() blocking

	struct board *b = board_create(trace);

	// RESET SEQUENCE
	*b->res = true;
	board_tick(b);
	*b->res = false;

	// NORMAL OPERATION
	bool resetting = false;
	char ch;
	struct timespec ts0;
	struct timespec ts1;
	if (measure) {
		clock_gettime(CLOCK_MONOTONIC, &ts0);
	}

	for (u64 i = 0;; i++) {
		if (resetting) {
			*b->res = false;
			resetting = false;
		}
		if (!(i % kbd_delay)) {
			if (read(0, &ch, 1) == 1) {
				if ('a' <= ch && ch <= 'z') {
					a2kbd_press(b->kbd, ch - 'a' + 'A');
				} else if ('0' <= ch && ch <= '9') {
					a2kbd_press(b->kbd, ch);
				} else if (0x01 <= ch && ch <= 0x1B) {
					a2kbd_press(b->kbd, ch);
				} else {
					switch (ch) {
					case '\r':
					case ' ':
					case '!':
					case '"':
					case '#':
					case '$':
					case '%':
					case '&':
					case '\'':
					case '(':
					case ')':
					case '*':
					case '+':
					case ',':
					case '-':
					case '.':
					case '/':
					case ':':
					case ';':
					case '<':
					case '=':
					case '>':
					case '?':
					case '@':
						a2kbd_press(b->kbd, ch);	
						break;
					
					case '\\':
						*b->res = true;
						resetting = true;
						break;

					case '|':
						sigint_handler(SIGINT);
					
					default:
						break;
					}
				}
			} else if (i > 50000000 && write && (write_i != write_len)) {
				a2kbd_press(b->kbd, (write_buffer[write_i] == '\n') ? '\r' : write_buffer[write_i]);
				if (write_buffer[write_i] == '\n') {
					kbd_delay = 16000000;
				} else {
					kbd_delay = 400000;
				}
				write_i++;
			} else if (i > 50000000 && write) {
				kbd_delay = 400000;
			}
		}
		if (!(i % 200000)) {
			a2graphics_draw(b->graphics);
		}
		if (measure && i >= 14318180) {
			break;
		}
		board_tick(b);
	}

	clock_gettime(CLOCK_MONOTONIC, &ts1);
	printf("TIME ELAPSED in 14,318,180 ticks: %ld.%03ld sec\n", ts1.tv_sec - ts0.tv_sec - (ts1.tv_nsec < ts0.tv_nsec), (ts1.tv_nsec < ts0.tv_nsec) ? 1000 + ((ts1.tv_nsec - ts0.tv_nsec) / 1000000) : ((ts1.tv_nsec - ts0.tv_nsec) / 1000000));
	board_free(b);
	if (write) {
		free(write_buffer);
	}
	sigint_handler(-1); // Success sentinel
}