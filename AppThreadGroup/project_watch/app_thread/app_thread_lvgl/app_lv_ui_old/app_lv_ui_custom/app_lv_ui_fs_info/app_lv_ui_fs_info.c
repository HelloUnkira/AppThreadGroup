
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *text;
    char     *fs_info;
    uint32_t  fs_info_size;
} *app_lv_ui_res_local = NULL;

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_fs_info_append(char *str)
{
    APP_SYS_LOG_INFO(str);
    uint32_t fs_info_size_new = app_lv_ui_res_local->fs_info_size + strlen(str) + 1;
    app_lv_ui_res_local->fs_info = lv_mem_realloc(app_lv_ui_res_local->fs_info, fs_info_size_new + 1);
    lv_memset_00(app_lv_ui_res_local->fs_info + app_lv_ui_res_local->fs_info_size,
                 fs_info_size_new + 1 - app_lv_ui_res_local->fs_info_size);
    app_lv_ui_res_local->fs_info_size = fs_info_size_new;
    strcat(app_lv_ui_res_local->fs_info, str);
    strcat(app_lv_ui_res_local->fs_info, "\n");
    APP_SYS_LOG_DEBUG("");
    APP_SYS_LOG_DEBUG("%u", app_lv_ui_res_local->fs_info_size);
    APP_SYS_LOG_DEBUG_RAW(app_lv_ui_res_local->fs_info);
    APP_SYS_LOG_DEBUG("");
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_fs_walk(char path[256], char file[256])
{
    lv_fs_dir_t rddir;
    lv_fs_res_t retval;
    
    /* 扩充fs_info,将新目录追加填充进去 */
    app_lv_ui_fs_info_append(path);
    
    if ((retval = lv_fs_dir_open(&rddir, path)) != LV_FS_RES_OK) {
        APP_SYS_LOG_WARN("lv_fs_dir_open %s fail:%d", path, retval);
        return;
    }
    
    while (true) {
        strcpy(file, "");
        if ((retval = lv_fs_dir_read(&rddir, file)) != LV_FS_RES_OK) {
            APP_SYS_LOG_WARN("lv_fs_dir_read fail:%d", retval);
            break;
        }
        
        if (strcmp(file, "") == 0)
            break;
        if (strcmp(file, ".") == 0 ||  strcmp(file, "..") == 0)
            continue;
        
        /* 没有后缀为目录,有后缀则为文件 */
        if (strcmp(lv_fs_get_ext(file), "") == 0) {
            char path_next[256] = "";
            char file_next[256] = "";
            sprintf(path_next, "%s%s",  path, file);
            app_lv_ui_fs_walk(path_next, file_next);
        } else {
            char path_file[256] = "";
            sprintf(path_file, "%s/%s", path, file);
            /* 扩充fs_info,将新目录追加填充进去 */
            app_lv_ui_fs_info_append(path_file);
        }
    }
    
    if ((retval = lv_fs_dir_close(&rddir)) != LV_FS_RES_OK) {
        APP_SYS_LOG_WARN("lv_fs_dir_close %s fail:%d", path, retval);
        return;
    }
}

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btn_c_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        
        if (app_lv_ui_res_local->fs_info != NULL) {
            lv_mem_free(app_lv_ui_res_local->fs_info);
            app_lv_ui_res_local->fs_info_size = 0;
            app_lv_ui_res_local->fs_info = NULL;
        }
        
        /* 检查letter */
        if (!lv_fs_is_ready('S'))
            lv_label_set_text(app_lv_ui_res_local->text, "");
        else {
            char path_curr[256] = "S:";
            char file_curr[256] = "";
            app_lv_ui_fs_walk(path_curr, file_curr);
            
            lv_label_set_text(app_lv_ui_res_local->text,
                              app_lv_ui_res_local->fs_info != NULL ?
                              app_lv_ui_res_local->fs_info :  "");
            
            if (app_lv_ui_res_local->fs_info != NULL) {
                lv_mem_free(app_lv_ui_res_local->fs_info);
                app_lv_ui_res_local->fs_info_size = 0;
                app_lv_ui_res_local->fs_info = NULL;
            }
        }
        
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
}

/*@brief 界面显示
 *@param scene 场景
 */
static void app_lv_ui_fs_info_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 初始化标签,上中部 */
        lv_obj_t *label = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_label_set_text_static(label, "FS Info");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, app_lv_style_ver_pct(2));
        /* trace_text */
        lv_obj_t *text_box = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(text_box);
        lv_obj_set_style_bg_color(text_box, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
        lv_obj_set_size(text_box, app_lv_style_hor_pct(90), app_lv_style_ver_pct(63));
        lv_obj_align_to(text_box, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        app_lv_ui_res_local->text = app_lv_style_label(text_box);
        lv_obj_set_style_text_align(app_lv_ui_res_local->text, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_long_mode(app_lv_ui_res_local->text, LV_LABEL_LONG_WRAP);
        lv_label_set_text(app_lv_ui_res_local->text, "");
        /* 下部按键 */
        lv_obj_t *btn = NULL, *btn_c = NULL, *lbl_c = NULL;
        btn = app_lv_style_one_btn(app_lv_ui_res_local->scene, &btn_c, &lbl_c);
        lv_obj_add_event_cb(btn_c, app_lv_ui_btn_c_cb, LV_EVENT_CLICKED, NULL);
        lv_label_set_text(lbl_c, "Reflush");
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 1, 1000);
        /*  */
        app_lv_ui_res_local->fs_info_size = 0;
        app_lv_ui_res_local->fs_info = NULL;
    }
}

/*@brief 界面隐藏
 *@param scene 场景
 */
static void app_lv_ui_fs_info_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /*  */
        if (app_lv_ui_res_local->fs_info != NULL) {
            lv_mem_free(app_lv_ui_res_local->fs_info);
            app_lv_ui_res_local->fs_info_size = 0;
            app_lv_ui_res_local->fs_info = NULL;
        }
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_fs_info = {
    /* 场景资源节点 */
    .show = app_lv_ui_fs_info_show,
    .hide = app_lv_ui_fs_info_hide,
};
