//JavaScript基本语法测试
//和C很像,尤其是算术运算部分
//https://www.w3school.com.cn/js/js_string_templates.asp

//这里只使用接口
//console.log
//ido_debug_info

//约定:
//Jerry对new关键字不支持
//并且,一定不要用new,尽最大能力一定不要用new,请不要用new
//这对性能而言是巨大的灾难,PC上面能不用new都基本上不用new

//对象简介:
//布尔是对象,        如果用new关键词定义
//数字是对象,        如果用new关键词定义
//字符串是对象,      如果用new关键词定义
//日期永远都是对象
//算术永远都是对象
//正则表达式永远都是对象
//数组永远都是对象
//函数永远都是对象
//对象永远都是对象

//开始...
//变量
//任何类型的变量都是同一关键字创建
//同样的,它也可以接受返回的任意类型的变量
//创建变量,全局
var x = 5;
var y = 2.5;
//加减乘除
var z = 1.0/(x+y)*(x-y);
var start = "!!!hello world!!!";
ido_debug_info("");
console.log(start);
console.log("1.0/(x+y)*(x-y):", 1.0/(x+y)*(x-y), " result:", z);
//let 和 const
{
    const PAI = 3.1415926;  //创建常量,不可重新赋值
    let   pai = 3.1415926;  //创建块作用域变量,有效范围在块内
}
//基本运算符(优先级和C差不多)
var x = 1;      //这里定义了同名变量(之前的那个就会失效)
var y = 0.5;    //这种出现了同名情况(实际上是由于提升导致的)
var z = 1.0/(x+y)*(x-y);
var a = 33 % 7; //取模
console.log("z:", (z -= ++z), "z:", z);
console.log("a:", (a += a--), "a:", a);
console.log("a**2:", a**2);  //幂运算:pow(x,y)
//字符串级联
var str1 = "Hello";
var str2 = "Kitty";
var str3 = str1 + str2;
//中间夹个变量
console.log(x + " " + str1 + " " + y + " " + str2 + " " + z);
console.log(str3);
//比较运算(忽略>,<,>=,<=)
console.log("(1 ==  1.0):", 1 ==  1.0); //等于(类型不一定相等: 1 == "1")
console.log("(1 !=  1.0):", 1 !=  1.0); //不等于
console.log("(1 === 1.0):", 1 === 1.0); //等值等类型
console.log("(1 !== 1.0):", 1 !== 1.0); //不等值或不等类型
//逻辑运算(忽略&&,||,!)(忽略&,|,!,^)
//!!!JavaScript使用32位有符号数!!!
console.log("(-1 <<  1):", -1 <<  1);   //填0左位移
console.log("(-1 >>  1):", -1 >>  1);   //符号右位移
console.log("(-1 >>> 1):", -1 >>> 1);   //填0右位移
//复合赋值(+=，-=,*=,/=,%=,<<=,>>=,>>>=,&=,^=,|=,**=)
//函数1
var f1 = function(x, y) {
    return x ** y;
}
//函数2
function f2(x, y) {
    return x ** y;
}
console.log("2**3:", f1(2,3));
console.log("2**3:", f2(2,3));
//匿名函数(打印不执行函数,这里相当于传函数指针)
console.log("2**3:", function() {return 2**3});
//对象
var object = {
    type :"hello ketty",
    state:"so beautiful",
};
console.log(object, " ", object.type, " ", object.state);
//字符串方法(等等)
var str = "1 11 101 11 1 11 1"
//搜索
console.log("str.indexOf:",      str.indexOf("1"));
console.log("str.lastIndexOf:",  str.lastIndexOf("1"));
console.log("str.indexOf:",      str.indexOf("1", 2));
console.log("str.lastIndexOf:",  str.lastIndexOf("1", 18));
console.log("str.search:",       str.search("101"));
//提取
console.log("str.slice:",        str.slice(3, 7));       //3~7
console.log("str.slice:",        str.slice(-7, -3));     //逆向
console.log("str.slice:",        str.slice(3));          //3~end
console.log("str.slice:",        str.slice(-2));         //逆向~start
console.log("str.substring:",    str.substring(3, 7));   //3~7
console.log("str.substring:",    str.substring(3));      //3~end
console.log("str.substr:",       str.substr(3, 4));      //3~3 + 4
console.log("str.substr:",       str.substr(3));         //3~end
console.log("str.substr:",       str.substr(-5));        //逆向~start
//替换
console.log("str.replace",       str.replace("101", "010"));
//转化
var str = "  Hello Kitty  ";
console.log("str.toUpperCase:",  str.toUpperCase());
console.log("str.toLowerCase:",  str.toLowerCase());
//连接
console.log("str.concat:",       str.concat(" MIUSA "));
console.log("str.trim:",         str.trim());            //移除俩端留白
//定位
console.log("str.charAt:",       str.charAt(4));
console.log("str.charCodeAt:",   str.charCodeAt(4));
//匹配
console.log("str.includes:",     str.includes("Kitty"));
//数字字符串(除+)
console.log("100 * 10:",         "100" * "10");
console.log("100 / 10:",         "100" / "10");
console.log("100 - 10:",         "100" - "10");
//数字关键字: -Infinity , Infinity , NaN 
//数字方法
var number = 100.101;
console.log("100 / \"Kitty\":",          100 / "Kitty");
console.log("100 / 0:",                  100 / 0);
console.log("100 / 0:",                  100 / 0);
//方法
console.log("number.toString:",          (101.010).toString());
console.log("number.toExponential",      number.toExponential());    //转成指数形式
console.log("number.toExponential",      number.toExponential(5));   //保留指定小数位
console.log("number.toFixed",            number.toFixed());
console.log("number.toFixed",            number.toFixed(5));
console.log("number.toPrecision",        number.toPrecision());      //转成字符串
console.log("number.toPrecision",        number.toPrecision(3));     //指定保留位
console.log("number.valueOf:",           (102 + 3).valueOf());
//强转
console.log("Number:",                   Number(number));
console.log("Number:",                   parseFloat("101.010 202.020")); //从头解析
console.log("Number:",                   parseInt("101.010 202.020"));   //从头解析
//范围
console.log("MAX_VALUE:",                Number.MAX_VALUE);
console.log("MIN_VALUE:",                Number.MIN_VALUE);
console.log("POSITIVE_INFINITY:",        Number.POSITIVE_INFINITY);
console.log("NEGATIVE_INFINITY:",        Number.NEGATIVE_INFINITY);
console.log("NEGATIVE_INFINITY:",        Number.NEGATIVE_INFINITY);
console.log("NaN:",                      Number.NaN);
//数组
//它很像Python中的列表(可以疯狂嵌套,兼容万物)(所以部分不额外说)
var array  = [[1, [true]], "1", ["hello",[["world"],[function(){},{type:"object"}]]]];
var array1 = ["lala"];
var array2 = ["lala"];
console.log("array:" + array);
console.log("array:" + array.toString());
console.log("array:" + array.join("#")); //同上,挂分隔符
console.log("array:" + array.pop());         //尾移除
console.log("array:" + array.push("lala"));  //尾追加
console.log("array:" + array.shift());       //弹出首项,剩余左移
console.log("array:" + array.unshift("0"));  //追加首项,剩余右移
//console.log("array:" + array[array.length] = "lala");  //语法错误.未知
var array  = [[1, [true]], "1", ["hello",[["world"],[function(){},{type:"object"}]]]];
//第一个0表明下标0,第二个表明要移除元素个数(先移除后追加)
console.log("array:" + array.splice(0,0,"lala","lala"));
console.log("array:" + array.splice(0,2,"lala","lala"));
//连接不定个数的数组
console.log("array:" + array.concat(array1, array2));
//切割
console.log("array:" + array.slice(1));
console.log("array:" + array.slice(0, 3));
//排序
console.log("array:" + array.sort());    //默认转字符串排序
//翻转
console.log("array:" + array.sort().reverse());
var array1 = [40, 100, 1, 5, 25, 10];
console.log("array:" + array1.sort(function(a,b){return a-b;}));  //数字顺排序
console.log("array:" + array1.sort(function(a,b){return b-a;}));  //数字反排序
//洗牌排序(乱排序...排乱序...)
console.log("array:" + array1.sort(function(a,b){return 0.5-Math.random()}));
//找最大最小(引擎不支持)
//console.log("array:" + array1.sort(function(array){return Math.max.apply(null, arr);}));
//console.log("array:" + array1.sort(function(array){return Math.min.apply(null, arr);}));
//自定义比值函数可以排序各种各样的数组(对象数组,等等)
//找最大最小(这是自定义的数字数组的查找):
function function_max (array) {
    var max = -Infinity;
    for (let i in array)
        if (max < array[i])
            max = array[i];
    return max;
}

