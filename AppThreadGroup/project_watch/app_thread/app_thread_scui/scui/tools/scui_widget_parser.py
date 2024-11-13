# 控件资源打包
import re
import os
import sys
import json


# 流式处理所有widget文件
def scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, scui_widget_parser_table):
    scui_widget_parser_h = scui_widget_parser_list[0]
    scui_widget_parser_c = scui_widget_parser_list[1]
    # 头文件添加前缀, 源文件添加前缀
    scui_widget_parser_h.write('#ifndef SCUI_WIDGET_PARSER_H\n')
    scui_widget_parser_h.write('#define SCUI_WIDGET_PARSER_H\n\n')
    scui_widget_parser_h.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    scui_widget_parser_c.write('/*一个通过scui_widget_parser脚本生成的widget表\n */\n\n')
    scui_widget_parser_c.write('#include \"scui.h\"\n\n')
    # 填充数据表
    offset_name = scui_widget_parser_table['offset_name']
    offset_value = scui_widget_parser_table['offset_value']
    scui_widget_parser_h.write('typedef enum {\n')
    scui_widget_parser_h.write('\t%s = %s,\n' % (offset_name, offset_value))
    scui_widget_parser_handle_num = 0
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_widget_parser_handle_num += 1
                scui_widget_parser_h.write('\t%s,\n' % widget['widget.myself'])
            except Exception as e:
                print(widget)
                print(e)
                return
    scui_widget_parser_h.write('} scui_widget_parser_handle_t;\n\n')
    scui_widget_parser_h.write('extern const void * scui_widget_parser_table[%s];\n\n' % scui_widget_parser_handle_num)
    scui_widget_parser_h.write('#endif\n')
    # 填充函数定义或者声明
    scui_widget_parser_c.write('#if defined(SCUI_WIDGET_EVENT_USE_EMPTY) && SCUI_WIDGET_EVENT_USE_EMPTY == 1\n')
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_event_cb = 'static void %s(scui_event_t *event)'
                scui_widget_parser_c.write('%s\n{\n}\n' % scui_event_cb % widget['widget.event_cb'])
            except Exception as e:
                pass
            try:
                scui_event_cb = 'static void %s(scui_event_t *event)'
                scui_widget_parser_c.write('%s\n{\n}\n' % scui_event_cb % widget['notify_cb'])
            except Exception as e:
                pass
    scui_widget_parser_c.write('#else\n')
    for scene in scene_list:
        for widget in scene['widget']:
            try:
                scui_event_cb = 'extern void %s(scui_event_t *event);'
                scui_widget_parser_c.write('%s\n' % scui_event_cb % widget['widget.event_cb'])
            except Exception as e:
                pass
            try:
                scui_event_cb = 'extern void %s(scui_event_t *event);'
                scui_widget_parser_c.write('%s\n' % scui_event_cb % widget['notify_cb'])
            except Exception as e:
                pass
    scui_widget_parser_c.write('#endif\n\n')
    # 对目标控件集合进行流式处理,提取数据内容
    for scene in scene_list:
        for widget in scene['widget']:
            print(widget['widget.myself'])
            # 写入结构
            scui_widget_tag = 'scui_widget_' + widget['widget.myself']
            scui_widget_type_unknown = True
            for widget_maker in scui_widget_parser_table['widget']:
                if widget['widget.type'] == widget_maker['class']:
                    scui_widget_parser_c.write('static const %s %s = {\n' % (widget_maker['maker'], scui_widget_tag))
                    scui_widget_type_unknown = False
                    break
            # 未知类型
            if scui_widget_type_unknown:
                print('\nwidget type unknown\n')
                print(widget)
                continue
            # 填充基础数据目标
            scui_widget_field_child_num = 0
            for field in widget:
                for widget_maker in scui_widget_parser_table['widget']:
                    if widget_maker['class'] == 'widget':
                        # 存在子控件字段,单独计算
                        if field == 'widget.child_num':
                            scui_widget_field_child_num = eval(widget[field])
                        elif field in widget_maker['field']:
                            scui_widget_parser_c.write('\t.%-30s = %s,\n' % (field, widget[field]))
                        break
            # 统计本控件有多少布局孩子
            for target in scene['widget']:
                try:
                    if widget['widget.myself'] == target['widget.parent']:
                        scui_widget_field_child_num += 1
                except Exception as e:
                    pass
            if scui_widget_field_child_num != 0:
                scui_widget_parser_c.write('\t.%-30s = %s,\n' % ("widget.child_num", str(scui_widget_field_child_num)))
            # 填充指定数据目标
            for field in widget:
                for widget_maker in scui_widget_parser_table['widget']:
                    if widget_maker['class'] == widget['widget.type']:
                        if field in widget_maker['field']:
                            scui_widget_parser_c.write('\t.%-30s = %s,\n' % (field, widget[field]))
                        break
            scui_widget_parser_c.write('};\n\n')
    # 填充数据表
    scui_widget_parser_c.write('const void * scui_widget_parser_table[%s] = {\n' % scui_widget_parser_handle_num)
    for scene in scene_list:
        for widget in scene['widget']:
            scui_widget_tag = 'scui_widget_' + widget['widget.myself']
            scui_widget_parser_c.write('\t(void *)&%s,\n' % scui_widget_tag)
    scui_widget_parser_c.write('};\n')
    pass


