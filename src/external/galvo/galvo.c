/*
 * galvo.c
 */

#include <galvo.h>

struct galvo_ctx_t * galvo_alloc(const char * name)
{
	struct galvo_ctx_t * ctx;
	struct fb_t * fb = search_fb(name);

	if(!fb)
		return NULL;

	ctx = malloc(sizeof(struct galvo_ctx_t));
	if(!ctx)
		return NULL;

	ctx->x = 0;
	ctx->y = 0;
	ctx->r = 0;
	ctx->g = 0;
	ctx->b = 0;
	ctx->fb = fb;
	ctx->pixels = fb->alone->pixels;
	ctx->width = fb->alone->width;
	ctx->height = fb->alone->height;

	memset(ctx->pixels, 0xff, (ctx->width * ctx->height * 4));
	ctx->fb->present(ctx->fb, ctx->fb->alone);
	return ctx;
}

void galvo_free(struct galvo_ctx_t * ctx)
{
	if(ctx)
		free(ctx);
}

void galvo_set_xy(struct galvo_ctx_t * ctx, u16_t x, u16_t y)
{
	ctx->x = x;
	ctx->y = y;
}

void galvo_set_color(struct galvo_ctx_t * ctx, u16_t r, u16_t g, u16_t b)
{
	ctx->r = r;
	ctx->g = g;
	ctx->b = b;
}

void galvo_refresh(struct galvo_ctx_t * ctx)
{
	u32_t c = (0xff << 24) | ((ctx->r >> 8) << 16) | ((ctx->g >> 8) << 8) | ((ctx->b >> 8) << 0);
	u32_t * p = (u32_t *)((u8_t *)ctx->pixels + (ctx->y * ctx->width + ctx->x) * 4);
	*p = c;
	ctx->fb->present(ctx->fb, ctx->fb->alone);
}

void galvo_clear(struct galvo_ctx_t * ctx)
{
	ctx->x = 0;
	ctx->y = 0;
	ctx->r = 0;
	ctx->g = 0;
	ctx->b = 0;
	memset(ctx->pixels, 0xff, (ctx->width * ctx->height * 4));
	ctx->fb->present(ctx->fb, ctx->fb->alone);
}