function function_min (array) {
    var min = Infinity;
    for (let i in array)
        if (min > array[i])
            min = array[i];
    return min;
}
//发现无效(???)(插个眼)(直接用上面的自定义都行了)
console.log("array:" + array1.sort(function_max));
console.log("array:" + array1.sort(function_min));
//数组迭代(不支持)
//console.log("array:" + array1.forEach(function_max));
//console.log("array:" + array1.forEach(function_min));
//数学
console.log("math.E:",           Math.E);
console.log("math.PI:",          Math.PI);
console.log("math.SQRT2:",       Math.SQRT2);
console.log("math.SQRT1_2:",     Math.SQRT1_2);
console.log("math.LN2:",         Math.LN2);
console.log("math.LN10:",        Math.LN10);
console.log("math.LOG2E:",       Math.LOG2E);
console.log("math.LOG10E:",      Math.LOG10E);
//
console.log("math.abs:",         Math.abs(-4.7));
//取整和随机搭配即可获得随机整数,   Math.floor(Math.random(0,1) * 10); //[0,1) * 10 范围
console.log("math.ceil:",        Math.ceil(4.7));    //上取整
console.log("math.floor:",       Math.floor(4.7));   //下取整
console.log("math.round:",       Math.round(4.7));   //就近取整
console.log("math.random:",      Math.random(0,1));  //[0,1)
console.log("math.min:",         Math.min(0, 450, 35, 10, -8, -300, -78));
console.log("math.max:",         Math.max(0, 450, 35, 10, -8, -300, -78));
//
console.log("math.pow:",         Math.pow(8,2));
console.log("math.exp:",         Math.exp(1));       //e**x
console.log("math.sqrt:",        Math.sqrt(8));
console.log("math.log:",         Math.log(10));      //以e为底
console.log("math.sin:",         Math.sin(90 / 180 * Math.PI));
console.log("math.cos:",         Math.cos(0  / 180 * Math.PI));
console.log("math.tan:",         Math.tan(45 / 180 * Math.PI));
console.log("math.asin:",        Math.asin(Math.sin(90 / 180 * Math.PI)));
console.log("math.acos:",        Math.acos(Math.cos(0  / 180 * Math.PI)));
console.log("math.atan:",        Math.atan(Math.tan(45 / 180 * Math.PI)));
console.log("math.atan2:",       Math.atan2(3, 2)); //坐标(y,x)求正切
//逻辑,比较与C相近
// (case,default,break,continue)与C形式完全一致
//if else   与C形式完全一致
// switch   与C形式完全一致(严格比较===,等值等类型)
// 基本for  与C形式完全一致          for(i=0;i<length;i++){...}
// while    与C形式完全一致
// do while 与C形式完全一致

