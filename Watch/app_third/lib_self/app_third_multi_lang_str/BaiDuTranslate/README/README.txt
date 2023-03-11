简要介绍:
    reverse.js:
        逆向工程抓取的JS脚本,用于传输加密,无需关心
    translate.py:
        爬虫脚本文件,请求百度翻译器获得结果并持久化,无需关心
    translate.json:
        需要对里面部分字段进行配置,每更换电脑内部部分字段需要额外配置
    translate.input:
        输入给翻译脚本的文本文件,格式为(无空格): 单词,单词,单词
    translate.output:
        翻译脚本输出出的文本文件,格式为(无空格): 列表(换行)列表(换行)列表(换行)
            这种文件文件简单的编码后即可定制化输出
    translate.xlsx:
        翻译脚本输出出的定向文件,适合复制粘贴使用


第一次需要对json中的四个特殊字段进行更新(百度反扒机制)
详情见README文件夹中流程,将网页获取到的信息粘贴到json脚本中的指定字段即可
之后只需要跟新json中的input_type,input_file,output_list即可


缺少包时:
    pip  install 包名 -i http://pypi.douban.com/simple/ --trusted-host pypi.douban.com
    pip3 install 包名 -i http://pypi.douban.com/simple/ --trusted-host pypi.douban.com

    缺少execjs时是安装这个包 --> PyExecJS


cmd==>>:python translate_0.py   //这个是基础同步版本的,速度慢一些
cmd==>>:python translate_1.py   //这个是异步协程版本的,速度理论上比上面的要快一些


补充错误:
execjs._exceptions.ProgramError: SyntaxError: 缺少 ';'
	1.安装nodejs(下载地址:https://nodejs.org/zh-cn/download/)
	2.默认安装(一路next),默认配置环境变量即可(重启PyCharm)


