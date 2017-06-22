#ifndef __ILDA_H__
#define __ILDA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct ilda_point_t {
	float x, y, z;
	unsigned char r, g, b;
	int last;
	int blank;
};

struct ilda_file_t {
	int count;
	struct ilda_point_t * points;
};

struct ilda_file_t * ilda_alloc(char * buf, int len);
void ilda_free(struct ilda_file_t * ilda);

#ifdef __cplusplus
}
#endif

#endif /* __GALVO_H__ */
