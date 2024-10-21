
#楼梯式头文件导入
import os
import sys
import pathlib
import argparse
import subprocess
import collections

#定义一个元组:
Uglify = collections.namedtuple("Uglify", ['content', 'used_uglifyjs'])

#解析命令行所携带的参数
def parse_args():
    #创建一个参数解析器对象,它用于接下来获取参数
    parser = argparse.ArgumentParser(description=
        ("Runs minifier on a JavaScript file and writes it out in a C string format."))
    #输入参数错误时给与的反馈指令
    parser.add_argument("input", metavar='input.js')
    parser.add_argument("output", metavar='output')
    parser.add_argument("-f", "--full",
                        help="Skip minimizing JS for debugging", action="store_true")
    #获取命令行的参数
    args = parser.parse_args()
    #print(args)
    #将所有路径设置为绝对路径，并扩展任何“~/”用法
    for arg_name in ('input', 'output'):
        attr          = getattr(args, arg_name)
        path_expand   = os.path.expanduser(attr)
        path_absolute = os.path.abspath(path_expand)
        path_all      = pathlib.Path(path_absolute)
        setattr(args, arg_name, path_all)
        #print('attr:',attr)
        #print('expand_path:',path_expand)
        #print('abs_path:',path_absolute)
        #print('all_path:',path_all)
        #print(args)
    #检查输入文件是否存在
    if not args.input.is_file():
        string = 'ERROR: The INPUT_FILE {!r} does not exist.'.format(str(args.input))
        print(string)
        parser.print_help()
        sys.exit(1)
    return args

#使用uglifyjs软件对js解析压缩解析
def uglifyjs(input_path):
    #解析文件名转字符串
    filename = str(input_path)
    #准备在命令行执行:uglifyjs --version
    try:
        cmd_line = ['uglifyjs', '--version']
        result = subprocess.call(cmd_line, shell = True, stdout=subprocess.DEVNULL)
    except FileNotFoundError:
        #测试时产生此错误是shell = True未设置导致,设置完毕后有效
        print("FileNotFoundError")
        return None
    #检查版本差异,是新版本还是旧版本
    #然后根据版本的不同,去确认下一个解析命令
    #此处这俩个解析参数目前无法使用,仍待后续继续熟悉
    #-nc或--no-copyright: 默认uglifyjs会在输出后的代码中添加版权信息等注释代码(传入该参数禁用此功能)
    #-mt或--mangle-toplevel: 在顶级作用域打乱变量名称(默认不开启) 未知何意
    ugl_args = [] #["-nc", "-mt"]
    if result == 0:
        #解析命令为: uglifyjs 文件名 --指定参数
        cmd_line = ['uglifyjs', filename] + ugl_args
    else:
        #解析命令为: uglifyjs --指定参数 文件名
        cmd_line = ['uglifyjs'] + ugl_args + [filename]
    try:
        output = subprocess.check_output(cmd_line, shell = True)
    except subprocess.CalledProcessError as exc:
        #测试时产生此错误是命令行参数无法识别导致,目前是弃用参数
        print("ERROR: Minification failed!")
        sys.exit(exc.returncode)
    #将输出的内容转指定格式,以字符串表示
    content = output.decode('utf-8')
    #后放置在元组中,参数1是文本,参数2是解析结果,此处默认True
    return Uglify(content, True)

#启用外部程序Bin2C.exe
def bin_to_c(output_path):
    #解析文件名转字符串
    filename = str(output_path)
    #准备在命令行执行:Bin2C.exe
    #但是,我们必须要获得绝对路径
    py_filename = os.path.abspath(__file__)
    filepath    = os.path.dirname(py_filename)
    filepath   += '\\software\\'
    #获得文件执行的绝对路径
    try:
        cmd_line = [filepath + 'Bin2C.exe ', filename]
        result = subprocess.call(cmd_line, shell = False, stdout=subprocess.DEVNULL)
    except FileNotFoundError:
        #测试时产生此错误是shell = True未设置导致,设置完毕后有效
        print("FileNotFoundError")
        return None

#处理部分转义字符
def write_minified(output_path, minified_result, show_in_txt):
    #以只写的方式打开指定文件
    with output_path.open('w') as out_file:
        #字符串流以"作为起始
        if show_in_txt == True:
            print('"\\', end='\n', file=out_file)
        last_char = ""
        #以字符迭代的方式遍历整个输出字符目录
        for char in minified_result.content:
            if show_in_txt == False:
                print(char, end='', file=out_file)
                continue
            #如果遇到原生",对其进行转义成\"
            if char in ('"', ):
                print(r'\"', end='', file=out_file)
            #如果可能遇到特定的转义起始(上一字符)
            elif last_char == '\\':
                #小心\r\n的情况,记得处理
                if char in ('n', 'r'):
                    #转义格式化:\\等价于转义的\
                    #{}被格式化为'n'或'r'中的一个
                    print('\\{}'.format(char), end='', file=out_file)
            #遇到换行符时
            elif char == '\n':
                #如果未启用压缩时,换行符被替换为:
                #代码中的原生换行符+可直接显示的换行符
                if not minified_result.used_uglifyjs:
                    print(r'\n"', end='\n"', file=out_file)
            #遇到;时,通常一行只有一个语句(额外添加内容)
            elif char == ';':
                print(char, end='', file=out_file)
                #尾后添加折行,并使用显示换行
                print('\\', end='\n', file=out_file)
            else:
                #其余的字符直接追加到文件中即可
                print(char, end='', file=out_file)
            #记录当前字符:因为有可能当前字符为:转义本身
            last_char = char
        #字符串流以"作为结束
        if show_in_txt == True:
            print('"', end='\n', file=out_file)

def main():
    #解析命令行参数
    args = parse_args()
    #如果参数有效(未携带-f或--full指令时),解析js
    minified_result = None
    if not args.full:
        minified_result = uglifyjs(args.input)
    #print(minified_result)
    #如果压缩解析失败时,或者不解析时
    if minified_result == None:
        #以只读的方式打开input文件
        fp = open(str(args.input), "r")
        #以字节流读取全部内容
        content = fp.read()
        #关闭该input文件
        fp.close()
        #重新组包,此时仅仅对文件进行转C的简要处理
        minified_result = Uglify(content, False)
    #处理输出的部分中的转义
    write_minified(args.output, minified_result, False)
    bin_to_c(args.output)
    return
    write_minified(args.output, minified_result, True)

if '__main__' == __name__:
    sys.exit(main())
    