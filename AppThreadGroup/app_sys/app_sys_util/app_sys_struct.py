import json


# -- coding: utf-8 --**


def app_sys_struct():
    # file operation
    file_in = open('app_sys_struct.json', 'r')
    file_out = open('app_sys_struct.h', 'w')
    file_in_str = file_in.read()
    file_in_dic = json.loads(file_in_str)
    print('-----parser start------')
    # struct member
    member = file_in_dic['member']
    typedef = file_in_dic['typedef']
    type_width = file_in_dic['type_width']
    name_width = file_in_dic['name_width']
    # depend-on
    file_out.write('#ifndef APP_SYS_STRUCT_H\n')
    file_out.write('#define APP_SYS_STRUCT_H\n\n')
    # simplify annotation
    file_out.write('/*一个简单的结构体方法重载器\n')
    file_out.write(' *辅助工具,一般不使用\n')
    file_out.write(' */\n\n')
    file_out.write('#include <stddef.h>\n')
    file_out.write('#include <stdint.h>\n')
    file_out.write('#include <stdbool.h>\n\n')
    # struct
    file_out.write('typedef struct {\n')
    # member
    for dic in member:
        file_out.write(('\t%-' + str(type_width) + 's') % (dic['type']))
        file_out.write(('\t%-' + str(name_width) + 's') % (dic['name'] + ';'))
        file_out.write('//')
        file_out.write(dic['comment'])
        file_out.write('\n')
    # struct
    file_out.write('} ' + typedef + '_t;\n')
    # set method
    for dic in member:
        file_out.write('\n')
        file_out.write('/*@brief \n')
        file_out.write(' *@param \n')
        file_out.write(' *@param \n')
        file_out.write(' *@retval \n')
        file_out.write(' */\n')
        file_out.write('static inline void ' + typedef + '_set_' + dic['name'])
        file_out.write('(' + typedef + '_t *instance, ' + dic['type'] + ' ' + dic['name'] + ')')
        file_out.write('\n{\n\tinstance->' + dic['name'] + ' = ' + dic['name'] + ';\n}\n')
    # get method
    for dic in member:
        file_out.write('\n')
        file_out.write('/*@brief \n')
        file_out.write(' *@param \n')
        file_out.write(' *@param \n')
        file_out.write(' *@retval \n')
        file_out.write(' */\n')
        file_out.write('static inline ' + dic['type'] + ' ' + typedef + '_get_' + dic['name'])
        file_out.write('(' + typedef + '_t *instance' + ')')
        file_out.write('\n{\n\treturn instance->' + dic['name'] + ';\n}\n')
    file_out.write('\n#endif\n')
    # file operation
    print('-----parser end--------')
    file_out.close()
    file_in.close()


if __name__ == '__main__':
    app_sys_struct()
