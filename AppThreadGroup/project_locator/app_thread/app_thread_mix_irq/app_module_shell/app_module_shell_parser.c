/* 实现目标: 泛用 shell 解析器, 支持命令注册/选项拆分/引号解析 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_module_shell_parser.h"

#define APP_MODULE_SHELL_PARSER_ARGV_MAX  (32)
#define APP_MODULE_SHELL_PARSER_CMD_MAX   (32)

/* 命令表 */
static const app_module_shell_cmd_t *app_module_shell_cmd_tbl[APP_MODULE_SHELL_PARSER_CMD_MAX] = {0};
static uint32_t app_module_shell_cmd_cnt = 0;

/* 当前 exec 的 write 回调 (供内置命令使用) */
static void (*exec_write)(const char *, uint32_t, void *) = NULL;
static void  *exec_write_priv = NULL;

/*@brief 辅助: 从字符串找到第一个非空白字符
 */
static const char *skip_blank(const char *p)
{
    while (*p == ' ' || *p == '\t')
        p++;
    return p;
}

/*@brief 辅助: 写入一行到控制台
 */
static void parser_puts(const char *s)
{
    exec_write(s, strlen(s), exec_write_priv);
    exec_write("\r\n", 2, exec_write_priv);
}

/*@brief 打印命令的帮助信息 (自动处理 -h/--help)
 */
static void print_help(const app_module_shell_cmd_t *cmd)
{
    parser_puts("");

    /* 用法行 */
    char buf[128];
    int n = snprintf(buf, sizeof(buf), "Usage: %s %s", cmd->name,
                     cmd->usage ? cmd->usage : "");
    if (n < 0) n = 0; else if ((uint32_t)n >= sizeof(buf)) n = sizeof(buf) - 1;
    buf[n] = '\0';
    parser_puts(buf);

    /* 描述 */
    if (cmd->desc) {
        snprintf(buf, sizeof(buf), "  %s", cmd->desc);
        parser_puts(buf);
    }

    /* 选项表 */
    if (cmd->opts) {
        parser_puts("Options:");
        for (const app_module_shell_opt_t *o = cmd->opts; o->opt_long != NULL; o++) {
            char line[128];
            int pos = 0;

            if (o->opt_short)
                pos = snprintf(line, sizeof(line), "  -%c, --%s", o->opt_short, o->opt_long);
            else
                pos = snprintf(line, sizeof(line), "      --%s", o->opt_long);

            if (o->has_arg)
                pos += snprintf(line + pos, sizeof(line) - pos, " <arg>");

            if (o->desc)
                snprintf(line + pos, sizeof(line) - pos, "   %s", o->desc);

            parser_puts(line);
        }
    }
}

/*@brief 通用 help 命令
 */
static void cmd_help_handler(int argc, char **argv, void *priv)
{
    (void)priv;

    if (argc > 1) {
        /* 查找指定命令的帮助 */
        for (uint32_t i = 0; i < app_module_shell_cmd_cnt; i++) {
            if (strcmp(app_module_shell_cmd_tbl[i]->name, argv[1]) == 0) {
                print_help(app_module_shell_cmd_tbl[i]);
                return;
            }
        }
        parser_puts("unknown command");
        return;
    }

    parser_puts("Available commands:");
    for (uint32_t i = 0; i < app_module_shell_cmd_cnt; i++) {
        const app_module_shell_cmd_t *cmd = app_module_shell_cmd_tbl[i];
        char buf[128];
        int n = snprintf(buf, sizeof(buf), "  %s", cmd->name);
        if (cmd->desc)
            snprintf(buf + n, sizeof(buf) - n, "  - %s", cmd->desc);
        parser_puts(buf);
    }
}

/* 内置 help 命令 */
static const app_module_shell_opt_t cmd_help_opts[] = {
    {.opt_long = NULL, .opt_short = 0, .has_arg = false, .desc = NULL},
};

static const app_module_shell_cmd_t cmd_help = {
    .name    = "help",
    .usage   = "[command]",
    .desc    = "显示命令列表或指定命令的帮助",
    .opts    = cmd_help_opts,
    .handler = cmd_help_handler,
    .priv    = NULL,
};

