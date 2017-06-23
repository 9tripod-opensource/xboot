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
	mir->pps = 30000;

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

static void scan_mirror_set_pixel(struct scan_mirror_t * mir, int16_t x0, int16_t y0)
{
	u32_t * p;
	u32_t c;
	int x, y, w, h;

	if(mir->blank)
		c = 0xff000000;
	else
		c = (0xff << 24) | (mir->r << 16) | (mir->g << 8) | (mir->b << 0);

	w = mir->width / 2;
	h = mir->height / 2;
	x = (float)x0 / 32768.0f * w + w;
	y = (float)(-y0) / 32768.0f * h + h;
	p = (u32_t *)((u8_t *)mir->pixels + (y * mir->width + x) * 4);
	*p = c;
}

static void scan_mirror_hline(struct scan_mirror_t * mir, int16_t x0, int16_t y0, int16_t x)
{
	int i;

	for(i = x0; i < x; i++)
	{
		scan_mirror_set_pixel(mir, i, y0);
	}
}

static void scan_mirror_vline(struct scan_mirror_t * mir, int16_t x0, int16_t y0, int16_t y)
{
	int i;

	for(i = y0; i < y; i++)
	{
		scan_mirror_set_pixel(mir, x0, i);
	}
}

static void scan_mirror_line(struct scan_mirror_t * mir, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
	int32_t   dx;						// 直线x轴差值变量
   	int32_t   dy;          			// 直线y轴差值变量
   	int8_t    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
   	int8_t    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
   	int32_t   dx_x2;					// dx*2值变量，用于加快运算速度
   	int32_t   dy_x2;					// dy*2值变量，用于加快运算速度
   	int32_t   di;						// 决策变量

   	dx = x2-x1;
   	dy = y2-y1;

   	if(dx>0)
   		dx_sym = 1;
   	else if(dx<0)
   	  	dx_sym = -1;
   	else
   	{
   		scan_mirror_vline(mir, x1,y1,y2);
      	return;
   	}

	if(dy>0)
   		dy_sym = 1;
   	else if(dy<0)
   		dy_sym = -1;
  	else
  	{
  		scan_mirror_hline(mir, x1,y1,x2);
      	return;
   	}

   	dx = dx_sym * dx;
   	dy = dy_sym * dy;
   	dx_x2 = dx*2;
   	dy_x2 = dy*2;

   	//使用Bresenham法进行画直线
   	if(dx>=dy)
   	{
   		di = dy_x2 - dx;
      	while(x1!=x2)
      	{
      		scan_mirror_set_pixel(mir, x1, y1);
         	x1 += dx_sym;
         	if(di<0)
         	{
         		di += dy_x2;
         	}
         	else
         	{
         		di += dy_x2 - dx_x2;
            	y1 += dy_sym;
         	}
      	}
      	scan_mirror_set_pixel(mir, x1, y1);
   	}
   	else
   	{
   		di = dx_x2 - dy;
      	while(y1!=y2)
      	{
      		scan_mirror_set_pixel(mir, x1, y1);
         	y1 += dy_sym;
         	if(di<0)
         	{
         		di += dx_x2;
         	}
         	else
         	{
         		di += dx_x2 - dy_x2;
            	x1 += dx_sym;
         	}
      	}
      	scan_mirror_set_pixel(mir, x1, y1);
   	}
}

static void scan_mirror_update(struct scan_mirror_t * mir)
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
	y = (float)(-mir->y) / 32768.0f * h + h;
	p = (u32_t *)((u8_t *)mir->pixels + (y * mir->width + x) * 4);
	*p = c;

	if(count++ > 30)
	{	count = 0;
		mir->fb->present(mir->fb, mir->fb->alone);
	}
}

void scan_mirror_set_pps(struct scan_mirror_t * mir, int pps)
{
	if(pps <= 0)
		pps = 30000;
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
	scan_mirror_line(mir, mir->x, mir->y, x, y);
	mir->x = x;
	mir->y = y;
	mir->z = z;
	scan_mirror_update(mir);
}
