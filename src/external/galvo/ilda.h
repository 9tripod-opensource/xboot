#ifndef __ILDA_H__
#define __ILDA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <scan-mirror.h>

void ilda_load_file(const char * file, struct scan_mirror_t * mir);

#ifdef __cplusplus
}
#endif

#endif /* __GALVO_H__ */
