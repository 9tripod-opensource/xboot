/*
 * ilda.c
 */

#include <ilda.h>

enum ilda_state_t {
	ILDA_STATE_MAGIC				= 0,
	ILDA_STATE_RESERVED1			= 1,
	ILDA_STATE_FORMAT				= 2,
	ILDA_STATE_NAME					= 3,
	ILDA_STATE_COMPANY				= 4,
	ILDA_STATE_COUNT				= 5,
	ILDA_STATE_FRAME_NUM			= 6,
	ILDA_STATE_FRAME_CNT			= 7,
	ILDA_STATE_SCANNER				= 8,
	ILDA_STATE_RESERVED2			= 9,
	ILDA_STATE_RECORD				= 10,
};

enum ilda_coord_format_t {
	ILDA_COORD_FORMAT_3D_INDEX		= 0,
	ILDA_COORD_FORMAT_2D_INDEX		= 1,
	ILDA_COORD_FORMAT_PALETTE_COLOR	= 2,
	ILDA_COORD_FORMAT_3D_TRUE_COLOR	= 4,
	ILDA_COORD_FORMAT_2D_TRUE_COLOR	= 5,
};

struct ilda_ctx_t {
	enum ilda_state_t state;
	uint8_t buf[32];
	int index;
	enum ilda_coord_format_t format;
	int count;
	int fnum;
	int fcnt;
};

struct ilda_point_t {
	int16_t x, y, z;
	uint8_t r, g, b;
	int index;
	int last;
	int blank;
};

