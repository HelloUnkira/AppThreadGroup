
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ABSOLUTE_VALUE(Value) (Value > 0 ? Value : -Value)

/* 计算定积分 */
double Cal_DI(double (*F_X)(double X), double Left, double Right, uint32_t Precision, uint32_t Flag)
{
    if (F_X == NULL || Left > Right || Precision == 0)
        return 0.0;
    
    double Current = 0.0, Result = 0.0, Result1 = 0.0, Result2 = 0.0;
    double Delta1  = (Right - Left) / (double)Precision;
    double Delta2  = (Right - Left) / (double)Precision * 2.0;

    //梯形法公式:
    //(b-a)/n(y(1)+...+y(n-1)+y(0)/2+y(n)/2)
    //此时:--> Precision = n
    if (Flag == 1) {
        
        Current = Left;
        for (uint32_t Index = 1; Index < Precision; Index++) {
            Current += Delta1;
            Result  += F_X(Current);
        }
        
        Result += (F_X(Left) + F_X(Right)) / 2.0;
        Result *= Delta1;
    }
    
    //抛物线法公式(辛普森公式):
    //(b-a)/6n(y(0)+y(2n)+4(y1+...+y(2n-1))+2(y2+...+y(2n-2)))
    //此时:--> Precision = 2n
    if (Flag == 2) {
        if (Precision % 2 != 0)
            Precision += 1;

        //求奇数部分
        Current = Left + Delta1;
        for (uint32_t Index = 1; Index < Precision; Index += 2) {
            Result1 += F_X(Current);
            Current += Delta2;
        }
        //求偶数部分
        Current = Left + Delta2;
        for (uint32_t Index = 2; Index < Precision; Index += 2) {
            Result2 += F_X(Current);
            Current += Delta2;
        }
        
        Result += (F_X(Left) + F_X(Right));
        Result += Result1 * 4.0 + Result2 * 2.0;
        Result *= Delta1 / 3.0;
    }
    
    return Result;
}

/* 牛顿切线法求取方程近似解 */
double Cal_NT(double (*F0_X)(double X), double (*F1_X)(double X), double Start, double Precision)
{
    if (F0_X == NULL || F1_X == NULL || Precision == 0.0)
        return 0;

    double X = Start;

    Precision = ABSOLUTE_VALUE(Precision);
    while (1) {
        //计算x0多项式的值与一阶导数的值
        double X_LevelZero = F0_X(X);
        double X_LevelOne  = F1_X(X);
        //
        if (X_LevelOne == 0.0)
            break;
        //计算切点值
        double Temp = X_LevelZero / X_LevelOne;
        //
        if (ABSOLUTE_VALUE(Temp) <= Precision)
            break;
        //迭代
        double X_Next = X - Temp;
        X = X_Next;
    }
    return X;
}

/* 计算多项式SUM(k=0->n) ak * X**k (参数为a(0->n)) */

/* 数学扩展:泰勒展开式 */

//e**X = SUM(k=(0->n)) (X**k / k!)

//优化1:ln(X) = ln(t*10**index) = ln(t) + index*ln10
//优化2:ln(t) = ln((1 + a) / (1 - a)) = ln(1 + a) - ln(1 - a) ==> a = (t - 1) / (t + 1)
//ln(X) = ln((1 + t)/(1 - t)) = ln(1 + t) - ln(1 - t)   [X = (1 + t)/(1 - t),t = (X - 1)/(X + 1)]
//= 2 * t SUM(k=(0->n)) (t * t)**k / (2 * k + 1)

//优化1:利用:+sin(X) = sin(X +- 2PAI)化简 --- X=[-PAI,PAI]
//优化2:利用:-sin(X) = sin(X +-  PAI)化简 --- X=[-PAI/2,PAI/2]
//sin(X) = SUM(k=(0->n)) ((-1)**k X**(2*k+1) / (2*k+1)!)

//优化1:利用:+cos(X) = cos(X +- 2PAI)化简 --- X=[-PAI,PAI]
//优化2:利用:-cos(X) = cos(X +-  PAI)化简 --- X=[-PAI/2,PAI/2]
//cos(X) = SUM(k=(0->n)) ((-1)**k X**(2*k) / (2*k)!)

//atan(X) = SUM(k=(0->n)) ((-1)**k X**(2*k+1) / (2*k+1))

