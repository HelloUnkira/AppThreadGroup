#ifndef SCUI_CWF_JSON_PARSER_H
#define SCUI_CWF_JSON_PARSER_H

/*约定(小端序, 1字节对齐): 
 *    本文件需要与 scui_cwf_json_parser.json 文件同步一致
 *    固件必须与json中版本号一并对应, 版本不匹配的云表盘不支持使用
 */

// 头部信息
// uint8_t  version[4];        // cwf版本号
// uint32_t json_ofs;          // json文件偏移
// uint32_t json_size;         // json文件大小
// uint32_t image_iofs;        // image info偏移
// uint32_t image_isize;       // image info大小
// uint32_t image_dofs;        // image data偏移
// uint32_t image_dsize;       // image data大小
// 总计: 28字节

// 单个 image info 约定
// uint8_t  format;  --> scui_cwf_json_format_t 需要转译成 scui_pixel_cf_t 类型
// uint8_t  type;    --> scui_image_type_t
// uint32_t height;  --> 转译成uintptr_t
// uint32_t width;   --> 转译成uintptr_t
// uint32_t size;    --> 转译成uintptr_t
// uint32_t data;    --> 转译成uintptr_t
// 总计: 18字节

#define SCUI_CWF_JSON_NAME      256

typedef struct {
    scui_handle_t  name;
    scui_image_t  *image_src;
    scui_handle_t *image_hit;
    uint32_t       image_num;
    scui_handle_t  parent;
    scui_handle_t *list_child;
    uint32_t       list_num;
    uint8_t       *list_type;
    uint8_t       *list_type_sub;
    void         **list_src;
} scui_cwf_json_parser_t;

/*@brief 更新cwf
 *@param inst 实例
 */
void scui_cwf_json_anim(void *inst);

/*@brief 销毁cwf
 *@param inst 实例地址
 */
void scui_cwf_json_burn(void **inst);

/*@brief 构建cwf
 *@param inst   实例地址
 *@param file   cwf名字
 *@param parent 父控件
 */
void scui_cwf_json_make(void **inst, const char *file, scui_handle_t parent);

#endif
