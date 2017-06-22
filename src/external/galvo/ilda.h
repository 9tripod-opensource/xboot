#ifndef __ILDA_H__
#define __ILDA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <galvo.h>

void ilda_load_file(struct galvo_ctx_t * gctx, const char * file);

#ifdef __cplusplus
}
#endif

#endif /* __GALVO_H__ */
