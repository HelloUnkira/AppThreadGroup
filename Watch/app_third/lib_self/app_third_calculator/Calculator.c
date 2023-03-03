/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#define   CALCULATOR_C
#include "Calculator.h"
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline bool CalculateIsDigit(char C)  {return ('0' <= C && C <= '9');}
static inline bool CalculateIsAlpha(char C)  {return ('A' <= C && C <= 'Z') ||
                                                     ('a' <= C && C <= 'z');}
static inline bool CalculateIsAlNum(char C)  {return ('A' <= C && C <= 'Z') ||
                                                     ('a' <= C && C <= 'z') ||
                                                     ('0' <= C && C <= '9');}
static inline bool CalculateIsBlank(char C)  {return (' ' == C || '\t' == C || '\n' == C);}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 添加部分自定义的函数以作扩展 */
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 自定义阶乘运算:x! */
static double factorial(int64_t x)
{
    //0! == 1 && 1! == 1
    if (x < 0) return 0.0;
    if (x < 2) return 1.0;
    
    double Result = 1.0;
    for (int64_t Index = 1; Index <= x; Result *= Index, Index++);
    return Result;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 自定义2阶乘运算:x!! */
static double factorial2(int64_t x)
{
    if (x < 0) return 0.0;
    //0!! == 1 && 1!! == 1
    if (x < 2) return 1.0;
    
    double Result = 1.0;
    for (int64_t Index  = (x % 2 != 0) ? 1 : ((x % 2 == 0) ? 2 : 0);
                 Index <= x; Result *= Index, Index += 2);
    return Result;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 自定义组合运算c(k,n) */
static double combination(int64_t k, int64_t n)
{
    if (k > n) return 0;
    /* 这里直接用上述自定义阶乘算即可 */
    return (double)((uint64_t)(factorial(n) / (factorial(k) * factorial(n - k))));
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline void * CalculatorMalloc(uint32_t size)
{
#ifndef CALCULATE_MALLOC
#error "CALCULATE_MALLOC: Dynamic memory policies must be supported"
#endif
    return CALCULATE_MALLOC(size);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline void CalculatorFree(void *pointer)
{
#ifndef CALCULATE_FREE
#error "CALCULATE_FREE: Dynamic memory policies must be supported"
#endif
    CALCULATE_FREE(pointer);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 符号记录(表明支持的基本运算规则): */
enum {
    DEFAULT         = 0,
    SUCCESS         = 1,
    ERROR           = 2,
    TYPEDATA        ,
    TYPEFLAG        ,
    /* 内置解析的括号 */
    I_Left          ,
    I_Right         ,
    I_Comma         ,
    /* 默认几个数学符号: */
    D_E             ,   // E
    D_PAI           ,   // PAI
    /* 基础运算类 */
    B_Plus          ,   // +(正号)
    B_Minus         ,   // -(负号)
    B_Add           ,   // +
    B_Subtract      ,   // -
    B_Multiply      ,   // *
    B_Devide        ,   // /
    /* 自定义运算 */
    C_Fact          ,   // double Factorial(int64_t n)
    C_Fact2         ,   // double Factorial2(int64_t n)
    C_Combi         ,   // double Combination(int64_t k, int64_t n)
    /* 扩展运算1类 */
    E_FMOD          ,   // double fmod(double x, double y)
    E_POW           ,   // double pow(double x, double y)
    E_FABS          ,   // double fabs(double x)
    /* 扩展运算2类 */
    E_CEIL          ,   // double ceil(double x)
    E_FLOOR         ,   // double floor(double x)
    /* 扩展运算3类 */
    E_SQRT          ,   // double sqrt(double x)
    /* 扩展运算4.1类 */
    E_COS           ,   // double cos(double x)
    E_SIN           ,   // double sin(double x)
    E_TAN           ,   // double tan(double x)
    /* 扩展运算4.2类 */
    E_COSH          ,   // double cosh(double x)
    E_SINH          ,   // double sinh(double x)
    E_TANH          ,   // double tanh(double x)
    /* 扩展运算4.3类 */
    E_ACOS          ,   // double acos(double x)
    E_ASIN          ,   // double asin(double x)
    E_ATAN          ,   // double atan(double x)
    E_ATAN2         ,   // double atan2(double y, double x)
    /* 扩展运算5类 */
    E_LOG           ,   // double log(double x)
    E_LOG10         ,   // double log10(double x)
    /* 扩展运算6类 */
    E_EXP           ,   // double exp(double x)
    /* 其余扩展后续支持 */
    E_FREXP         ,   // double frexp(double x, int *exponent)
    E_LDEXP         ,   // double ldexp(double x, int exponent)
    E_MODF          ,   // double modf(double x, double *integer)
    /*  */
} ExpressionFlag;
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 符号查阅表 */
const struct {uint32_t Length; const char *String; uint8_t Flag; uint8_t Number;} StringFlag[] = {
    /* 数学符号: */
    {2, "E",        D_E,            0},
    {2, "e",        D_E,            0},
    {4, "PAI",      D_PAI,          0},
    {4, "pai",      D_PAI,          0},
    /* 基本运算 */
    {0, "",         B_Plus,         1},
    {0, "",         B_Minus,        1},
    {0, "",         B_Add,          2},
    {0, "",         B_Subtract,     2},
    {0, "",         B_Multiply,     2},
    {0, "",         B_Devide,       2},
    /* 自定义运算 */
    {2, "c",        C_Combi,        2},
    {0, "",         C_Fact,         1},
    {0, "",         C_Fact2,        1},
    /* 扩展运算1类 */
    {5, "fmod",     E_FMOD,         2},
    {5, "fabs",     E_FABS,         1},
    {4, "pow",      E_POW,          2},
    /* 扩展运算2类 */
    {5, "ceil",     E_CEIL,         1},
    {6, "floor",    E_FLOOR,        1},
    /* 扩展运算3类 */
    {5, "sqrt",     E_SQRT,         1},
    /* 扩展运算4类 */
    {5, "asin",     E_ASIN,         1},
    {5, "acos",     E_ACOS,         1},
    {5, "atan",     E_ATAN,         1},
    {6, "atan2",    E_ATAN2,        2},
    {4, "cos",      E_COS,          1},
    {5, "cosh",     E_COSH,         1},
    {4, "sin",      E_SIN,          1},
    {5, "sinh",     E_SINH,         1},
    {4, "tan",      E_TAN,          1},
    {5, "tanh",     E_TANH,         1},
    /* 扩展运算5类 */
    {4, "log",      E_LOG,          1},
    {6, "log10",    E_LOG10,        1},
    /* 扩展运算6类 */
    {4, "exp",      E_EXP,          1},
};
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#define StringFlagLength  (sizeof(StringFlag) / sizeof(StringFlag[0]))
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 通过基础配置和伸缩因子生成初始栈或者扩展栈 */
static void CalculatorStack(uint8_t Type, void **Stack, uint32_t *Length)
{
    /* 初始化时的生成栈 */
    if (*Stack == NULL) {
        *Stack  = CalculatorMalloc(Type * CALCULATE_LENGTH_BASE);
        *Length = CALCULATE_LENGTH_BASE;
        return;
    }
    /* 结束时的回收栈 */
    if (*Length == 0) {
        CalculatorFree(*Stack);
        *Stack  = NULL;
        *Length = 0;
        return;
    }
    /* 长度至少为基础栈长 */
    if (*Length < CALCULATE_LENGTH_BASE)
        *Length = CALCULATE_LENGTH_BASE;
    /* 计算新栈长 */
    uint32_t LengthNew = *Length + CALCULATE_LENGTH_FACTOR;
    /* 生成新栈 */
    void *StackNew = CalculatorMalloc(Type * LengthNew);
    /* 将原生栈信息拷贝到新栈中 */
    if (Type == sizeof(double))
        for (uint32_t Index = 0; Index < *Length; Index++)
            ((double *)StackNew)[Index] = ((double *)(*Stack))[Index];
    if (Type == sizeof(uint8_t))
        for (uint32_t Index = 0; Index < *Length; Index++)
            ((uint8_t *)StackNew)[Index] = ((uint8_t *)(*Stack))[Index];
    /* 反馈新栈 */
    CalculatorFree(*Stack);
    *Stack  = StackNew;
    *Length = LengthNew;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 操作符所需操作数解析运算: */
static inline uint8_t CalculatorFlagNumber(uint8_t Type)
{
    for (uint32_t Index = 0; Index < StringFlagLength; Index++)
        if (StringFlag[Index].Flag == Type)
            return StringFlag[Index].Number;
    ERROR_PRINT(true, "CalculatorFlagNumber");
    return -1;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 将一个可能的字母起始的字符串解析为操作符 */
static inline bool CalculatorParseFlag(char *String, uint32_t Length, uint8_t *Flag)
{
    for (uint32_t Index = 0; Index < StringFlagLength; Index++) {
        if (Length != StringFlag[Index].Length)
            continue;
        if (strncmp(String, StringFlag[Index].String, Length) != 0)
            continue;
        *Flag = StringFlag[Index].Flag;
        return true;
    }
    /* 错误检查 */
    ERROR_PRINT(true, "CalculatorParseFlag:");
    ERROR_PRINT(true, String);
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 将一个可能的.或数字起始的字符串解析为数据 */
static inline bool CalculatorParseData(char *String, uint32_t Length, double *Result)
{
    uint32_t Index = 0;
    double PartInteger = 0.0;
    double PartDecimal = 0.0;
    double PartDecimalFactor = 1.0;
    /* 先解析整数部分 */
    while (Index < Length) {
        if (CalculateIsDigit(String[Index])) {
            PartInteger *= 10.0;
            PartInteger += String[Index++] - '0';
            continue;
        }
        if (String[Index] == '.')
            break;
        ERROR_PRINT(true, "CalculatorParseData:PartInteger");
        return false;
    }
    /* 再解析小数点 */
    if (String[Index] == '.')
        Index++;
    /* 再解析小数部分 */
    while (Index < Length) {
        if (CalculateIsDigit(String[Index])) {
            PartDecimalFactor *= 0.1;
            PartDecimal += (String[Index++] - '0') * PartDecimalFactor;
            continue;
        }
        ERROR_PRINT(true, "CalculatorParseData:PartDecimal");
        return false;
    }
    /* 拼接整数部分和小数部分 */
    *Result = PartInteger + PartDecimal;
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 接收上述指定类型参数完成结果计算反馈 */
/* 这里调用系统数学库,需要库作为基础外界支持 */
static inline bool CalculatorRun(uint8_t Type, double *Result, double *List)
{
    switch (Type) {
    /* 基本数解析 */
    case D_E:           *Result = exp(1);                       return true;
    case D_PAI:         *Result = acos(-1);                     return true;
    /* 基本运算解析 */
    case B_Plus:        *Result = List[0];                      return true;
    case B_Minus:       *Result = 0 - List[0];                  return true;
    case B_Add:         *Result = List[0] + List[1];            return true;
    case B_Subtract:    *Result = List[0] - List[1];            return true;
    case B_Multiply:    *Result = List[0] * List[1];            return true;
    case B_Devide:      *Result = List[0] / List[1];            return true;
    /* 自定义运算 */
    case C_Fact:
        *Result = factorial((int64_t)(List[0]));                return true;
    case C_Fact2:
        *Result = factorial2((int64_t)(List[0]));               return true;
    case C_Combi:
        *Result = combination((int64_t)(List[0]),
                              (int64_t)(List[1]));              return true;
    /* 扩展运算1类 */
    case E_FMOD:        *Result = fmod(List[0], List[1]);       return true;
    case E_POW:         *Result = pow(List[0], List[1]);        return true;
    case E_FABS:        *Result = fabs(List[0]);                return true;
    /* 扩展运算2类 */
    case E_CEIL:        *Result = ceil(List[0]);                return true;
    case E_FLOOR:       *Result = floor(List[0]);               return true;
    /* 扩展运算3类 */
    case E_SQRT:        *Result = sqrt(List[0]);                return true;
    /* 扩展运算4.1类 */
    case E_COS:         *Result = cos(List[0]);                 return true;
    case E_SIN:         *Result = sin(List[0]);                 return true;
    case E_TAN:         *Result = tan(List[0]);                 return true;
    /* 扩展运算4.2类 */
    case E_COSH:        *Result = cosh(List[0]);                return true;
    case E_SINH:        *Result = sinh(List[0]);                return true;
    case E_TANH:        *Result = tanh(List[0]);                return true;
    /* 扩展运算4.3类 */
    case E_ACOS:        *Result = acos(List[0]);                return true;
    case E_ASIN:        *Result = asin(List[0]);                return true;
    case E_ATAN:        *Result = atan(List[0]);                return true;
    case E_ATAN2:       *Result = atan2(List[0], List[1]);      return true;
    /* 扩展运算5类 */
    case E_LOG:         *Result = log(List[0]);                 return true;
    case E_LOG10:       *Result = log10(List[0]);               return true;
    /* 扩展运算6类 */
    case E_EXP:         *Result = exp(List[0]);                 return true;
    /* 其余扩展后续支持 */
    }
    ERROR_PRINT(1, "CalculatorRun:Type");
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 指定除()括号外其余运算符优先级规则 */
static inline uint8_t CalculatorGetPriority(uint8_t Type)
{
    /* ()的优先级最高 */
    if (Type == I_Left || Type == I_Right)
        return 10;
    /* 单目+-优先级仅次() */
    if (Type == B_Plus || Type == B_Minus)
        return 9;
    /* +-号优先级最低 */
    if (Type == B_Add || Type == B_Subtract)
        return 3;
    /* /*号优先级仅次+- */
    if (Type == B_Multiply || Type == B_Devide)
        return 4;
    /* 其他的优先级一致居中 */
    return 5;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 解析一个数据或符号并反馈为对应的类型 */
static inline uint8_t CalculatorParse(char *Expression, uint32_t *Index,
                                      uint8_t TypeLast, uint8_t FlagLast,
                                      double *Data,     uint8_t *Flag)
{
    if (Expression[*Index] == '\0')
        return SUCCESS;
    /* 1.先过滤一次留白 */
    while (CalculateIsBlank(Expression[*Index]) == true)
        (*Index) += 1;
    /* 额外添加**转义,如果出现**时转为pow */
    /* 额外添加!!转义,如果出现!!时转为二阶乘 */
    /* 额外添加!转义, 如果出现!时转为阶乘 */
    switch (strncmp(&Expression[*Index], "**", 2) == 0 ? 1 : 
            strncmp(&Expression[*Index], "!!", 2) == 0 ? 2 : 
            strncmp(&Expression[*Index], "!",  1) == 0 ? 3 : 0) {
    case 1: (*Index) += 2; *Flag = E_POW;   return TYPEFLAG;
    case 2: (*Index) += 2; *Flag = C_Fact2; return TYPEFLAG;
    case 3: (*Index) += 1; *Flag = C_Fact;  return TYPEFLAG;
    }
    /* 基础内置符号本地检测并反馈 */
    switch (Expression[*Index]) {
    case '(': (*Index)++; *Flag = I_Left;       return TYPEFLAG;
    case ')': (*Index)++; *Flag = I_Right;      return TYPEFLAG;
    case '*': (*Index)++; *Flag = B_Multiply;   return TYPEFLAG;
    case '/': (*Index)++; *Flag = B_Devide;     return TYPEFLAG;
    case '%': (*Index)++; *Flag = E_FMOD;       return TYPEFLAG;
    case ',': (*Index)++; *Flag = I_Comma;      return TYPEFLAG;
    //出现单目运算符时
    case '+': (*Index)++; 
        *Flag = (TypeLast == TYPEFLAG && FlagLast != I_Right) ? B_Plus :  B_Add;
        return TYPEFLAG;
    case '-': (*Index)++; //出现单目运算符时
        *Flag = (TypeLast == TYPEFLAG && FlagLast != I_Right) ? B_Minus : B_Subtract;
        return TYPEFLAG;
    }
    /* 准备进一步检测 */
    uint32_t IndexNew = *Index;
    /* 检测是出现了符号? */
    if (CalculateIsAlpha(Expression[IndexNew])) {
        /* 确认符号的范围 */
        while (CalculateIsAlNum(Expression[IndexNew]) ||
               CalculateIsBlank(Expression[IndexNew]))
               IndexNew++;
        /* 生成空间保留该片段,用于下一步的解析 */
        uint32_t Length = 0;
        char *String = (char *)CalculatorMalloc(IndexNew - *Index + 1);
        for (uint32_t IndexTemp = *Index; IndexTemp < IndexNew; IndexTemp++) {
            if (CalculateIsBlank(Expression[IndexTemp]) == true)
                continue;
            String[Length++] = Expression[IndexTemp];
        }
        /* 最后填充字符串尾缀 */
        String[Length++] = '\0';
        /* 解析符号 */
        if (CalculatorParseFlag(String, Length, Flag) == false) {
            ERROR_PRINT(true, "CalculatorParse:Flag");
            ERROR_PRINT(true, String);
            CalculatorFree(String);
            return ERROR;
        }
        /* 指针重定位 */
        *Index = IndexNew;
        /* 结束解析 */
        CalculatorFree(String);
        return TYPEFLAG;
    }
    /* 检测是出现了数字 */
    if (CalculateIsDigit(Expression[IndexNew]) || Expression[IndexNew] == '.') {
        /* 确认数字的范围 */
        while (CalculateIsDigit(Expression[IndexNew]) || Expression[IndexNew] == '.' ||
               CalculateIsBlank(Expression[IndexNew]))
               IndexNew++;
        /* 生成空间保留该片段,用于下一步的解析 */
        uint32_t Length = 0;
        char *String = (char *)CalculatorMalloc(IndexNew - *Index + 1);
        for (uint32_t IndexTemp = *Index; IndexTemp < IndexNew; IndexTemp++) {
            if (CalculateIsBlank(Expression[IndexTemp]) == true)
                continue;
            String[Length++] = Expression[IndexTemp];
        }
        /* 最后填充字符串尾缀 */
        String[Length] = '\0';
        /* 解析符号 */
        if (CalculatorParseData(String, Length, Data) == false) {
            ERROR_PRINT(true, "CalculatorParse:Number");
            ERROR_PRINT(true, String);
            CalculatorFree(String);
            return ERROR;
        }
        /* 指针重定位 */
        *Index = IndexNew;
        /* 结束解析 */
        CalculatorFree(String);
        return TYPEDATA;
    }
    ERROR_PRINT(true, "CalculatorParse:Unkonwn");
    return ERROR;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 表达式解析策略(堆栈模式) */
bool CalculatorMathExpression(char *Expression, double *Result, ErrorPrint Print)
{
    /* 生成数据暂存栈 */
    double  *NumberStack = NULL;
    uint32_t NumberMax   = 0;
    uint32_t NumberTop   = 0;
    /* 生成符号暂存栈 */
    uint8_t *FlagStack = NULL;
    uint32_t FlagMax   = 0;
    uint32_t FlagTop   = 0;
    /* 生成栈空间,初始化 */
    CalculatorStack(sizeof(double),  (void **)&NumberStack, &NumberMax);
    CalculatorStack(sizeof(uint8_t), (void **)&FlagStack,   &FlagMax);
    ERROR_PRINT(NumberStack == NULL || NumberMax == 0,
                "CalculatorMathExpression: NumberXxx");
    ERROR_PRINT(FlagStack   == NULL || FlagMax   == 0,
                "CalculatorMathExpression: FlagXxx");
    /* 后缀表达式计算 */
    uint8_t  Type   = DEFAULT;  //当前状态
    uint8_t  State  = DEFAULT;  //程序运行状态
    uint32_t Index  = 0;        //索引进动情况
    double   Data   = 0;        //数据暂存记录
    uint8_t  Flag   = DEFAULT;  //符号暂存记录
    /* 后缀表达式解析 */
    /* 先到符号栈压入一个左括号 */
    FlagStack[++FlagTop] = I_Left;
    uint8_t  TypeLast   = TYPEFLAG;      //上次状态暂存记录
    uint8_t  FlagLast   = I_Left; //上次状态暂存记录
    do {
        /* 计算到了末尾,假压入一个右括号,用于消除堆栈全部操作符 */
        if (Expression[Index] == '\0') {
            Flag  = I_Right;
            Type  = TYPEFLAG;
            State = SUCCESS;
        }
        /* 解析一个数据或符号 */
        if (Expression[Index] != '\0')
            Type = CalculatorParse(Expression, &Index, TypeLast, FlagLast, &Data, &Flag);
        /* 错误检查 */
        if (Type == ERROR) {
            char ErrorString[30] = {0};
            sprintf(ErrorString, "Error Index:%d", Index);
            ERROR_PRINT(true, ErrorString);
            Print(ErrorString);
            State == ERROR;
            break;
        }
        /* 如果解析到了一个数据 */
        if (Type == TYPEDATA) {
            /* 如果栈顶空间不够,扩展它 */
            if (++NumberTop > NumberMax)
                CalculatorStack(sizeof(double), (void **)&NumberStack, &NumberMax);
            /* 将数据加入到缓存栈的栈顶 */
            NumberStack[NumberTop] = Data;
        }
        /* 如果解析到了一个符号 */
        /* 运算规则1:左括号直接入栈 */
        /* 运算规则2:右括号入栈,弹出运算符到左括号为止 */
        /* 运算规则3:将当前优先级高的和相等的操作符弹出,直到遇见(或最低 */
        if (Type == TYPEFLAG) {
            /* 其他情况都要涉及操作符的弹出过程 */
            while (FlagTop > 0) {
                /* 左括号直接入栈 */
                if (Flag == I_Left) {
                    /* 如果栈顶空间不够,扩展它 */
                    if (++FlagTop > FlagMax)
                        CalculatorStack(sizeof(uint8_t), (void **)&FlagStack, &FlagMax);
                    /* 将符号压入到缓存栈的栈顶 */
                    FlagStack[FlagTop] = Flag;
                    break;
                }
                /* 当前优先级高于栈顶优先级直接入栈 */
                if (Flag != I_Right &&
                   (CalculatorGetPriority(Flag) >
                    CalculatorGetPriority(FlagStack[FlagTop]))) {
                    /* 如果栈顶空间不够,扩展它 */
                    if (++FlagTop >= FlagMax)
                        CalculatorStack(sizeof(uint8_t), (void **)&FlagStack, &FlagMax);
                    /* 将符号压入到缓存栈的栈顶 */
                    FlagStack[FlagTop] = Flag;
                    break;
                }
                /* 逗号表达式可直接丢弃 */
                if (FlagStack[FlagTop] == I_Comma) {
                    FlagTop--;
                    continue;
                }
                /* 遇到左括号时 */
                if (FlagStack[FlagTop] == I_Left) {
                    /* 右括号遇到第一个左括号停下,弹出左括号 */
                    if (Flag == I_Right) {
                        FlagTop--;
                        break;
                    }
                    /* 其他符号遇到左括号停下,压入符号 */
                    /* 如果栈顶空间不够,扩展它 */
                    if (++FlagTop >= FlagMax)
                        CalculatorStack(sizeof(uint8_t), (void **)&FlagStack, &FlagMax);
                    /* 将符号压入到缓存栈的栈顶 */
                    FlagStack[FlagTop] = Flag;
                    break;
                }
                /* 其他符号遇到低优先级的符号停下 */
                if ((Flag != I_Right) &&
                    (CalculatorGetPriority(Flag) >
                     CalculatorGetPriority(FlagStack[FlagTop]))) {
                    /* 如果栈顶空间不够,扩展它 */
                    if (++FlagTop >= FlagMax)
                        CalculatorStack(sizeof(uint8_t), (void **)&FlagStack, &FlagMax);
                    /* 将符号压入到缓存栈的栈顶 */
                    FlagStack[FlagTop] = Flag;
                    break;
                }

                /* 获取栈顶操作符,解析 */
                uint8_t Number = CalculatorFlagNumber(FlagStack[FlagTop]);
                /* 栈顶操作符错误 */
                if (Number == -1) {
                    char ErrorString[30] = {0};
                    sprintf(ErrorString, "Error Index:%d", Index);
                    ERROR_PRINT(true, ErrorString);
                    Print(ErrorString);
                    State == ERROR;
                    break;
                }
                /* 使用解析后的参数数量计算栈顶结果 */
                if (Number > NumberTop) {
                    char ErrorString[30] = {0};
                    sprintf(ErrorString, "Error Index:%d", Index);
                    ERROR_PRINT(true, ErrorString);
                    Print(ErrorString);
                    State == ERROR;
                    break;
                }
                /* 获取运算操作数 */
                /* 暂定最大接收5个操作数,目前实际最大只有2元运算符 */
                double List[5] = {0};
                /* 操作数逆向弹出 */
                if (Number >= 2)
                    List[1] = NumberStack[NumberTop--];
                if (Number >= 1)
                    List[0] = NumberStack[NumberTop--];
                if (Number == 0);
                /* 计算栈顶结果 */
                double ResultTemp = 0;
                if (CalculatorRun(FlagStack[FlagTop], &ResultTemp, List) == false) {
                    char ErrorString[30] = {0};
                    sprintf(ErrorString, "Error Index:%d", Index);
                    ERROR_PRINT(true, ErrorString);
                    Print(ErrorString);
                    State == ERROR;
                    break;
                }
                /* 栈顶结果重新入栈 */
                /* 如果栈顶空间不够,扩展它 */
                if (++NumberTop >= NumberMax)
                    CalculatorStack(sizeof(double), (void **)&NumberStack, &NumberMax);
                /* 将数据加入到缓存栈的栈顶 */
                NumberStack[NumberTop] = ResultTemp;
                /* 栈顶符号弹出 */
                FlagTop--;
            }
        }
        /* 保留状态 */
        TypeLast = Type;
        FlagLast = Flag;
    } while (State != SUCCESS && State != ERROR);
    /* 错误检查: */
    if (FlagTop != 0) {
        ERROR_PRINT(true, "ERROR: Flag Too Much\n");
        Print("ERROR: Flag Too Much\n");
        State = ERROR;
    }
    if (NumberTop != 1) {
        ERROR_PRINT(true, "ERROR: Number Too Much\n");
        Print("ERROR: Number Too Much\n");
        State = ERROR;
    }
    
    if (State == SUCCESS)
        *Result = NumberStack[NumberTop];
    /* 结束运行 */
    NumberMax = 0;
    FlagMax   = 0;
    /* 回收栈空间 */
    CalculatorStack(sizeof(double),  (void **)&NumberStack, &NumberMax);
    CalculatorStack(sizeof(uint8_t), (void **)&FlagStack,   &FlagMax);
    /* 计算结果反馈 */
    if (State != SUCCESS)
        return false;
    if (State == SUCCESS)
        return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
