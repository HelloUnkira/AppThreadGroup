/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include "Tetris.h"
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 矩阵索引转换 */
static inline TetrisBlock * Tetris_MatrixOffset(Tetris *Instance, uint32_t X, uint32_t Y)
{
    return &Instance->Matrix[Instance->Elements * Y + X];
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 从图元集合中获取图元 */
static inline void Tetris_UpdateGraph(Tetris *Instance, bool UpdateAll)
{
    /* 获得图元存储地址 */
    bool *Address = TetrisGraph_GetGraph(Instance->Type, Instance->Wheel);
    /* 迁移图元到目标点 */
    for (uint8_t I = 0; I < Instance->Scale; I++)
    for (uint8_t J = 0; J < Instance->Scale; J++)
        Instance->Graph[I][J].Status = Address[Instance->Scale * I + J];
    
    if (UpdateAll)
    {
        /* 获得图元存储地址 */
        bool *Address = TetrisGraph_GetGraph(Instance->TypeNext, Instance->WheelNext);
        /* 迁移图元到目标点 */
        for (uint8_t I = 0; I < Instance->ScaleNext; I++)
        for (uint8_t J = 0; J < Instance->ScaleNext; J++)
            Instance->GraphNext[I][J].Status = Address[Instance->ScaleNext * I + J];
    }
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 检查图元是否落在矩阵的有效位置内 */
static bool Tetris_PositionCheck(Tetris *Instance)
{
    for (uint8_t Y = 0; Y < Instance->Scale; Y++)
    for (uint8_t X = 0; X < Instance->Scale; X++)
        if (Instance->Graph[Y][X].Status) {
            /* 检查图元左上角和右下角是否在矩阵范围内 */
            if (Instance->Elements <= (uint32_t)(Instance->PosX + X) ||
                Instance->Lines    <= (uint32_t)(Instance->PosY + Y))
                return false;
            /* 检查目标图元与矩阵是否相交 */
            if (Tetris_MatrixOffset(Instance, Instance->PosX + X, Instance->PosY + Y)->Status)
                return false;
        }
    /* 检查结束反馈 */
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 将图元放在矩阵的有效位置内(仅当图元必须固定时使用) */
static void Tetris_PositionFixup(Tetris *Instance)
{
    for (uint8_t Y = 0; Y < Instance->Scale; Y++)
    for (uint8_t X = 0; X < Instance->Scale; X++)
        if (Instance->Graph[Y][X].Status) {
            *Tetris_MatrixOffset(Instance, Instance->PosX + X, Instance->PosY + Y) =
             Instance->Graph[Y][X];
            Instance->Graph[Y][X].Status = false;
        }
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 检查矩阵是否存在可消除行,如果有则迭代消除 */
static bool Tetris_LineErase(Tetris *Instance)
{
    bool EraseLine = false;
    /* 扫描当前行线 */
    for (uint32_t K = Instance->Lines - 1; K > 0; K--) {
        /* 轮转检查指定行 */
        bool EraseLineCheck = false;
        while (true) {
            for (uint32_t J = 0; J < Instance->Elements; J++)
                if (Tetris_MatrixOffset(Instance, J, K)->Status == false) {
                    EraseLineCheck = true;
                    break;
                }
            if (EraseLineCheck)
                break;
            /* 当前行需要消除 */
            for (uint32_t I = K; I - 1 > 0; I--)
            for (uint32_t J = 0; J < Instance->Elements; J++)
                *Tetris_MatrixOffset(Instance, J, I) = 
                *Tetris_MatrixOffset(Instance, J, I - 1);
            /* 最上行填充空 */
            for (uint32_t J = 0; J < Instance->Elements; J++)
                Tetris_MatrixOffset(Instance, J, 0)->Status = 0;
            /* 至少消除了一行 */
            Instance->EraseLine += 1;
            EraseLine = true;
        }
    }
    return EraseLine;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 准备一个随机图元 */
static bool Tetris_ReadyGraph(Tetris *Instance)
{
    Instance->Type  = Instance->TypeNext;
    Instance->Wheel = Instance->WheelNext;
    Instance->Scale = Instance->ScaleNext;
    
    /* 拷贝外部记录 */
    for (uint8_t I = 0; I < Instance->Scale; I++)
    for (uint8_t J = 0; J < Instance->Scale; J++)
        Instance->Graph[I][J] = Instance->GraphNext[I][J];
    
    Instance->TypeNext  = Instance->GetType();
    Instance->WheelNext = 0;
    Instance->ScaleNext = TetrisGraph_GetScale(Instance->TypeNext);
    /* 载入外部记录 */
    TetrisBlock Record = {0};
    Instance->SetRecord(Instance->TypeNext, &Record);
    for (uint8_t I = 0; I < Instance->ScaleNext; I++)
    for (uint8_t J = 0; J < Instance->ScaleNext; J++)
        Instance->GraphNext[I][J] = Record;
    
    /* 载入目标图元 */
    Tetris_UpdateGraph(Instance, true);
    
    Instance->PosY = 0;
    Instance->PosX = 0;
    /* 图元点设置,并循环检查是否有效 */
    uint32_t Center = Instance->Elements / 2 - TetrisGraphScale / 2;
    /* 向左检查是否有空位可以填充 */
    for (Instance->PosX = Center; Instance->PosX >= 0 && Instance->PosX < Instance->Elements;
         Instance->PosX--)
        if (Tetris_PositionCheck(Instance))
            return true;
    /* 向右检查是否有空位可以填充 */
    for (Instance->PosX = Center; Instance->PosX >= 0 && Instance->PosX < Instance->Elements;
         Instance->PosX++)
        if (Tetris_PositionCheck(Instance))
            return true;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 目标图元向左移动,无法移动则不移动 */
bool Tetris_MoveLeft(Tetris *Instance)
{
    Instance->PosX--;
    if (Tetris_PositionCheck(Instance))
        return true;
    Instance->PosX++;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 目标图元向左移动,无法移动则不移动 */
bool Tetris_MoveRight(Tetris *Instance)
{
    Instance->PosX++;
    if (Tetris_PositionCheck(Instance))
        return true;
    Instance->PosX--;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 目标图元向下移动,无法移动则不移动 */
bool Tetris_MoveStep(Tetris *Instance)
{
    Instance->PosY++;
    if (Tetris_PositionCheck(Instance))
        return true;
    Instance->PosY--;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 目标图元向左旋转(逆时针),无法旋转则不旋转 */
bool Tetris_RotateLeft(Tetris *Instance)
{
    Instance->Wheel--;
    Tetris_UpdateGraph(Instance, false);
    if (Tetris_PositionCheck(Instance))
        return true;
    Instance->Wheel++;
    Tetris_UpdateGraph(Instance, false);
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 目标图元向右旋转(顺时针),无法旋转则不旋转 */
bool Tetris_RotateRight(Tetris *Instance)
{
    Instance->Wheel++;
    Tetris_UpdateGraph(Instance, false);
    if (Tetris_PositionCheck(Instance))
        return true;
    Instance->Wheel--;
    Tetris_UpdateGraph(Instance, false);
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Tetris_Ready(Tetris *Instance)
{
    /* 生成下一图元 */
    Instance->TypeNext  = Instance->GetType();
    Instance->WheelNext = 0;
    Instance->ScaleNext = TetrisGraph_GetScale(Instance->TypeNext);
    /* 载入外部记录 */
    TetrisBlock Record = {0};
    Instance->SetRecord(Instance->TypeNext, &Record);
    for (uint8_t I = 0; I < Instance->ScaleNext; I++)
    for (uint8_t J = 0; J < Instance->ScaleNext; J++)
        Instance->GraphNext[I][J] = Record;
    /* 生成随机图元 */
    return Tetris_ReadyGraph(Instance);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
/* 定时事件循环(它需要从已经准备好的流程开始执行) */
bool Tetris_Iterate(Tetris *Instance)
{
    if (Instance->Step == 0) {
        /* 如果图元无效,表明游戏结束 */
        if (!Tetris_PositionCheck(Instance))
            return false;
        /* 如果可以进动一步,则进动它 */
        if (!Tetris_MoveStep(Instance))
            Instance->Step = 1;
        return true;
    }
    if (Instance->Step == 1) {
        /* 如果可以进动一步,则进动它 */
        /* (补充逻辑,因为存在通过旋转让其进动的可能性) */
        if (Tetris_MoveStep(Instance)) {
            Instance->Step = 0;
            return true;
        }
        /* 如果不能进动一步,则固定它并更新一个新的图元 */
        Tetris_PositionFixup(Instance);
        Instance->Step = 2;
        return true;
    }
    if (Instance->Step == 2) {
        /* 如果可以消除行,则消除它 */
        Tetris_LineErase(Instance);
        Instance->Step = 3;
        return true;
    }
    if (Instance->Step == 3) {
        /* 生成下一个图元 */
        Instance->Step = 0;
        return Tetris_ReadyGraph(Instance);
    }
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
