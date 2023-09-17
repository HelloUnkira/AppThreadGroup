import os
import os.path
import json

# -- coding: utf-8 --**


# 编写集成化源文件
def encode_app_sys_ext_mem_src_table_c(file, file_list, json_list):
    # 写点简要的说明
    file.write('/*一个代替注册的,脚本自动生成的,本地静态的外存data表\n')
    file.write(' *通过app_sys_ext_mem_src_table.py生成\n')
    file.write(' *参考本目录下的.json中的模式生成源\n */\n\n')
    file.write('#include "app_ext_lib.h"\n')
    file.write('#include "app_sys_lib.h"\n\n')
    # 提取所有外源依赖
    for index, json_text in enumerate(json_list):    # 字典列表中是字典
        file.write('static const app_sys_ext_mem_src_t app_sys_ext_mem_src_%s_table[] = {\n' % file_list[index])
        chunk_base = 0
        for items in json_text['data_group']:  # 列表中是字典
            file.write('\t{\n')
            chunk_offset = chunk_base
            for item in items:  # 从字典提取关键字和值
                if item == 'data_brief':
                    file.write('\t\t/* %s */\n' % items[item])
                if item == 'data_name':
                    file.write('\t\t.data_name = \"%s\",\n' % items[item])
                if item == 'data_size':
                    file.write('\t\t.data_size = %s,\n' % items[item])
                    chunk_base = chunk_base + eval(items[item])
            file.write('\t\t.data_base = %s,\n' % hex(chunk_offset))
            file.write('\t},\n')
        file.write('};\n\n')
    # 生成资源总表
    file.write('static const app_sys_ext_mem_src_t * app_sys_ext_mem_src_set_table[] = {\n')
    for file_name in file_list:
        file.write('\tapp_sys_ext_mem_src_%s_table,\n' % file_name)
    file.write('};\n\n')
    # 编写资源固化长度标签
    file.write('static const uint32_t app_sys_ext_mem_src_set_size_table[] = {\n')
    for file_name in file_list:
        file.write('\tapp_sys_arr_len(app_sys_ext_mem_src_%s_table),\n' % file_name)
    file.write('};\n\n')
    # 生成字符串表
    file.write('static const char * app_sys_ext_mem_src_string_table[] = {\n')
    for file_name in file_list:
        file.write('\t\"%s\",\n' % file_name)
    file.write('};\n\n')
    # 编写固化长度标签
    file.write('static const uint32_t app_sys_ext_mem_src_table_size = %s;\n\n' % str(len(json_list)))
    # 编写固化访问函数
    file.write('/*@brief     通过chunk名字以及data名字索引data静态实例\n')
    file.write(' *@param[in] chunk_name 名字\n')
    file.write(' *@param[in] data_name  名字\n')
    file.write(' *@retval    静态实例\n */\n')
    file.write('const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_name')
    file.write('(const char *chunk_name, const char *data_name)\n')
    file.write('{\n')
    file.write('\tfor (uint32_t idx = 0; idx < app_sys_ext_mem_src_table_size; idx++)\n')
    file.write('\t\tif (!strcmp(app_sys_ext_mem_src_string_table[idx], chunk_name))\n')
    file.write('\t\t\tfor (uint32_t idx1 = 0; idx1 < app_sys_ext_mem_src_set_size_table[idx]; idx1++)\n')
    file.write('\t\t\t\tif (!strcmp(app_sys_ext_mem_src_set_table[idx][idx1].data_name, data_name))\n')
    file.write('\t\t\t\t\treturn &app_sys_ext_mem_src_set_table[idx][idx1];\n')
    file.write('\t\treturn NULL;\n')
    file.write('}\n')
    file.write('/*@brief     通过chunk名字以及索引data静态实例\n')
    file.write(' *@param[in] chunk_name 名字\n')
    file.write(' *@param[in] index      索引\n')
    file.write(' *@retval    静态实例\n */\n')
    file.write('const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_index')
    file.write('(const char *chunk_name, uint32_t index)\n')
    file.write('{\n')
    file.write('\tfor (uint32_t idx = 0; idx < app_sys_ext_mem_src_table_size; idx++)\n')
    file.write('\t\tif (!strcmp(app_sys_ext_mem_src_string_table[idx], chunk_name))\n')
    file.write('\t\t\tif (index < app_sys_ext_mem_src_set_size_table[index])\n')
    file.write('\t\t\t\treturn &app_sys_ext_mem_src_set_table[idx][index];\n')
    file.write('\t\t\telse\n')
    file.write('\t\t\t\treturn NULL;\n')
    file.write('\t\treturn NULL;\n')
    file.write('}\n')


# 编写集成化头文件
def encode_app_sys_ext_mem_src_table_h(file):
    file.write('#ifndef APP_SYS_EXT_MEM_SRC_TABLE_H\n')
    file.write('#define APP_SYS_EXT_MEM_SRC_TABLE_H\n\n')
    # 编写固化访问函数
    file.write('/*@brief     通过chunk名字以及data名字索引data静态实例\n通过chunk名字以及data名字索引data静态实例\n')
    file.write(' *@param[in] chunk_name 名字\n')
    file.write(' *@param[in] data_name  名字\n')
    file.write(' *@retval    静态实例\n */\n')
    file.write('const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_name')
    file.write('(const char *chunk_name, const char *data_name);\n\n')
    file.write('/*@brief     通过chunk名字以及索引data静态实例\n')
    file.write(' *@param[in] chunk_name 名字\n')
    file.write(' *@param[in] index      索引\n')
    file.write(' *@retval    静态实例\n */\n')
    file.write('const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_index')
    file.write('(const char *chunk_name, uint32_t index);\n')
    file.write('\n#endif\n')


# 启用集成化资源总表生成
def encode_app_sys_ext_mem_src_table():
    # 读取本文件夹下所有的json文件并获得文件名
    json_file_list = []
    for file_name in os.listdir('.'):
        # 跳过主分区表,一级分区表,其他的均为二级分区表
        if file_name.find('app_sys_ext_mem_table.json') != -1:
            continue
        if file_name.find('.json') != -1:
            json_file_list.append(file_name.split('.')[0])
    # json转Python字符串并转标准字典
    file_list = []
    json_list = []
    for file_name in json_file_list:
        json_file = open(file_name + '.json', 'r')
        json_dict = json.loads(json_file.read())
        # 仅支持批量化资源总表管理
        if json_dict['type'] == file_name:
            file_list.append(file_name)
            json_list.append(json_dict)
        json_file.close()
    # 开启俩个文件
    app_sys_ext_mem_src_table_h = open('app_sys_ext_mem_src_table.h', mode='w', encoding='utf-8')
    app_sys_ext_mem_src_table_c = open('app_sys_ext_mem_src_table.c', mode='w', encoding='utf-8')
    # 解析
    encode_app_sys_ext_mem_src_table_h(app_sys_ext_mem_src_table_h)
    encode_app_sys_ext_mem_src_table_c(app_sys_ext_mem_src_table_c, file_list, json_list)
    # 关闭俩个文件
    app_sys_ext_mem_src_table_h.close()
    app_sys_ext_mem_src_table_c.close()


if __name__ == '__main__':
    encode_app_sys_ext_mem_src_table()
    print('app sys ext mem src table generation completed')
