#ifndef SCUI_DRAW_THORVG_H
#define SCUI_DRAW_THORVG_H

#ifdef __cplusplus
extern "C" {
#endif

/* 矢量绘图引擎 */
#define SCUI_DRAW_USE_THORVG_SRC                    (1)

void scui_draw_thorvg_assert(void *ptr);
void * scui_draw_thorvg_alloc(int size);
void * scui_draw_thorvg_zalloc(int size);
void * scui_draw_thorvg_realloc(void *ptr, int new_size);
char * scui_draw_thorvg_strdup(const char * src);
void scui_draw_thorvg_free(void *ptr);

#ifndef PRIu32
#define PRIu32  "I32u"
#endif

#ifdef __cplusplus
}
#endif

#endif