// break    添加标签 ~== C形式goto
//for迭代器与Python形式相近
var array = [1, true, "1", function(){}, {type:object}, "hello kitty"];
var object = {type:"object",string:"hello kitty",value:true,data:1234};
//遍历对象属性
console.log("object:")
for (let i in object) {
    console.log(i+":",object["" + i]);  //转为字符串
    console.log(" ");
}
console.log("");

//遍历数组
console.log("array:")
for (let i in array) {
    console.log(array[i]);
    console.log(" ");
}
console.log("");

//of不支持迭代对象!
//不要随意使用of,使用上述的in
//console.log("array:")
//for (let i of array) {
//    console.log(array[i]);
//    console.log(" ");
//}
//console.log("");

//类型检查
console.log("typeof:", typeof object);     //这是一个对象类型
console.log("typeof:", typeof array);      //这是一个数组类型
//基本位运算(与C形式完全一致)
//特殊位运算操作
console.log("parseInt:", (100102 >>> 0).toString(2));        //10进制转2进制
console.log("parseInt:", parseInt(100102, 2).toString(10));  //10进制转2进制
//字符串正则表达式:
var string = "hello kitty";
console.log("str.search:",    str.search(/Kitty/));
console.log("str.search:",    str.search(/kitty/i));
console.log("str.replace:",   str.replace(/kitty/i, "Kitty"));
//正则表达式规则(注意在notepad++中颜色的变化)
//第一个项是模式,后面是可选的修饰符
//见:https://www.w3school.com.cn/js/js_regexp.asp测试案例

