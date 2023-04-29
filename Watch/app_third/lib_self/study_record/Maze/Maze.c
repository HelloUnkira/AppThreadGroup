/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include "Maze.h"
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#define  MazeFactor (4 * 10)
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline MazeBlock * Maze_MatrixOffset(Maze *Instance, uint32_t PosX, uint32_t PosY)
{
    return &Instance->Matrix[Instance->Elements * PosY + PosX];
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static void Maze_ArrayUpdate(Maze *Instance, MazeCell **Array, uint32_t *Length)
{
    /* 起始时的生成 */
    if (*Array == NULL) {
        *Array  = Instance->Allocate(sizeof(MazeCell) * (MazeFactor));
        *Length = MazeFactor;
        return;
    }
    /* 结束时的回收 */
    if (*Length == 0) {
        Instance->Free(*Array);
        *Array  = NULL;
        return;
    }
    /* 中间时的扩展 */
    MazeCell *ArrayOld = *Array;
    *Array = Instance->Allocate(sizeof(MazeCell) * (*Length + MazeFactor));
    for (uint32_t Index = 0; Index < *Length; Index++)
        (*Array)[Index] = ArrayOld[Index];
    Instance->Free(ArrayOld);
    *Length += MazeFactor;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static bool Maze_ArrayCellExist(MazeCell *Array, uint32_t Length, MazeCell *Target)
{
    for (uint32_t Index = 0; Index < Length; Index++)
        if (Target->PosX == Array[Index].PosX && Target->PosY == Array[Index].PosY)
            return true;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static bool Maze_Routine(Maze *Instance)
{
    /* 迷宫最少要3*3的阵列方可工作 */
    if (Instance->Elements <= 2 || Instance->Lines <= 2)
        return false;
    /* 生成一个起始数组 */
    uint32_t Number = 0;
    uint32_t Length = 0;
    MazeCell *CellSet = NULL;
    Maze_ArrayUpdate(Instance, &CellSet, &Length);
    /* 所有单元格均未访问,让迷宫全都是墙 */
    for (uint32_t PosY = 0; PosY < Instance->Lines; PosY++)
    for (uint32_t PosX = 0; PosX < Instance->Elements; PosX++) {
        Maze_MatrixOffset(Instance, PosX, PosY)->H_Status = 1;
        Maze_MatrixOffset(Instance, PosX, PosY)->V_Status = 1;
        Maze_MatrixOffset(Instance, PosX, PosY)->Visit = 0;
    }
    /* 清除边界,墙有效作用域为[0,Lines-2][0,Elements-2] */
    for (uint32_t PosX = 0; PosX < Instance->Elements; PosX++)
        Maze_MatrixOffset(Instance, PosX, Instance->Lines - 1)->H_Status = 0;
    for (uint32_t PosY = 0; PosY < Instance->Lines; PosY++)
        Maze_MatrixOffset(Instance, Instance->Elements - 1, PosY)->V_Status = 0;
    /* 进行初始化流程 */
    if (true) {
        /* 随机选定一个单元格 */
        MazeCell R_Cell = {0};
        R_Cell.PosX = Instance->Random(Instance->Elements),
        R_Cell.PosY = Instance->Random(Instance->Lines),
        /* 将单元格标记为通路 */
        Maze_MatrixOffset(Instance, R_Cell.PosX, R_Cell.PosY)->Visit = 1;
        /* 获得单元格的临接网格 */
        MazeCell A_Cell[4] = {{.PosX = R_Cell.PosX - 1, .PosY = R_Cell.PosY,},
                              {.PosX = R_Cell.PosX + 1, .PosY = R_Cell.PosY,},
                              {.PosX = R_Cell.PosX, .PosY = R_Cell.PosY - 1,},
                              {.PosX = R_Cell.PosX, .PosY = R_Cell.PosY + 1,}};
        /* 将单元格的临接网格加入到集合 */
        for (uint32_t Index = 0; Index < 4; Index++) {
            MazeCell Cell = A_Cell[Index];
            if (Cell.PosX < Instance->Elements && Cell.PosY < Instance->Lines)
                CellSet[Number++] = Cell;
        }
    }
    /* 开始循环刷新 */
    while (Number != 0) {
        /* 集合空间可用性检查 */
        if (Length - Number < 4)
            Maze_ArrayUpdate(Instance, &CellSet, &Length);
        /* 抓取一个随机目标单元格,并将其移除 */
        uint32_t R_Index = Instance->Random(Number);
        MazeCell R_Cell  = CellSet[R_Index];
        CellSet[R_Index] = CellSet[Number-- - 1];
        /* 当前单元格标记为已经访问 */
        Maze_MatrixOffset(Instance, R_Cell.PosX, R_Cell.PosY)->Visit = 1;
        /* 获得单元格的临接网格 */
        uint32_t I_Number  = 0;
        uint32_t I_Cell[4] = {0};
        MazeCell A_Cell[4] = {{.PosX = R_Cell.PosX - 1, .PosY = R_Cell.PosY,},
                              {.PosX = R_Cell.PosX + 1, .PosY = R_Cell.PosY,},
                              {.PosX = R_Cell.PosX, .PosY = R_Cell.PosY - 1,},
                              {.PosX = R_Cell.PosX, .PosY = R_Cell.PosY + 1,}};
       /* 检查该网格周围的网格存在性 */
        for (uint32_t Index = 0; Index < 4; Index++) {
            MazeCell Cell = A_Cell[Index];
            if ((Cell.PosX < Instance->Elements && Cell.PosY < Instance->Lines) &&
                (Maze_MatrixOffset(Instance, Cell.PosX, Cell.PosY)->Visit == 1))
                I_Cell[I_Number++] = Index;
        }
        /* 从有效目标中随机一个 */
        uint8_t I_Index = Instance->Random(I_Number);
        /* 移除当前格子与临格中间的墙 */
        switch (I_Cell[I_Index]) {
        case 0: /* 左边单元格 */
            Maze_MatrixOffset(Instance, R_Cell.PosX - 1, R_Cell.PosY)->V_Status = 0; break;
        case 1: /* 右边单元格 */
            Maze_MatrixOffset(Instance, R_Cell.PosX + 0, R_Cell.PosY)->V_Status = 0; break;
        case 2: /* 上面单元格 */
            Maze_MatrixOffset(Instance, R_Cell.PosX, R_Cell.PosY - 1)->H_Status = 0; break;
        case 3: /* 下面单元格 */
            Maze_MatrixOffset(Instance, R_Cell.PosX, R_Cell.PosY + 0)->H_Status = 0; break;
        default:    break;
        }
        /* 遍历四个随机临格,将未被访问的节点无重复加入 */
        for (uint32_t Index = 0; Index < 4; Index++) {
            MazeCell Cell = A_Cell[Index];
            /* 随机临格需要有效性,且未被访问掉 */
            if ((Cell.PosX >= Instance->Elements || Cell.PosY >= Instance->Lines) ||
                (Maze_MatrixOffset(Instance, Cell.PosX, Cell.PosY)->Visit == 1))
                continue;
            /* 随机临格无重复加入 */
            if (Maze_ArrayCellExist(CellSet, Number, &Cell) == false)
                CellSet[Number++] = Cell;
        }
    }
    /* 回收空间 */
    Length = 0;
    Maze_ArrayUpdate(Instance, &CellSet, &Length);
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static void Maze_CellUpdate(Maze *Instance, MazeCell *Cell, MazeBlock *Record)
{
    MazeBlock *Offset =  Maze_MatrixOffset(Instance, Cell->PosX, Cell->PosY);
    MazeBlock  Target = *Offset;
    
    *Offset = *Record;
    
    Offset->Visit = Target.Visit;
    Offset->H_Status = Target.H_Status;
    Offset->V_Status = Target.V_Status;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Maze_Ready(Maze *Instance)
{
    /* 生成起始节点与结束节点 */
    uint32_t I_Temp = 0;
    uint32_t I_Cell[4] = {0, 1, 2, 3};
    MazeCell A_Cell[4] = {{.PosX = 0, .PosY = 0,},
                          {.PosX = 0, .PosY = Instance->Lines - 1,},
                          {.PosX = Instance->Elements - 1, .PosY = 0,},
                          {.PosX = Instance->Elements - 1, .PosY = Instance->Lines - 1,}};
    /* 随机打乱四个编号 */
    for (uint32_t K = 0; K < 4; K++) {
        uint32_t I = Instance->Random(4);
        uint32_t J = Instance->Random(4);
        I_Temp    = I_Cell[I];
        I_Cell[I] = I_Cell[J];
        I_Cell[J] = I_Temp;
    }
    /* 取前俩个作为随机数 */
    Instance->PosS = A_Cell[I_Cell[0]];
    Instance->PosE = A_Cell[I_Cell[1]];
    Instance->PosC = Instance->PosS;
    /* 设置起始点和结束点Record */
    Maze_CellUpdate(Instance, &Instance->PosS, &Instance->RecordS);
    Maze_CellUpdate(Instance, &Instance->PosE, &Instance->RecordE);
    Maze_CellUpdate(Instance, &Instance->PosC, &Instance->RecordC);
    /* 生成随机图 */
    return Maze_Routine(Instance);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Maze_Execute(Maze *Instance, MazeWay Way)
{
    /* 获得目标位置 */
    MazeCell Cell = {0};
    switch (Way) {
    case Maze_Up: 
        Cell.PosX = Instance->PosC.PosX;
        Cell.PosY = Instance->PosC.PosY - 1;
        break;
    case Maze_Down:
        Cell.PosX = Instance->PosC.PosX;
        Cell.PosY = Instance->PosC.PosY + 1;
        break;
    case Maze_Left:
        Cell.PosX = Instance->PosC.PosX - 1;
        Cell.PosY = Instance->PosC.PosY;
        break;
    case Maze_Right:
        Cell.PosX = Instance->PosC.PosX + 1;
        Cell.PosY = Instance->PosC.PosY;
        break;
    default:
        return true;
    }
    /* 判断目标位置合法 */
    if (Cell.PosX >= Instance->Elements || Cell.PosY >= Instance->Lines)
        return true;
    /* 判断目标是否可到达 */
    bool TouchFlag = false;
    switch (Way) {
    case Maze_Up:
        if (Maze_MatrixOffset(Instance, Instance->PosC.PosX, Instance->PosC.PosY - 1)->H_Status == 0)
            TouchFlag = true;   break;
    case Maze_Down:
        if (Maze_MatrixOffset(Instance, Instance->PosC.PosX, Instance->PosC.PosY + 0)->H_Status == 0)
            TouchFlag = true;   break;
    case Maze_Left:
        if (Maze_MatrixOffset(Instance, Instance->PosC.PosX - 1, Instance->PosC.PosY)->V_Status == 0)
            TouchFlag = true;   break;
    case Maze_Right:
        if (Maze_MatrixOffset(Instance, Instance->PosC.PosX + 0, Instance->PosC.PosY)->V_Status == 0)
            TouchFlag = true;   break;
    }
    if (TouchFlag == false)
        return true;
    /* 判断目标是结束点 */
    if (Cell.PosX == Instance->PosE.PosX && Cell.PosY == Instance->PosE.PosY)
        return false;
    /* 现在将当前点设置为过程点 */
    Maze_CellUpdate(Instance, &Instance->PosC, &Instance->RecordT);
    /* 将过程点设置为当前点 */
    Instance->PosC = Cell;
    /* 刷新一次过程点,起始点和结束点 */
    Maze_CellUpdate(Instance, &Instance->PosS, &Instance->RecordS);
    Maze_CellUpdate(Instance, &Instance->PosE, &Instance->RecordE);
    Maze_CellUpdate(Instance, &Instance->PosC, &Instance->RecordC);
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
