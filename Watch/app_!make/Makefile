
#一个简单的Makefile

app_dbg = -g
app_std = -std=gnu99

app_system = -lpthread

app_arch_h = \
    -I ../app_arch/                     \
    -I ../app_arch/app_arch_adaptor     \
    -I ../app_arch/app_arch_ext_mem     \

app_arch_c = \
    ../app_arch/*.c                     \
    ../app_arch/app_arch_adaptor/*.c    \
    ../app_arch/app_arch_ext_mem/*.c    \

app_os_h = \
    -I ../app_os/                   \
    -I ../app_os/app_os_adaptor     \
    -I ../app_os/app_os_ext_mem     \

app_os_c = \
    ../app_os/*.c                   \
    ../app_os/app_os_adaptor/*.c    \
    ../app_os/app_os_ext_mem/*.c    \

app_sys_h = \
    -I ../app_sys/                      \
    -I ../app_sys/app_sys_pipe          \
    -I ../app_sys/app_sys_ring_buffer   \
    -I ../app_sys/app_sys_util          \

app_sys_c = \
    ../app_sys/*.c                      \
    ../app_sys/app_sys_pipe/*.c         \
    ../app_sys/app_sys_ring_buffer/*.c  \
    ../app_sys/app_sys_util/*.c         \

app_thread_h = \
    -I ../app_thread/                                       \
    -I ../app_thread/app_thread_master                      \
    -I ../app_thread/app_thread_master/app_thread_adaptor   \
    -I ../app_thread/app_thread_master/app_module_system    \
    -I ../app_thread/app_thread_mix_irq                             \
    -I ../app_thread/app_thread_mix_custom                          \
    -I ../app_thread/app_thread_mix_custom/app_module_alarm         \
    -I ../app_thread/app_thread_mix_custom/app_module_clock         \
    -I ../app_thread/app_thread_mix_custom/app_module_countdown     \
    -I ../app_thread/app_thread_mix_custom/app_module_stopwatch     \
    -I ../app_thread/app_thread_mix_custom/app_module_world_time    \
    -I ../app_thread/app_thread_source_manage                           \
    -I ../app_thread/app_thread_source_manage/app_module_ext_mem        \
    -I ../app_thread/app_thread_source_manage/app_module_source         \
    -I ../app_thread/app_thread_source_manage/app_module_dump           \
    -I ../app_thread/app_thread_source_manage/app_module_load           \
    -I ../app_thread/app_thread_source_manage/app_module_source_mix     \

app_thread_c = \
    ../app_thread/*.c                                       \

app_thread_master_c = \
    ../app_thread/app_thread_master/*.c                     \
    ../app_thread/app_thread_master/app_thread_adaptor/*.c  \
    ../app_thread/app_thread_master/app_module_system/*.c   \

app_thread_mix_irq_c = \
    ../app_thread/app_thread_mix_irq/*.c                            \

app_thread_mix_custom_c = \
    ../app_thread/app_thread_mix_custom/*.c                         \
    ../app_thread/app_thread_mix_custom/app_module_alarm/*.c        \
    ../app_thread/app_thread_mix_custom/app_module_clock/*.c        \
    ../app_thread/app_thread_mix_custom/app_module_countdown/*.c    \
    ../app_thread/app_thread_mix_custom/app_module_stopwatch/*.c    \
    ../app_thread/app_thread_mix_custom/app_module_world_time/*.c   \

app_thread_source_manage_c = \
    ../app_thread/app_thread_source_manage/*.c                          \
    ../app_thread/app_thread_source_manage/app_module_ext_mem/*.c       \
    ../app_thread/app_thread_source_manage/app_module_source/*.c        \
    ../app_thread/app_thread_source_manage/app_module_dump/*.c          \
    ../app_thread/app_thread_source_manage/app_module_load/*.c          \
    ../app_thread/app_thread_source_manage/app_module_source_mix/*.c    \

app_main_h = \
    -I ../app_!linux    \
    -I ..               \

app_main_c = \
    ../app_!linux/*.c   \
    ../*.c  \

main: app_arch.o app_os.o app_sys.o app_thread.o app_main.o
	gcc $(app_dbg) -o main ./*.o $(app_system)
	mv -f ./*.o  make_bin
	mv -f ./main make_exe

app_arch.o:
	@gcc -c $(app_std) $(app_arch_c) $(app_arch_h)
	@echo app_arch

app_os.o:
	@gcc -c $(app_std) $(app_os_c) $(app_os_h)
	@echo app_os

app_sys.o:
	@gcc -c $(app_std) $(app_sys_c) $(app_arch_h) $(app_os_h) $(app_sys_h)
	@echo app_sys

app_thread.o:
	@gcc -c $(app_std) $(app_thread_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h)
	@echo app_thread
	@gcc -c $(app_std) $(app_thread_master_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h)
	@echo app_thread_master
	@gcc -c $(app_std) $(app_thread_mix_irq_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h)
	@echo app_thread_mix_irq
	@gcc -c $(app_std) $(app_thread_mix_custom_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h)
	@echo app_thread_mix_custom
	@gcc -c $(app_std) $(app_thread_source_manage_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h)
	@echo app_thread_source_manage

app_main.o:
	@gcc -c $(app_std) $(app_main_c) $(app_arch_h) $(app_os_h) $(app_sys_h) $(app_thread_h) $(app_main_h)
	@echo app_main