//异常(与Java或Python的形式类似或一致)
try {
    //可以自己丢出异常(注意这里投掷的是字符串对象)
    //也就是说error就是下述的字符串
    throw    "it was a deliberate mistake: from throw";
    //当然也可以这样子投掷(这相当于创建一个对象,其内容与error一致)
    throw {name   : SyntaxError,
           message:"it was a deliberate mistake: from throw"};
    
    //这个函数可以强行制造异常
    adddlert("it was a deliberate mistake: from adddlert");
}
catch(error) {
    //注意:系统投掷的error是一个对象,有下述俩个属性
    //第一个属性表明异常的类型,第二个属性是投掷的内容
    //error.name:
    //EvalError:        eval()中发生的错误
    //RangeError:       数字范围溢出
    //ReferenceError:   未定义的引用
    //SyntaxError:      语法错误
    //TypeError:        类型错误
    //URIError:         encodeURI()中发生的错误
    //建议把它们都打印出来:
    console.log("what happend?:", error);
    console.log("what happend?:", error.name, error.message);
}
finally {
    console.log("it has to be here");
}

//使用严格模式,是否有效?
try {
    "use strict";
    variable = 0;   //先引用
    var variable;   //后定义
}
catch(error) {
    //如果抛出异常,说明严格模式有效,否则就是常用的提升
    console.log("use strict finished");
}

//this指针(参考C++语法)
var variable    = this;     //特殊:指向全局对象
var object      = {         //this 指向本地对象
    type    :"object",
    string  :"hello kitty",
    value   :true,
    data    :1234,
    test    : function () {
        console.log("object:",    this);
        console.log(" type:",    this.type);
        console.log(" string:",  this.string);
        console.log(" value:",   this.value);
        console.log(" data:",    this.data);
        console.log("");
    },
    //对象访问器get,让函数以属性形式访问
    //测试发现,Jerry对set不支持,所以
    //还是使用上述函数的方式做不会有任何问题
    //get get_object() {
    //    this.test();    //这里偷了懒
    //},
    //对象访问器set,让函数以属性形式访问
    //set set_object() {  //这里偷了懒,要用参数传参,这里固定了
    //    this.type   = "-object-";
    //    this.string = "-hello kitty-";
    //    this.value  = true;
    //    this.data   = 1234;
    //},
    //对象访问器不唯一
    //get get_xxx() { /*...*/ },
    //set set_xxx() { /*...*/ },
}


