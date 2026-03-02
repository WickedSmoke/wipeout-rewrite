#include "system.h"
#include "input.h"
#include "render.h"
#include "platform.h"
#include "mem.h"
#include "utils.h"

#include "wipeout/game.h"

static double time_real;
static double time_scaled;
#ifdef LOCK_30_FPS
static const double tick_last = 0.03333333333333333;
#else
static double tick_last;
#endif
static double cycle_time = 0;
static double frame_t0;
static unsigned int frame_count;

void system_init(void) {
	time_real = platform_now();
	frame_t0 = time_real;
	frame_count = 0;
	time_scaled = 0.0;

	input_init();
	render_init(platform_screen_size());
	game_init();
}

void system_cleanup(void) {
	render_cleanup();
	input_cleanup();
}

void system_exit(void) {
	platform_exit();
}

void system_update(double now) {
#ifndef LOCK_30_FPS
	double real_delta = now - time_real;
	time_real = now;
	tick_last = min(real_delta, 0.1);
#endif

	if ((++frame_count & 7) == 0) {
		g.frame_rate = 8.0 / (now - frame_t0);
		frame_t0 = now;
	}
	time_scaled += tick_last;

	// FIXME: come up with a better way to wrap the cycle_time, so that it
	// doesn't lose precission, but also doesn't jump upon reset.
	cycle_time = time_scaled;
	if (cycle_time > 3600 * M_PI) {
		cycle_time -= 3600 * M_PI;
	}
	
	render_frame_prepare();
	
	game_update();

	render_frame_end();
	input_clear();
	mem_temp_check();
}

void system_reset_cycle_time(void) {
	cycle_time = 0;
}

void system_resize(vec2i_t size) {
	render_set_screen_size(size);
}

double system_tick(void) {
	return tick_last;
}

double system_time(void) {
	return time_scaled;
}

double system_cycle_time(void) {
	return cycle_time;
}
