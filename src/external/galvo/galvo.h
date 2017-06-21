#ifndef __GALVO_H__
#define __GALVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>

struct galvo_ctx_t {
	u16_t x, y;
	u16_t r, g, b;

	struct fb_t * fb;
	void * pixels;
	int width;
	int height;
};

struct galvo_ctx_t * galvo_alloc(const char * name);
void galvo_free(struct galvo_ctx_t * ctx);
void galvo_set_xy(struct galvo_ctx_t * ctx, u16_t x, u16_t y);
void galvo_set_color(struct galvo_ctx_t * ctx, u16_t r, u16_t g, u16_t b);
void galvo_refresh(struct galvo_ctx_t * ctx);
void galvo_clear(struct galvo_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __GALVO_H__ */