static const uint8_t ilda_default_palette[256][3] = {
	{ 0, 0, 0 },
	{ 255, 255, 255 },
	{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 255, 255 },
	{ 0, 0, 255 },
	{ 255, 0, 255 },
	{ 255, 128, 128 },
	{ 255, 140, 128 },
	{ 255, 151, 128 },
	{ 255, 163, 128 },
	{ 255, 174, 128 },
	{ 255, 186, 128 },
	{ 255, 197, 128 },
	{ 255, 209, 128 },
	{ 255, 220, 128 },
	{ 255, 232, 128 },
	{ 255, 243, 128 },
	{ 255, 255, 128 },
	{ 243, 255, 128 },
	{ 232, 255, 128 },
	{ 220, 255, 128 },
	{ 209, 255, 128 },
	{ 197, 255, 128 },
	{ 186, 255, 128 },
	{ 174, 255, 128 },
	{ 163, 255, 128 },
	{ 151, 255, 128 },
	{ 140, 255, 128 },
	{ 128, 255, 128 },
	{ 128, 255, 140 },
	{ 128, 255, 151 },
	{ 128, 255, 163 },
	{ 128, 255, 174 },
	{ 128, 255, 186 },
	{ 128, 255, 197 },
	{ 128, 255, 209 },
	{ 128, 255, 220 },
	{ 128, 255, 232 },
	{ 128, 255, 243 },
	{ 128, 255, 255 },
	{ 128, 243, 255 },
	{ 128, 232, 255 },
	{ 128, 220, 255 },
	{ 128, 209, 255 },
	{ 128, 197, 255 },
	{ 128, 186, 255 },
	{ 128, 174, 255 },
	{ 128, 163, 255 },
	{ 128, 151, 255 },
	{ 128, 140, 255 },
	{ 128, 128, 255 },
	{ 140, 128, 255 },
	{ 151, 128, 255 },
	{ 163, 128, 255 },
	{ 174, 128, 255 },
	{ 186, 128, 255 },
	{ 197, 128, 255 },
	{ 209, 128, 255 },
	{ 220, 128, 255 },
	{ 232, 128, 255 },
	{ 243, 128, 255 },
	{ 255, 128, 255 },
	{ 255, 128, 243 },
	{ 255, 128, 232 },
	{ 255, 128, 220 },
	{ 255, 128, 209 },
	{ 255, 128, 197 },
	{ 255, 128, 186 },
	{ 255, 128, 174 },
	{ 255, 128, 163 },
	{ 255, 128, 151 },
	{ 255, 128, 140 },
	{ 255, 0, 0 },
	{ 255, 23, 0 },
	{ 255, 46, 0 },
	{ 255, 70, 0 },
	{ 255, 93, 0 },
	{ 255, 116, 0 },
	{ 255, 139, 0 },
	{ 255, 162, 0 },
	{ 255, 185, 0 },
	{ 255, 209, 0 },
	{ 255, 232, 0 },
	{ 255, 255, 0 },
	{ 232, 255, 0 },
	{ 209, 255, 0 },
	{ 185, 255, 0 },
	{ 162, 255, 0 },
	{ 139, 255, 0 },
	{ 116, 255, 0 },
	{ 93, 255, 0 },
	{ 70, 255, 0 },
	{ 46, 255, 0 },
	{ 23, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 255, 23 },
	{ 0, 255, 46 },
	{ 0, 255, 70 },
	{ 0, 255, 93 },
	{ 0, 255, 116 },
	{ 0, 255, 139 },
	{ 0, 255, 162 },
	{ 0, 255, 185 },
	{ 0, 255, 209 },
	{ 0, 255, 232 },
	{ 0, 255, 255 },
	{ 0, 232, 255 },
	{ 0, 209, 255 },
	{ 0, 185, 255 },
	{ 0, 162, 255 },
	{ 0, 139, 255 },
	{ 0, 116, 255 },
	{ 0, 93, 255 },
	{ 0, 70, 255 },
	{ 0, 46, 255 },
	{ 0, 23, 255 },
	{ 0, 0, 255 },
	{ 23, 0, 255 },
	{ 46, 0, 255 },
	{ 70, 0, 255 },
	{ 93, 0, 255 },
	{ 116, 0, 255 },
	{ 139, 0, 255 },
	{ 162, 0, 255 },
	{ 185, 0, 255 },
	{ 209, 0, 255 },
	{ 232, 0, 255 },
	{ 255, 0, 255 },
	{ 255, 0, 232 },
	{ 255, 0, 209 },
	{ 255, 0, 185 },
	{ 255, 0, 162 },
	{ 255, 0, 139 },
	{ 255, 0, 116 },
	{ 255, 0, 93 },
	{ 255, 0, 70 },
	{ 255, 0, 46 },
	{ 255, 0, 23 },
	{ 128, 0, 0 },
	{ 128, 12, 0 },
	{ 128, 23, 0 },
	{ 128, 35, 0 },
	{ 128, 47, 0 },
	{ 128, 58, 0 },
	{ 128, 70, 0 },
	{ 128, 81, 0 },
	{ 128, 93, 0 },
	{ 128, 105, 0 },
	{ 128, 116, 0 },
	{ 128, 128, 0 },
	{ 116, 128, 0 },
	{ 105, 128, 0 },
	{ 93, 128, 0 },
	{ 81, 128, 0 },
	{ 70, 128, 0 },
	{ 58, 128, 0 },
	{ 47, 128, 0 },
	{ 35, 128, 0 },
	{ 23, 128, 0 },
	{ 12, 128, 0 },
	{ 0, 128, 0 },
	{ 0, 128, 12 },
	{ 0, 128, 23 },
	{ 0, 128, 35 },
	{ 0, 128, 47 },
	{ 0, 128, 58 },
	{ 0, 128, 70 },
	{ 0, 128, 81 },
	{ 0, 128, 93 },
	{ 0, 128, 105 },
	{ 0, 128, 116 },
	{ 0, 128, 128 },
	{ 0, 116, 128 },
	{ 0, 105, 128 },
	{ 0, 93, 128 },
	{ 0, 81, 128 },
	{ 0, 70, 128 },
	{ 0, 58, 128 },
	{ 0, 47, 128 },
	{ 0, 35, 128 },
	{ 0, 23, 128 },
	{ 0, 12, 128 },
	{ 0, 0, 128 },
	{ 12, 0, 128 },
	{ 23, 0, 128 },
	{ 35, 0, 128 },
	{ 47, 0, 128 },
	{ 58, 0, 128 },
	{ 70, 0, 128 },
	{ 81, 0, 128 },
	{ 93, 0, 128 },
	{ 105, 0, 128 },
	{ 116, 0, 128 },
	{ 128, 0, 128 },
	{ 128, 0, 116 },
	{ 128, 0, 105 },
	{ 128, 0, 93 },
	{ 128, 0, 81 },
	{ 128, 0, 70 },
	{ 128, 0, 58 },
	{ 128, 0, 47 },
	{ 128, 0, 35 },
	{ 128, 0, 23 },
	{ 128, 0, 12 },
	{ 255, 192, 192 },
	{ 255, 64, 64 },
	{ 192, 0, 0 },
	{ 64, 0, 0 },
	{ 255, 255, 192 },
	{ 255, 255, 64 },
	{ 192, 192, 0 },
	{ 64, 64, 0 },
	{ 192, 255, 192 },
	{ 64, 255, 64 },
	{ 0, 192, 0 },
	{ 0, 64, 0 },
	{ 192, 255, 255 },
	{ 64, 255, 255 },
	{ 0, 192, 192 },
	{ 0, 64, 64 },
	{ 192, 192, 255 },
	{ 64, 64, 255 },
	{ 0, 0, 192 },
	{ 0, 0, 64 },
	{ 255, 192, 255 },
	{ 255, 64, 255 },
	{ 192, 0, 192 },
	{ 64, 0, 64 },
	{ 255, 96, 96 },
	{ 255, 255, 255 },
	{ 245, 245, 245 },
	{ 235, 235, 235 },
	{ 224, 224, 224 },
	{ 213, 213, 213 },
	{ 203, 203, 203 },
	{ 192, 192, 192 },
	{ 181, 181, 181 },
	{ 171, 171, 171 },
	{ 160, 160, 160 },
	{ 149, 149, 149 },
	{ 139, 139, 139 },
	{ 128, 128, 128 },
	{ 117, 117, 117 },
	{ 107, 107, 107 },
	{ 96, 96, 96 },
	{ 85, 85, 85 },
	{ 75, 75, 75 },
	{ 64, 64, 64 },
	{ 53, 53, 53 },
	{ 43, 43, 43 },
	{ 32, 32, 32 },
	{ 21, 21, 21 },
	{ 11, 11, 11 },
	{ 0, 0, 0 }
};

