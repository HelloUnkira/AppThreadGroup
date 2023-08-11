
import os
import os.path
import json


# 编写集成化源文件
def encode_app_sys_vbus_c(file, dir_list):
    # 写点简要的说明
    file.write('/*app_sys_vbus.c:\n')
    file.write(' *通过app_sys_vbus.py生成\n')
    file.write(' *参考app_sys_vbus.json中的模式生成源\n')
    file.write(' *它可以搭建不同线程中不同子模块的连接\n')
    file.write(' *也可以搭建同一线程中不同子模块的连接\n')
    file.write(' *由于极小的限制而成为一个万金油模组\n')
    file.write(' *可以对及其混乱的流程进行解耦合\n')
    file.write(' *但是优先不建议使用\n')
    file.write(' */\n\n')
    file.write('#include "app_sys_vbus.h"\n\n')
    # 提取所有外源依赖
    for items in dir_list:          # 列表中是字典
        for item in items:      # 从字典提取关键字和值
            if item == 'name':
                string = '/* ' + items[item] + ' */\n'
                file.write(string)
            if item == 'brief':
                string = '/* ' + items[item] + ' */\n'
                file.write(string)
            if item == 'response':
                for element in items[item]:
                    string = 'extern void ' + element + '(app_sys_vbus *event);\n'
                    file.write(string)
    file.write('\n')
    # 生成派发函数
    file.write("extern void app_sys_vbus_respond(app_sys_vbus *event)\n")
    file.write('{\n')
    file.write('\tswitch (event->command) {\n')
    for items in dir_list:          # 列表中是字典
        tag_case = 0
        for item in items:      # 从字典提取关键字和值
            if item == 'name':
                tag_case = 1
                string = '\tcase ' + items[item] + ':\n'
                file.write(string)
            if item == 'response':
                for element in items[item]:
                    string  = '\t\t' + element + '(event);\n'
                    file.write(string)
        if tag_case == 1:
            string = '\t\tbreak;\n'
            file.write(string)
    file.write('\t}\n}\n\n')
    # 本地挂载一个空函数
    file.write('void app_sys_vbus_empty(app_sys_vbus *event)\n')
    file.write('{\n}\n')

# 编写集成化头文件
def encode_app_sys_vbus_h(file, dir_list):
    file.write('#ifndef APP_SYS_VBUS_H\n')
    file.write('#define APP_SYS_VBUS_H\n\n')
    # 写点简要的说明
    file.write('/*app_sys_vbus.h:\n')
    file.write(' *通过app_sys_vbus.py生成\n')
    file.write(' *参考app_sys_vbus.json中的模式生成源\n')
    file.write(' *它可以搭建不同线程中不同子模块的连接\n')
    file.write(' *也可以搭建同一线程中不同子模块的连接\n')
    file.write(' *由于极小的限制而成为一个万金油模组\n')
    file.write(' *可以对及其混乱的流程进行解耦合\n')
    file.write(' *但是优先不建议使用\n')
    file.write(' */\n\n')
    file.write('#include <stdint.h>\n\n')
    file.write('typedef struct {\n\t')
    file.write('/* 数据交换取决于双方约定俗成 */\n\t')
    file.write('uint32_t command;   /* 响应的指令 */\n\t')
    file.write('uint32_t size;      /* 数据元长度 */\n\t')
    file.write('void    *data;      /* 数据元 */\n')
    file.write('} app_sys_vbus;\n\n')
    file.write("extern void app_sys_vbus_respond(app_sys_vbus *event);\n\n")
    event_set_count = 0
    for items in dir_list:          # 列表中是字典
        for item in items:      # 从字典提取关键字和值
            if item == 'name':
                event_set_count += 1
                string  = '#define ' + items[item] + '\t\t0x'
                string += format(event_set_count, '>04x') + '\n'
                file.write(string)
    file.write('\n#endif\n')


# 启用集成化事件集生成
def encode_app_sys_vbus():
    # json转Python字符串并转标准字典
    json_file = open('app_sys_vbus.json', 'r')
    json_dict = json.loads(json_file.read())
    # 仅支持批量化事件集管理
    if json_dict['type'] != 'app_sys_vbus':
        return
    # 从标准字典获取标准列表
    dir_list = json_dict['app_sys_vbus']
    # 开启三个文件
    app_sys_vbus_h = open('app_sys_vbus.h', 'w', encoding='utf-8')
    app_sys_vbus_c = open('app_sys_vbus.c', 'w', encoding='utf-8')
    # 解析
    encode_app_sys_vbus_h(app_sys_vbus_h, dir_list)
    encode_app_sys_vbus_c(app_sys_vbus_c, dir_list)
    # 关闭三个文件
    app_sys_vbus_h.close()
    app_sys_vbus_c.close()
    json_file.close()


if __name__ == '__main__':
    encode_app_sys_vbus()
    input('app sys vbus generation completed\n')
