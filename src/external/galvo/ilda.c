/*
 * ilda.c
 */

#include <ilda.h>

struct ilda_hdr_t {
	uint8_t magic[4];
	uint8_t pad[3];
	uint8_t format;
	uint8_t name[8];
	uint8_t company[8];
	uint8_t count[2];
	uint8_t frameno[2];
	uint8_t framecount[2];
	uint8_t scanner;
	uint8_t reseved;
} __attribute__ ((packed));

static uint16_t value_of_two_bytes(uint8_t * buf)
{
	return (uint16_t)((buf[0] << 8) | buf[1]);
}

struct ilda_file_t * ilda_alloc(char * buf, int len)
{
	struct ilda_hdr_t hdr;
	struct ilda_file_t * ilda;
	uint8_t * p;
	int count, frameno, framecount;
	int i;

	if(!buf || len < sizeof(struct ilda_hdr_t))
		return NULL;

	ilda = malloc(sizeof(struct ilda_file_t));
	if(!ilda)
		return NULL;
	memset(ilda, 0, sizeof(struct ilda_file_t));

	while(!ilda->count)
	{
		memcpy(&hdr, buf, sizeof(struct ilda_hdr_t));
		if(hdr.magic[0] != 'I' || hdr.magic[1] != 'L' || hdr.magic[2] != 'D' || hdr.magic[3] != 'A')
		{
			free(ilda);
			return NULL;
		}

		count = value_of_two_bytes(hdr.count);
		frameno = value_of_two_bytes(hdr.frameno);
		framecount = value_of_two_bytes(hdr.framecount);

		switch(hdr.format)
		{
		case 0:
			ilda->points = malloc(sizeof(struct ilda_point_t) * count);
			p = (uint8_t *)buf + sizeof(struct ilda_hdr_t);
			for(i = 0; i < count; i++)
			{
				ilda->points[i].x = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].y = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].z = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				if(*p && (1 << 7))
					ilda->points[i].last = 1;
				else
					ilda->points[i].last = 0;
				if(*p && (1 << 6))
					ilda->points[i].blank = 1;
				else
					ilda->points[i].blank = 0;
				p += 1;
				ilda->points[i].r = *p;
				ilda->points[i].g = *p;
				ilda->points[i].b = *p;
				p += 1;
			}
			ilda->count = count;
			break;

		case 1:
			ilda->points = malloc(sizeof(struct ilda_point_t) * count);
			p = (uint8_t *)buf + sizeof(struct ilda_hdr_t);
			for(i = 0; i < count; i++)
			{
				ilda->points[i].x = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].y = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].z = 0;
				if(*p && (1 << 7))
					ilda->points[i].last = 1;
				else
					ilda->points[i].last = 0;
				if(*p && (1 << 6))
					ilda->points[i].blank = 1;
				else
					ilda->points[i].blank = 0;
				p += 1;
				ilda->points[i].r = *p;
				ilda->points[i].g = *p;
				ilda->points[i].b = *p;
				p += 1;
			}
			ilda->count = count;
			break;

		case 2:
			break;

		case 4:
			ilda->points = malloc(sizeof(struct ilda_point_t) * count);
			p = (uint8_t *)buf + sizeof(struct ilda_hdr_t);
			for(i = 0; i < count; i++)
			{
				ilda->points[i].x = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].y = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].z = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				if(*p && (1 << 7))
					ilda->points[i].last = 1;
				else
					ilda->points[i].last = 0;
				if(*p && (1 << 6))
					ilda->points[i].blank = 1;
				else
					ilda->points[i].blank = 0;
				p += 1;
				ilda->points[i].b = *p;
				p += 1;
				ilda->points[i].g = *p;
				p += 1;
				ilda->points[i].r = *p;
				p += 1;
			}
			ilda->count = count;
			break;

		case 5:
			ilda->points = malloc(sizeof(struct ilda_point_t) * count);
			p = (uint8_t *)buf + sizeof(struct ilda_hdr_t);
			for(i = 0; i < count; i++)
			{
				ilda->points[i].x = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].y = (int16_t)value_of_two_bytes(p) / 32768.0f;
				p += 2;
				ilda->points[i].z = 0;
				if(*p && (1 << 7))
					ilda->points[i].last = 1;
				else
					ilda->points[i].last = 0;
				if(*p && (1 << 6))
					ilda->points[i].blank = 1;
				else
					ilda->points[i].blank = 0;
				p += 1;
				ilda->points[i].b = *p;
				p += 1;
				ilda->points[i].g = *p;
				p += 1;
				ilda->points[i].r = *p;
				p += 1;
			}
			ilda->count = count;
			break;

		default:
			break;
		}
	}

	return ilda;
}

void ilda_free(struct ilda_file_t * ilda)
{
	if(ilda)
	{
		if(ilda->points)
			free(ilda->points);
		free(ilda);
	}
}