/*@brief 拆分行到 argv (支持引号)
 *@param line 输入行 (会被修改, 插入 '\0')
 *@param argv argv 输出数组
 *@param max  argv 最大容量
 *@return 拆分出的参数个数
 */
static uint32_t split_argv(char *line, char **argv, uint32_t max)
{
    uint32_t argc = 0;
    char *p = line;

    while (*p != '\0' && argc < max) {
        p = (char *)skip_blank(p);
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') {
            /* 引号字段: 遇到下一个同引号结束 */
            char quote = *p++;
            argv[argc++] = p;
            while (*p != '\0' && *p != quote) {
                if (*p == '\\' && *(p + 1) != '\0')
                    p++;
                p++;
            }
            if (*p != '\0')
                *p++ = '\0';
        } else {
            /* 普通字段: 空白/引号/结尾结束 */
            argv[argc++] = p;
            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '"' && *p != '\'')
                p++;
            if (*p != '\0')
                *p++ = '\0';
        }
    }
    return argc;
}

/*@brief 解析选项 (-x, --xxx, --xxx=val)
 *@param argc    argv 数量
 *@param argv    参数列表
 *@param opts    选项描述表
 *@param opt_out 输出: 选项值表 (长度同 opts 表, 无 NULL 终止)
 *       opt_out[i] = NULL 表示未设置, 非 NULL 表示值 (无参数选项指向 "")
 *@param idx_out 输出: 非选项参数的起始 argv 索引
 *@retval true   解析成功
 *@retval false  遇到未知选项或缺少参数
 */
static bool parse_opts(int argc, char **argv,
    const app_module_shell_opt_t *opts,
    const char **opt_out, int *idx_out)
{
    int opt_cnt = 0;
    for (const app_module_shell_opt_t *o = opts; o->opt_long != NULL; o++)
        opt_out[opt_cnt++] = NULL;

    int i;
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] != '-') break;

        /* -- 结束选项解析 */
        if (arg[1] == '-' && arg[2] == '\0') { i++; break; }

        bool found = false;
        int oi;

        if (arg[1] == '-') {
            /* 长选项 --xxx 或 --xxx=val */
            const char *eq = strchr(arg + 2, '=');
            int name_len = eq ? (int)(eq - (arg + 2)) : (int)strlen(arg + 2);

            for (oi = 0; oi < opt_cnt; oi++) {
                if (strncmp(arg + 2, opts[oi].opt_long, (size_t)name_len) == 0
                    && opts[oi].opt_long[name_len] == '\0') {
                    found = true;
                    if (opts[oi].has_arg) {
                        if (eq)
                            opt_out[oi] = eq + 1;
                        else if (i + 1 < argc)
                            opt_out[oi] = argv[++i];
                        else
                            return false;
                    } else {
                        opt_out[oi] = "";
                        if (eq) return false;
                    }
                    break;
                }
            }
        } else {
            /* 短选项 -x 或 -xval */
            for (oi = 0; oi < opt_cnt; oi++) {
                if (arg[1] == opts[oi].opt_short) {
                    found = true;
                    if (opts[oi].has_arg) {
                        if (arg[2] != '\0')
                            opt_out[oi] = arg + 2;
                        else if (i + 1 < argc)
                            opt_out[oi] = argv[++i];
                        else
                            return false;
                    } else {
                        opt_out[oi] = "";
                        /* 支持 -abc → -a -b -c (组合短选项) */
                        if (arg[2] != '\0') {
                            memmove(arg + 1, arg + 2, strlen(arg + 2) + 1);
                            i--;
                        }
                    }
                    break;
                }
            }
        }

        if (!found) return false;
    }

    *idx_out = i;
    return true;
}

/*@brief shell 解析器初始化
 */
void app_module_shell_parser_ready(void)
{
    app_module_shell_cmd_cnt = 0;
    app_module_shell_parser_register(&cmd_help);
    APP_SYS_LOG_INFO("shell parser ready, max cmd=%d", APP_MODULE_SHELL_PARSER_CMD_MAX);
}

