/*
 * scan-mirror.c
 */

#include <scan-mirror.h>

struct scan_mirror_t * scan_mirror_alloc(const char * name)
{
	struct scan_mirror_t * mir;
	struct fb_t * fb = search_fb(name);

	if(!fb)
		return NULL;

	mir = malloc(sizeof(struct scan_mirror_t));
	if(!mir)
		return NULL;

	mir->x = 0;
	mir->y = 0;
	mir->z = 0;
	mir->r = 0;
	mir->g = 0;
	mir->b = 0;
	mir->blank = 0;
	mir->pps = 1000;

	mir->fb = fb;
	mir->pixels = fb->alone->pixels;
	mir->width = fb->alone->width;
	mir->height = fb->alone->height;
	scan_mirror_clear(mir);

	return mir;
}

void scan_mirror_free(struct scan_mirror_t * mir)
{
	if(mir)
		free(mir);
}

void scan_mirror_clear(struct scan_mirror_t * mir)
{
	uint32_t * p = mir->pixels;
	int i;

	for(i = 0; i < mir->width *  mir->height; i++)
		*p++ = 0xff000000;
	mir->fb->present(mir->fb, mir->fb->alone);
}

void scan_mirror_set_pps(struct scan_mirror_t * mir, int pps)
{
	if(pps <= 0)
		pps = 1000;
	mir->pps = pps;
}

void scan_mirror_set_color(struct scan_mirror_t * mir, uint8_t r, uint8_t g, uint8_t b, uint8_t blank)
{
	mir->r = r;
	mir->g = g;
	mir->b = b;
}

void scan_mirror_goto_xyz(struct scan_mirror_t * mir, int16_t x, int16_t y, int16_t z)
{
	mir->x = x;
	mir->y = y;
	mir->z = z;
}

void scan_mirror_update(struct scan_mirror_t * mir)
{
	static int count = 0;
	u32_t * p;
	u32_t c;
	int x, y, w, h;

	if(mir->blank)
		c = 0xff000000;
	else
		c = (0xff << 24) | (mir->r << 16) | (mir->g << 8) | (mir->b << 0);

	w = mir->width / 2;
	h = mir->height / 2;
	x = (float)mir->x / 32768.0f * w + w;
	y = (float)mir->y / 32768.0f * h + h;
	p = (u32_t *)((u8_t *)mir->pixels + (y * mir->width + x) * 4);
	*p = c;

	if(count++ > 30)
	{	count = 0;
		mir->fb->present(mir->fb, mir->fb->alone);
	}
}