var object1     = {
    type    :"object",
    string  :"hello kitty",
    value   :true,
    data    :1234,
}
//测试
object.test();      //函数是函数形式访问
//对象访问器测试
//object.set_object;  //访问器使用是属性形式访问
//object.get_object;  //访问器使用是属性形式访问
object.test.call(object1);  //对象使用自己的方法解析其他对象
//对象构造器(使用new生成(不建议使用))
//静态定义并使用方法(推荐使用)
function object_constructor() {
   this.type   = "object";
   this.string = "hello kitty";
   this.value  = true;
   this.data   = 1234;
}
//对象构造器动态添加方法(或者属性)
object_constructor.prototype.other = "halo";
object_constructor.prototype.test  = function(){};
//使用对象构造器创建对象(暂未知可行性,可用)
var object1 = object_constructor();
object.test.call(object1);

/* 详用见网站,未测试
//管理对象的方法
Object.create()                                     // 以现有对象为原型创建对象
Object.defineProperty(object, property, descriptor) // 添加或更改对象属性
Object.defineProperties(object, descriptors)        // 添加或更改对象属性
Object.getOwnPropertyDescriptor(object, property)   // 访问属性
Object.getOwnPropertyNames(object)                  // 以数组返回所有属性
Object.getPrototypeOf(object)                       // 访问原型
Object.keys(object)                                 // 以数组返回可枚举属性
//保护对象的方法
Object.preventExtensions(object)                    // 防止向对象添加属性
Object.isExtensible(object)                         // 如果属性可以添加到对象,则返回 true
Object.seal(object)                                 // 防止更改对象属性(不是值)
Object.isSealed(object)                             // 如果对象被密封,则返回 true
Object.freeze(object)                               // 防止对对象进行任何更改
Object.isFrozen(object)                             // 如果对象被冻结,则返回 true
*/
//Map对象,Set对象,使用应该较少,详情见网站,未测试




//箭头函数
var f1 = function() {
    //这里的this指针表示调用函数的对象;
    return "Ordinary function";
}
var f2 = () => {
    //这里的this指针表示函数的拥有者
    return "arrow function";
    return "arrow function";
}
var f3 = () => "special arrow function";    //只有一条语句时
var f4 = (argument) => "special arrow function get:" + argument;
//
console.log(f1());
console.log(f2());
console.log(f3());
console.log(f4("halo"));

//类(前面的都只是对象:对象是实例,类是模板;参考Java)
//类需要new关键字生成对象,Jerry对new关键字不支持
//所以可用不使用类,直接定义对象并指定关联即可,在环境中类不一定使用到
var array = ["object", true, 1234];
class classname {
    //构造器函数(好习惯是必写该函数)
    //参考Python(__init__),但它不具备析构函数
    constructor(array) {    //这里是测试,使用数组传
        this.type  = array[0];
        this.value = array[1];
        this.data  = array[2];
    }
    //方法(无需加function)
    test() {
        console.log("object:",   this);
        console.log("type:",    this.type);
        console.log("value:",   this.value);
        console.log("data:",    this.data);
        console.log("");
    }
}
//创建对象(使用数组初始化)
//var object = calssname.constructor(array);
//测试
//object.test();

//回调,将函数名作为参数传导即可
var f1 = function () {
    return "i am function 1";
}
var f2 = function(f) {
    return f();
}

//回调与异步
//其他细节关键字详情参见网站

//Promise对象,忽略(使用new生成,一个类)
//承诺对象类似于生产者消费者的关系,模拟写一份简要且够用即可

//JSON: JavaScript Object Notation
//JSON转JavaScript
//JSON格式完全是JavaScript的,其类型与JavaScript也一致
//可参考Python的方式转,一层一层解析

//闭包:
var count = (function() {
    var counter = 0;
    return function() {
        return counter += 1;
    }
})();
console.log("count:", count());
console.log("count:", count());
console.log("count:", count());

//上述的代码最终可分解为如下
/*
var count;
count = function () {
    var counter = 0;
    //更新函数对象后,上述的计数值依然有效
    //下一次函数入口被定位到此处
    count = function() {
        return counter += 1;
    }
}
*/

//console.log(instanceof object); //对象类型的实例(编译不过)

