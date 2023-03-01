import os
import os.path
import json


# -- coding: utf-8 --**


# 编写集成化源文件
def encode_app_module_ext_mem_table_c(file, dir_list):
    # 写点简要的说明
    file.write('/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表\n')
    file.write(' *通过app_module_ext_mem_table.py生成\n')
    file.write(' *参考app_module_ext_mem_table.json中的模式生成源\n */\n\n')
    file.write('#include "app_std_lib.h"\n')
    file.write('#include "app_module_ext_mem.h"\n\n')
    # 先提取所有的chunk_base,然后为此生成chunk_offset
    chunk_base = []
    chunk_offset = []
    for items in dir_list:      # 列表中是字典
        for item in items:      # 从字典提取关键字和值
            if item == 'chunk_base':
                if int(items[item], 16) not in chunk_base:
                    chunk_base.append(int(items[item], 16))
                    chunk_offset.append(int(0))
    # 提取所有外源依赖
    file.write('static const app_module_ext_mem_t app_module_ext_mem_table[] = {\n')
    for items in dir_list:      # 列表中是字典
        file.write('\t{\n')
        for item in items:      # 从字典提取关键字和值
            if item == 'chunk_brief':
                file.write('\t\t/* %s */\n' % items[item])
            if item == 'chunk_name':
                file.write('\t\t.chunk_name \t= \"%s\",\n' % items[item])
            if item == 'chunk_base':
                file.write('\t\t.chunk_base \t=  %s,\n' % items[item])
                index = chunk_base.index(int(items[item], 16))
                local_offset = chunk_offset[index]
            if item == 'chunk_size':
                file.write('\t\t.chunk_size \t=  %s,\n' % items[item])
                chunk_offset[index] = chunk_offset[index] + eval(items[item])
        file.write('\t\t.chunk_offset \t=  %s,\n' % hex(local_offset))
        file.write('\t},\n')
    file.write('};\n\n')
    # 编写固化长度标签
    file.write('static const uint32_t app_module_ext_mem_table_size =\n')
    file.write('               sizeof(app_module_ext_mem_table) /\n')
    file.write('               sizeof(app_module_ext_mem_table[0]);\n\n')
    # 编写固化访问函数
    file.write('/*@brief通过名字索引chunk静态实例\n')
    file.write(' *@param[in] name名字\n')
    file.write(' *@retval    chunk静态实例\n */\n')
    file.write('const app_module_ext_mem_t * app_module_ext_mem_find_by_name(const char *name)\n')
    file.write('{\n')
    file.write('\tfor (uint32_t idx = 0; idx < app_module_ext_mem_table_size; idx++)\n')
    file.write('\t\tif (!strcmp(app_module_ext_mem_table[idx].chunk_name, name))\n')
    file.write('\t\t\treturn &app_module_ext_mem_table[idx];\n')
    file.write('\t\treturn NULL;\n')
    file.write('}\n')


# 编写集成化头文件
def encode_app_module_ext_mem_table_h(file):
    file.write('#ifndef APP_MODULE_EXT_MEM_TABLE_H\n')
    file.write('#define APP_MODULE_EXT_MEM_TABLE_H\n\n')
    # 写点简要的说明
    file.write('/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表\n')
    file.write(' *通过app_module_ext_mem_table.py生成\n')
    file.write(' *参考app_module_ext_mem_table.json中的模式生成源\n */\n\n')
    # 编写固化访问函数接口
    file.write('/*@brief通过名字索引chunk静态实例\n')
    file.write(' *@param[in] name名字\n')
    file.write(' *@retval    chunk静态实例\n */\n')
    file.write('const app_module_ext_mem_t * app_module_ext_mem_find_by_name(const char *name);\n')
    file.write('\n#endif\n')
    pass


# 启用集成化外存表生成
def encode_app_module_ext_mem_table():
    # json转Python字符串并转标准字典
    json_file = open('app_module_ext_mem_table.json', 'r')
    json_dict = json.loads(json_file.read())
    json_file.close()
    # 仅支持批量化外存表管理
    if json_dict['type'] != 'app_module_ext_mem_table':
        return
    # 从标准字典获取标准列表
    dir_list = json_dict['app_module_ext_mem_table']
    # 开启三个文件
    app_module_ext_mem_table_h = open('app_module_ext_mem_table.h', mode='w', encoding='utf-8')
    app_module_ext_mem_table_c = open('app_module_ext_mem_table.c', mode='w', encoding='utf-8')
    # 解析
    encode_app_module_ext_mem_table_h(app_module_ext_mem_table_h)
    encode_app_module_ext_mem_table_c(app_module_ext_mem_table_c, dir_list)
    # 关闭三个文件
    app_module_ext_mem_table_h.close()
    app_module_ext_mem_table_c.close()


if __name__ == '__main__':
    encode_app_module_ext_mem_table()
    input('\nscript generation completed\n')
