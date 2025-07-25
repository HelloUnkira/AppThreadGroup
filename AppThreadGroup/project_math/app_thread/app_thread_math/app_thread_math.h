#ifndef APP_THREAD_MATH_H
#define APP_THREAD_MATH_H

typedef enum {  /* 线程模组 */
    app_thread_math_none = 0,           /* 系统模组,占位符(线程组公共事件) */
    app_thread_math_test,               /* 测试模组 */
} app_thread_math_module_t;

// 这个从头文件中拿出来
void AppMathTest(void);

#endif
