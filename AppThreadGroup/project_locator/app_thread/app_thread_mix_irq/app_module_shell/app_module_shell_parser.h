#ifndef APP_MODULE_SHELL_PARSER_H
#define APP_MODULE_SHELL_PARSER_H

#include <stdint.h>
#include <stdbool.h>

/*@brief 命令选项描述
 *        注册命令时附带, 解析器自动处理 -h/--help 等
 */
typedef struct {
    const char *opt_long;       /* 长选项名, 如 "help" */
    char        opt_short;      /* 短选项字母, 如 'h' */
    bool        has_arg;        /* 是否需要参数 (如 --file path) */
    const char *desc;           /* 选项描述文本 */
} app_module_shell_opt_t;

/*@brief 命令描述
 */
typedef struct {
    const char *name;           /* 命令名称 */
    const char *usage;          /* 用法摘要, 如 "echo <text...>" */
    const char *desc;           /* 命令描述 */
    const app_module_shell_opt_t *opts;   /* 选项表, NULL 结尾 */
    void (*handler)(int argc, char **argv, void *priv);
    void *priv;                 /* 私有数据, 透传给 handler */
} app_module_shell_cmd_t;

/*@brief shell 解析器初始化
 */
void app_module_shell_parser_ready(void);

/*@brief 注册一个命令
 *@param cmd 命令描述 (调用者保持生命期)
 *@retval true  注册成功
 *@retval false 命令表已满或重名
 */
bool app_module_shell_parser_register(const app_module_shell_cmd_t *cmd);

/*@brief 解析并执行一行输入
 *@param line  输入的原始行 (会被修改, strtok 风格)
 *@param write 输出回调 (写回控制台)
 *@param priv  透传给 write 回调
 */
void app_module_shell_parser_exec(char *line,
    void (*write)(const char *buf, uint32_t size, void *priv),
    void *priv);

#endif
