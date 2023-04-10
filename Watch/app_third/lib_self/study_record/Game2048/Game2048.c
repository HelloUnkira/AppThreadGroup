/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include "Game2048.h"
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 矩阵索引转换 */
static inline uint8_t * Game2048_MatrixOffset(Game2048 *Instance, uint32_t X, uint32_t Y)
{
    return &Instance->Matrix[Instance->Scale * X + Y];
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 矩阵逆时针旋转(Left) */
static void Game2048_MatrixRotateLeft(Game2048 *Instance)
{
    uint32_t Edge1 = 0, Edge2 = Instance->Scale - 1;
    
    uint8_t Temp = 0;
    /* L:Left, R:Right, T:Top, B:Bottom */
    for (uint32_t L = Edge1, R = Edge2, T = Edge1, B = Edge2; T < B; L++, R--, T++, B--) {
        /* 对指定的圈进行旋转 */
        for (uint32_t I = 0; I < R - L; I++) {
            /* 交换四个数 */
            Temp = *Game2048_MatrixOffset(Instance, T, L + I);
            
            *Game2048_MatrixOffset(Instance, T, L + I) =
            *Game2048_MatrixOffset(Instance, T + I, R);
            
            *Game2048_MatrixOffset(Instance, T + I, R) =
            *Game2048_MatrixOffset(Instance, B, R - I);
            
            *Game2048_MatrixOffset(Instance, B, R - I) =
            *Game2048_MatrixOffset(Instance, B - I, L);
            
            *Game2048_MatrixOffset(Instance, B - I, L) = Temp;
        }
    }
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 矩阵顺时针旋转(Right) */
static void Game2048_MatrixRotateRight(Game2048 *Instance)
{
    uint32_t Edge1 = 0, Edge2 = Instance->Scale - 1;
    
    uint8_t Temp = 0;
    /* L:Left, R:Right, T:Top, B:Bottom */
    for (uint32_t L = Edge1, R = Edge2, T = Edge1, B = Edge2; T < B; L++, R--, T++, B--) {
        /* 对指定的圈进行旋转 */
        for (uint32_t I = 0; I < R - L; I++) {
            /* 交换四个数 */
            Temp = *Game2048_MatrixOffset(Instance, T, L + I);
            
            *Game2048_MatrixOffset(Instance, T, L + I) =
            *Game2048_MatrixOffset(Instance, B - I, L);
            
            *Game2048_MatrixOffset(Instance, B - I, L) =
            *Game2048_MatrixOffset(Instance, B, R - I);
            
            *Game2048_MatrixOffset(Instance, B, R - I) =
            *Game2048_MatrixOffset(Instance, T + I, R);
            
            *Game2048_MatrixOffset(Instance, T + I, R) = Temp;
        }
    }
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 按照指定方向合并矩阵 */
static bool Game2048_MatrixMerge(Game2048 *Instance, Game2048Way Way)
{
    /* 合并可以先通过矩阵的本地旋转之后再进行合并 */
    /* 虽然会损失一些微不足道的效率,但是这能将游戏语义归一化 */
    
    switch (Way) {
    case Game2048_Up:       Game2048_MatrixRotateLeft(Instance);    break;
    case Game2048_Down:     Game2048_MatrixRotateRight(Instance);   break;
    case Game2048_Left:                                             break;
    case Game2048_Right:    Game2048_MatrixRotateLeft(Instance);
                            Game2048_MatrixRotateLeft(Instance);    break;
                            //Game2048_MatrixRotateRight(Instance);
                            //Game2048_MatrixRotateRight(Instance);   break;
                            /* 对称语义,二选其一即可 */
    }
    
    bool MergeCatch = false;
    /* 这是向左合并的执行流程(这是最规整的语义) */
    for (uint32_t Line = 0; Line < Instance->Scale; Line++) {
        uint32_t Offset = 0;
        for (uint32_t Index = 0; Index < Instance->Scale; Index++)
            Instance->Buffer[Index] = 0;
        for (uint32_t Index = 0; Index < Instance->Scale; Index++) {
            uint8_t Target = *Game2048_MatrixOffset(Instance, Line, Index);
            if (Target == 0)
                continue;
            else if (Instance->Buffer[Offset] == 0) {
                Instance->Buffer[Offset] = Target;
                if (Offset != Index)
                    MergeCatch = true;
            } else if (Instance->Buffer[Offset] == Target) {
                Instance->Buffer[Offset++] += 1;
                    MergeCatch = true;
            } else {
                Instance->Buffer[++Offset] = Target;
                if (Offset != Index)
                    MergeCatch = true;
            }
        }
        for (uint32_t Index = 0; Index < Instance->Scale; Index++)
            *Game2048_MatrixOffset(Instance, Line, Index) = Instance->Buffer[Index];
    }
    
    switch (Way) {
    case Game2048_Up:       Game2048_MatrixRotateRight(Instance);   break;
    case Game2048_Down:     Game2048_MatrixRotateLeft(Instance);    break;
    case Game2048_Left:                                             break;
    case Game2048_Right:    //Game2048_MatrixRotateLeft(Instance);
                            //Game2048_MatrixRotateLeft(Instance);    break;
                            Game2048_MatrixRotateRight(Instance);
                            Game2048_MatrixRotateRight(Instance);   break;
                            /* 对称语义,二选其一即可 */
    }
    
    return MergeCatch;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 刷取一个新的元素 */
static bool Game2048_RandomOne(Game2048 *Instance, uint8_t Value)
{
    uint32_t VaildNumber = 0;
    uint32_t VaildTarget = 0;
    uint32_t VaildIndex  = 0;
    /* 先计算有多少个空余位置 */
    for (uint32_t I = 0; I < Instance->Scale; I++)
    for (uint32_t J = 0; J < Instance->Scale; J++)
        if (*Game2048_MatrixOffset(Instance, I, J) == 0)
            VaildNumber++;
    if (VaildNumber == 0)
        return false;
    /* 生成一个随机空余位置 */
    VaildTarget = Instance->Random(VaildNumber);
    /* 重新统计并找到目标位置 */
    for (uint32_t I = 0; I < Instance->Scale; I++)
    for (uint32_t J = 0; J < Instance->Scale; J++)
        if (*Game2048_MatrixOffset(Instance, I, J) == 0) {
            if (VaildTarget == VaildIndex) {
                *Game2048_MatrixOffset(Instance, I, J) = Value;
                return true;
            }
            VaildIndex++;
        }
    /* 从这里退出为意外情况 */
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Game2048_Ready(Game2048 *Instance, uint8_t Difficult)
{
    Instance->Difficult = Difficult;
    /* 后刷取新的元素 */
    for (uint32_t Count = 0; Count < Instance->Difficult; Count++)
        if (Game2048_RandomOne(Instance, 1))
            Instance->Status = 0;
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Game2048_Execute(Game2048 *Instance, Game2048Way Way)
{
    /* 先进行合并,不接受无效合并刷新 */
    if (!Game2048_MatrixMerge(Instance, Way)) {
        Instance->Status |= Way;
        if (Instance->Status == (Game2048_Up | Game2048_Down | Game2048_Left | Game2048_Right))
            return false;
        return true;
    }
    /* 后刷取新的元素 */
    bool RandomFlag = false;
    for (uint32_t Count = 0; Count < Instance->Difficult; Count++)
        if (Game2048_RandomOne(Instance, 1))
            Instance->Status = 0;
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