/*@brief 注册一个命令
 */
bool app_module_shell_parser_register(const app_module_shell_cmd_t *cmd)
{
    if (cmd == NULL || cmd->name == NULL)
        return false;

    for (uint32_t i = 0; i < app_module_shell_cmd_cnt; i++) {
        if (strcmp(app_module_shell_cmd_tbl[i]->name, cmd->name) == 0)
            return false;
    }

    if (app_module_shell_cmd_cnt >= APP_MODULE_SHELL_PARSER_CMD_MAX)
        return false;

    app_module_shell_cmd_tbl[app_module_shell_cmd_cnt++] = cmd;
    APP_SYS_LOG_INFO("shell cmd registered: %s", cmd->name);
    return true;
}

/*@brief 解析并执行一行输入
 */
void app_module_shell_parser_exec(char *line,
    void (*write)(const char *, uint32_t, void *), void *priv)
{
    if (line == NULL || line[0] == '\0' || line[0] == '#')
        return;

    /* 拆分 argv */
    char *argv[APP_MODULE_SHELL_PARSER_ARGV_MAX] = {0};
    uint32_t argc = split_argv(line, argv, APP_MODULE_SHELL_PARSER_ARGV_MAX);

    if (argc == 0) return;

    /* 缓存 write 回调 (供内置命令用) */
    exec_write      = write;
    exec_write_priv = priv;

    /* 查找命令 */
    const app_module_shell_cmd_t *cmd = NULL;
    for (uint32_t i = 0; i < app_module_shell_cmd_cnt; i++) {
        if (strcmp(app_module_shell_cmd_tbl[i]->name, argv[0]) == 0) {
            cmd = app_module_shell_cmd_tbl[i];
            break;
        }
    }

    if (cmd == NULL) {
        parser_puts("unknown command, try 'help'");
        return;
    }

    /* 检查 -h/--help (不管命令有没有注册 help 选项) */
    if (argc > 1) {
        for (uint32_t i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_help(cmd);
                return;
            }
        }
    }

    /* 如果有选项表, 走 parse_opts 分离选项 */
    if (cmd->opts) {
        int opt_cnt = 0;
        for (const app_module_shell_opt_t *o = cmd->opts; o->opt_long != NULL; o++)
            opt_cnt++;

        if (opt_cnt >= 64) {
            APP_SYS_LOG_ERR("too many opts (%d)", opt_cnt);
            return;
        }

        const char *opt_vals[64];
        int arg_idx;

        if (!parse_opts((int)argc, argv, cmd->opts, opt_vals, &arg_idx)) {
            parser_puts("invalid option, try '-h' for help");
            return;
        }

        /* 检查是否触发了 --help (如果命令注册了 help 选项) */
        for (int oi = 0; oi < opt_cnt; oi++) {
            if (strcmp(cmd->opts[oi].opt_long, "help") == 0 && opt_vals[oi]) {
                print_help(cmd);
                return;
            }
        }

        /* 重建 argv: 只保留非选项参数 */
        char *new_argv[APP_MODULE_SHELL_PARSER_ARGV_MAX];
        new_argv[0] = argv[0];
        int new_argc = 1;
        for (int j = arg_idx; j < (int)argc && new_argc < APP_MODULE_SHELL_PARSER_ARGV_MAX; j++)
            new_argv[new_argc++] = argv[j];

        /* 在 argv 尾部附加 opt_vals 指针, handler 通过 opt_vals 访问选项 */
        /* 将 opt_vals 嵌入 argv[new_argc], 由 handler 内部解引用 */
        /* 调用者无需关心此细节, 只需在 handler 中通过 argv[new_argc] 取 opt_vals */
        /* 传递方式: (const char **)(uintptr_t) 转换, 避免 char* 强制警告 */
        argv[new_argc] = (char *)(uintptr_t)opt_vals;
        cmd->handler(new_argc, argv, cmd->priv);
    } else {
        cmd->handler((int)argc, argv, cmd->priv);
    }

    exec_write      = NULL;
    exec_write_priv = NULL;
}
