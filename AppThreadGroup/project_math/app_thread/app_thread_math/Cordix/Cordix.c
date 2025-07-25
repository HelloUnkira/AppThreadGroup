#include <stdint.h>
#include <stdbool.h>

#include "CordixTable.h"

const double PIE = 3.14159265358;  //圆周率PIE(acos(-1))

//问题:C不支持浮点数移位,如果使用整形
//当i达到某一个值时,2^-i是一个非常小的值以至于
//x_current和y_current左移时都为0,此时迭代陷入到困境中
//Cordix算法(第一象限):
//x1 =  xcos(t) + ysin(t);
//y1 = -xsin(t) + ycos(t);
//矩阵表达式:                 简化为:
//[x1][ cos(t) sin(t)][X] ==> [x1]cos(t)[   1    tan(t)][X]
//[y1][-sin(t) cos(t)][Y] ==> [y1]cos(t)[-tan(t)   1   ][Y]

static void CordixAngleRun(double Angle, double Result[2])
{
    /* 利用换角公式化简Angle,将Angle归化到[-PAI/2,PAI/2] */
    double AngleFlag = 1.0;
    {
        double Period1 = 1.0 * PIE;
        double Period2 = 2.0 * PIE;
        /* 利用:sin(Angle) = sin(Angle +- 2PAI)化简 */
        /* 利用:cos(Angle) = cos(Angle +- 2PAI)化简 */
        while (Angle -  Period2 >= 0.0)
               Angle -= Period2;
        while (Angle +  Period2 <= 0.0)
               Angle += Period2;
        if (Angle -  Period2 > -Angle)
            Angle -= Period2;
        if (Angle +  Period2 < -Angle)
            Angle += Period2;
        /* 利用:-sin(Angle) = sin(Angle +- PAI)化简 */
        /* 利用:-cos(Angle) = cos(Angle +- PAI)化简 */
        if (Angle -  Period1 > -Angle) {
            Angle -= Period1;
            AngleFlag = -AngleFlag;
        }
        if (Angle +  Period1 < -Angle) {
            Angle += Period1;
            AngleFlag = -AngleFlag;
        }
    }
    /* 计算补偿因子:SUM(I=(0->N-1)) (1 + 2**(-2I))**(-1/2) */
    /* 初始化状态量 */
    double Factor = 1.0;
    double NextX = 0.0, CurrentX = 0.60725935;
    double NextY = 0.0, CurrentY = 0.0;
    /* 开始逆时针旋转 */
    for (uint32_t I = 0; I < CORDIX_TABLE_SIZE; I++) {
        /* (注意:因为C不支持浮点数移位,这里还是只能除) */
        double Flag = Angle > 0.0 ? +1.0 : -1.0;
        /* 逆时针伪旋转公式 */
        NextX = CurrentX - Flag * CurrentY * Factor;
        NextY = CurrentY + Flag * CurrentX * Factor;
        Angle -= Flag * ArctanTable[I];
        CurrentX = NextX;
        CurrentY = NextY;
        Factor  *= 0.5;
    }
    Result[0] = AngleFlag * CurrentY;
    Result[1] = AngleFlag * CurrentX;
}

double CordixSin(double Angle)
{
    double Result[2] = {0};
    CordixAngleRun(Angle, Result);
    return Result[0];
}

double CordixCos(double Angle)
{    
    double Result[2] = {0};
    CordixAngleRun(Angle, Result);
    return Result[1];
}

double CordixTan(double Angle)
{
    double Result[2] = {0};
    CordixAngleRun(Angle, Result);
    return Result[0] / Result[1];
}

double CordixArctan(double Y, double X)
{
    uint8_t AngleFlag = 0;
    /* 微0值默认为正零值,这符合常规语义 */
    (-1E-6 <= X && X <= +1E-6) && (X = 0.0);
    (-1E-6 <= Y && Y <= +1E-6) && (Y = 0.0);
    /* 坐标归化为第一象限的值 */
    ((X < 0 && Y > 0) && (X = -X, AngleFlag = 1));
    ((X < 0 && Y < 0) && (X = -X, Y = -Y, AngleFlag = 2));
    ((X > 0 && Y < 0) && (Y = -Y, AngleFlag = 3));
    /* 初始化状态量 */
    double Result = 0.0;
    double Factor = 1.0;
    double NextX  = 0.0, CurrentX = X;
    double NextY  = 0.0, CurrentY = Y;
    /* 开始旋转 */
    for (uint32_t I = 0; I < CORDIX_TABLE_SIZE; I++) {
        /* (注意:因为C不支持浮点数移位,这里还是只能除) */
        double Flag = CurrentY > 0.0 ? +1.0 : -1.0;
        /* 逆时针伪旋转公式 */
        NextX = CurrentX + Flag * CurrentY * Factor;
        NextY = CurrentY - Flag * CurrentX * Factor;
        Result += Flag * ArctanTable[I];
        CurrentX = NextX;
        CurrentY = NextY;
        Factor  *= 0.5;
    }
    /* 回补角 */
    if (AngleFlag == 1)
        Result = PIE - Result;
    if (AngleFlag == 2)
        Result = Result - PIE;
    if (AngleFlag == 3)
        Result = 0.0 - Result;
    return Result;
}
