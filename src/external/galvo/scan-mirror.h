#ifndef __SCAN_MIRROR_H__
#define __SCAN_MIRROR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>

struct scan_mirror_t {
	int16_t x, y, z;
	uint8_t r, g, b, blank;
	int pps;

	struct fb_t * fb;
	void * pixels;
	int width;
	int height;
};

struct scan_mirror_t * scan_mirror_alloc(const char * name);
void scan_mirror_free(struct scan_mirror_t * mir);
void scan_mirror_clear(struct scan_mirror_t * mir);

void scan_mirror_set_pps(struct scan_mirror_t * mir, int pps);
void scan_mirror_set_color(struct scan_mirror_t * mir, uint8_t r, uint8_t g, uint8_t b, uint8_t blank);
void scan_mirror_goto_xyz(struct scan_mirror_t * mir, int16_t x, int16_t y, int16_t z);

#ifdef __cplusplus
}
#endif

#endif /* __SCAN_MIRROR_H__ */