# 遍历整个文件夹,提取目标文件
def scui_widget_parser_collect(file_path_list, path):
    if os.path.isfile(path):
        if os.path.splitext(path)[1] == '.json':
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', path):
                file_path_list.append(path)
    if os.path.isdir(path):
        for item in os.listdir(path):
            # 规定如果目标文件或所在路径存在特殊屏蔽标记#,忽略它
            if not re.findall(r'#', os.path.join(path, item)):
                scui_widget_parser_collect(file_path_list, os.path.join(path, item))


# 打印重定向到文件
class ScuiRedirectPrint(object):
    def __init__(self, stream=sys.stdout, path='.', file='log.txt'):
        if not os.path.exists(path):
            os.makedirs(path)
        self.log = open(os.path.join(path, file), mode='w+', encoding='utf-8')
        self.terminal = stream

    def write(self, message):
        self.log.write(message)
        self.terminal.write(message)

    def flush(self):
        pass


# 主流程
def scui_widget_parser():
    # json转Python字符串并转标准字典
    parser_path = os.path.join(os.path.dirname(__file__), 'scui_widget_parser.json')
    json_file = open(parser_path, 'r', encoding='utf-8')
    json_dict = json.loads(json_file.read())
    json_file.close()
    if json_dict['type'] != r'scui widget parser table':
        print('parser table unknown')
        return
    scui_widget_parser_table = json_dict
    # print(scui_widget_parser_table)
    # 参数列表:文件根目录,
    if len(sys.argv) != 3:
        print('argv list not match')
        return
    src_path = sys.argv[1]
    dst_path = sys.argv[2]
    # 获得文件处理根路径
    if not os.path.exists(src_path):
        print('src path is not exist')
        return
    if not os.path.exists(dst_path):
        print('dst path is not exist')
        return
    # print重定向
    sys.stdout = ScuiRedirectPrint(sys.stdout, file=os.path.join(dst_path, 'scui_widget_parser.out'))   # redirect print
    sys.stderr = ScuiRedirectPrint(sys.stderr, file=os.path.join(dst_path, 'scui_widget_parser.err'))   # redirect print
    print('src path:', src_path)
    print('dst path:', dst_path)
    # 遍历整个文件夹,获取指定扩展名的文件
    file_path_list = []
    scui_widget_parser_collect(file_path_list, src_path)
    # check:
    # for item in file_path_list:
    #     print(item)
    # 读取所有的json文件并将其打包到一起
    json_dict_list = []
    for file in file_path_list:
        try:
            json_file = open(file, 'r', encoding='utf-8')
            json_dict = json.loads(json_file.read())
            if json_dict['type'] == 'scene':
                json_dict_list.append(json_dict)
            json_file.close()
        except Exception as e:
            print(file)
            print(e)
            return
    scene_list = json_dict_list
    # print(json_dict_list)
    # 核查文件支持
    scui_widget_parser_h = open(os.path.join(dst_path, 'scui_widget_parser.h'), mode='w', encoding='utf-8')
    scui_widget_parser_c = open(os.path.join(dst_path, 'scui_widget_parser.c'), mode='w', encoding='utf-8')
    scui_widget_parser_list = [scui_widget_parser_h, scui_widget_parser_c]
    scui_widget_parser_scene_list(scene_list, scui_widget_parser_list, scui_widget_parser_table)
    scui_widget_parser_h.close()
    scui_widget_parser_c.close()


if __name__ == '__main__':
    try:
        scui_widget_parser()
    except Exception as e:
        print(e)
    print('scui widget parser finish')
    input('请按任意键退出...')
