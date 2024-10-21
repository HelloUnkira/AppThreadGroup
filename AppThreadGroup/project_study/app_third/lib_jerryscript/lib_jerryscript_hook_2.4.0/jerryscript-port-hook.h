#ifndef JERRYSCRIPT_PORT_HOOK_H
#define JERRYSCRIPT_PORT_HOOK_H

jerry_log_level_t jerry_port_hook_get_log_level(void);

void jerry_port_hook_set_log_level(jerry_log_level_t level);

void jerry_port_hook_set_current_context(jerry_context_t *context_p);

#endif