static inline uint16_t value_of_2bytes(uint8_t * buf)
{
	return (uint16_t)((buf[0] << 8) | buf[1]);
}

static inline void ilda_push_byte(struct ilda_ctx_t * ctx, struct scan_mirror_t * mir, char byte)
{
	struct ilda_point_t point;

	switch(ctx->state)
	{
	case ILDA_STATE_MAGIC:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 4)
		{
			if(ctx->buf[0] == 'I' && ctx->buf[1] == 'L' && ctx->buf[2] == 'D' && ctx->buf[3] == 'A')
			{
				ctx->state = ILDA_STATE_RESERVED1;
				ctx->index = 0;
			}
			else
			{
				ctx->state = ILDA_STATE_MAGIC;
				ctx->index = 0;
			}
		}
		break;

	case ILDA_STATE_RESERVED1:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 3)
		{
			ctx->state = ILDA_STATE_FORMAT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FORMAT:
		ctx->format = (enum ilda_coord_format_t)byte;
		ctx->state = ILDA_STATE_NAME;
		ctx->index = 0;
		break;

	case ILDA_STATE_NAME:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 8)
		{
			ctx->state = ILDA_STATE_COMPANY;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_COMPANY:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 8)
		{
			ctx->state = ILDA_STATE_COUNT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_COUNT:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->count = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_FRAME_NUM;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_NUM:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->fnum = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_FRAME_CNT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_CNT:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->fcnt = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_SCANNER;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_SCANNER:
		ctx->state = ILDA_STATE_RESERVED2;
		ctx->index = 0;
		break;

	case ILDA_STATE_RESERVED2:
		if(ctx->count > 0)
		{
			ctx->state = ILDA_STATE_RECORD;
			ctx->index = 0;
		}
		else
		{
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
		}
		scan_mirror_clear(mir);
		break;

	case ILDA_STATE_RECORD:
		ctx->buf[ctx->index++] = byte;
		switch(ctx->format)
		{
		case ILDA_COORD_FORMAT_3D_INDEX:
			if(ctx->index >= 8)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0]));
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2]));
				point.z = (int16_t)(value_of_2bytes(&ctx->buf[4]));
				if(ctx->buf[6] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ctx->buf[6] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.index = ctx->buf[7];
				point.r = ilda_default_palette[point.index][0];
				point.g = ilda_default_palette[point.index][1];
				point.b = ilda_default_palette[point.index][2];

				scan_mirror_set_color(mir, point.r, point.g, point.b, point.blank);
				scan_mirror_goto_xyz(mir, point.x, point.y, point.z);

				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
				ctx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_2D_INDEX:
			if(ctx->index >= 6)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0]));
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2]));
				point.z = 0;
				if(ctx->buf[4] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ctx->buf[4] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.index = ctx->buf[5];
				point.r = ilda_default_palette[point.index][0];
				point.g = ilda_default_palette[point.index][1];
				point.b = ilda_default_palette[point.index][2];

				scan_mirror_set_color(mir, point.r, point.g, point.b, point.blank);
				scan_mirror_goto_xyz(mir, point.x, point.y, point.z);

				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
				ctx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_PALETTE_COLOR:
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
			break;

		case ILDA_COORD_FORMAT_3D_TRUE_COLOR:
			if(ctx->index >= 10)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0]));
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2]));
				point.z = (int16_t)(value_of_2bytes(&ctx->buf[4]));
				if(ctx->buf[6] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ctx->buf[6] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.b = ctx->buf[7];
				point.g = ctx->buf[8];
				point.r = ctx->buf[9];

				scan_mirror_set_color(mir, point.r, point.g, point.b, point.blank);
				scan_mirror_goto_xyz(mir, point.x, point.y, point.z);

				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
				ctx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_2D_TRUE_COLOR:
			if(ctx->index >= 8)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0]));
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2]));
				point.z = 0;
				if(ctx->buf[4] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ctx->buf[4] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.b = ctx->buf[5];
				point.g = ctx->buf[6];
				point.r = ctx->buf[7];

				scan_mirror_set_color(mir, point.r, point.g, point.b, point.blank);
				scan_mirror_goto_xyz(mir, point.x, point.y, point.z);

				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
				ctx->count--;
			}
			break;

		default:
			break;
		}

		if(ctx->count <= 0)
		{
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
		}
		break;

	default:
		ctx->state = ILDA_STATE_MAGIC;
		ctx->index = 0;
		break;
	}
}

void ilda_load_file(const char * file, struct scan_mirror_t * mir)
{
	struct ilda_ctx_t ctx;
	char buf[512];
	int fd, n, i;

	if(!file || !mir)
		return;

	memset(&ctx, 0, sizeof(struct ilda_ctx_t));
	ctx.state = ILDA_STATE_MAGIC;
	ctx.index = 0;

	fd = open(file, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return;

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		for(i = 0; i < n; i++)
		{
			ilda_push_byte(&ctx, mir, buf[i]);
		}
	}

	close(fd);
}
