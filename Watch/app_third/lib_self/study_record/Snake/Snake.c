/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
#include "Snake.h"
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
SnakeNode * Snake_GetHead(Snake *Instance)
{
    if (&Instance->List == NULL) return NULL;
    DL_Node *Head = DL_List_GetHead(&Instance->List);
    return Head == NULL ? NULL : DL_GetOwner(SnakeNode, Node, Head);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
SnakeNode * Snake_GetTail(Snake *Instance)
{
    if (&Instance->List == NULL) return NULL;
    DL_Node *Tail = DL_List_GetTail(&Instance->List);
    return Tail == NULL ? NULL : DL_GetOwner(SnakeNode, Node, Tail);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
SnakeNode * SnakeNode_GetPrev(SnakeNode *Instance)
{
    if (&Instance->Node == NULL) return NULL;
    DL_Node *Prev = DL_Node_GetPrev(&Instance->Node);
    return Prev == NULL ? NULL : DL_GetOwner(SnakeNode, Node, Prev);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
SnakeNode * SnakeNode_GetNext(SnakeNode *Instance)
{
    if (&Instance->Node == NULL) return NULL;
    DL_Node *Next = DL_Node_GetNext(&Instance->Node);
    return Next == NULL ? NULL : DL_GetOwner(SnakeNode, Node, Next);
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline SnakeBlock * Snake_MatrixOffset(Snake *Instance, uint32_t X, uint32_t Y)
{
    return &Instance->Matrix[Y * Instance->Width + X];
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline bool Snake_InverseWayCheck(Snake *Instance, uint32_t X, uint32_t Y)
{
    /* 注意要跳过第二个蛇身节点,因为反方向操作时过滤为无效 */
    SnakeNode *SecondNode = SnakeNode_GetNext(Snake_GetHead(Instance));
    if (SecondNode == NULL || (SecondNode->PosX == X && SecondNode->PosY == Y))
        return true;
    return false;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
static inline uint8_t Snake_PositionIsVaild(Snake *Instance, uint32_t X, uint32_t Y)
{
    /* 合法位置不能越界 */
    if (X >= Instance->Width || Y >= Instance->Height)
        return false;
    /* 合法位置不能是墙 */
    if (Snake_MatrixOffset(Instance, X, Y)->Status)
        return false;
    /* 合法位置不能是蛇身本身 */
    Snake_Traverse_Backward(Instance, Node)
        if (Node->PosX == X && Node->PosY == Y)
            return false;
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Snake_Ready(Snake *Instance)
{
    DL_List_Reset(&Instance->List);
    
    SnakeNode *Head = NULL;
    SnakeNode *Tail = NULL;
    Instance->Create(sizeof(SnakeNode), (void **)&Head);
    Instance->Create(sizeof(SnakeNode), (void **)&Tail);
    DL_Node_Reset(&Head->Node);
    DL_Node_Reset(&Tail->Node);
    
    /* 我这里偷个懒,正式逻辑不能这么搞,刷一个蛇头,蛇尾就是周围四个值 */
    while (true) {
        do {Instance->NextTouch(&Head->PosX, &Head->PosY);
        } while (!Snake_PositionIsVaild(Instance, Head->PosX, Head->PosY));
        do {Instance->NextTouch(&Tail->PosX, &Tail->PosY);
        } while (!Snake_PositionIsVaild(Instance, Tail->PosX, Tail->PosY));
    
        if ((Head->PosX - Tail->PosX == 1 ||
             Tail->PosX - Head->PosX == 1) && Head->PosY == Tail->PosY)
            break;
        if ((Head->PosY - Tail->PosY == 1 ||
             Tail->PosY - Head->PosY == 1) && Head->PosX == Tail->PosX)
            break;
    }
    
    DL_List_InsertPrepend(&Instance->List, NULL, &Head->Node);
    DL_List_InsertAppend(&Instance->List,  NULL, &Tail->Node);
    
    SnakeNode Node = {0};
    Instance->SetRecord1(&Node, Head);
    Instance->SetRecord2(Tail, &Node);
    
    do {Instance->NextTouch(&Instance->TouchPosX, &Instance->TouchPosY);
    } while (!Snake_PositionIsVaild(Instance, Instance->TouchPosX, Instance->TouchPosY));
    
    Instance->CurrentWay = Snake_None;
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
bool Snake_Execute(Snake *Instance, SnakeWay Way)
{
    /* 获得头尾节点 */
    SnakeNode *Node = NULL;
    SnakeNode *Head = Snake_GetHead(Instance);
    SnakeNode *Tail = Snake_GetTail(Instance);
    /* 获取下一个节点位置坐标 */
    uint32_t PosX = Instance->Width;
    uint32_t PosY = Instance->Height;
    switch (Way) {
    case Snake_Up:      PosX = Head->PosX; PosY = Head->PosY - 1;   break;
    case Snake_Down:    PosX = Head->PosX; PosY = Head->PosY + 1;   break;
    case Snake_Left:    PosX = Head->PosX - 1; PosY = Head->PosY;   break;
    case Snake_Right:   PosX = Head->PosX + 1; PosY = Head->PosY;   break;
    default:            return true;
    }
    /* 注意要跳过第二个蛇身节点,因为反方向操作时过滤为无效 */
    if (Snake_InverseWayCheck(Instance, PosX, PosY))
        if (Instance->CurrentWay == Snake_None)
            return true;
        else
            Way = Instance->CurrentWay;
    /* 检查下一个节点是否非法 */
    if (!Snake_PositionIsVaild(Instance, PosX, PosY))
        return false;
    /* 重新更新方向状态 */
    Instance->CurrentWay = Way;
    switch (Way) {
    case Snake_Up:      PosX = Head->PosX; PosY = Head->PosY - 1;   break;
    case Snake_Down:    PosX = Head->PosX; PosY = Head->PosY + 1;   break;
    case Snake_Left:    PosX = Head->PosX - 1; PosY = Head->PosY;   break;
    case Snake_Right:   PosX = Head->PosX + 1; PosY = Head->PosY;   break;
    default:            return true;
    }
    /* 生成一个新节点并加入到节点头 */
    Instance->Create(sizeof(SnakeNode), (void **)&Node);
    Instance->SetRecord1(Head, Node);
    DL_Node_Reset(&Node->Node);
    Node->PosX = PosX;
    Node->PosY = PosY;
    DL_List_InsertPrepend(&Instance->List, NULL, &Node->Node);
    /* 如果下一个节点是目标点 */
    if (PosX == Instance->TouchPosX && PosY == Instance->TouchPosY) {
        do {
        /* 更新一个新的节点并返回 */
        Instance->NextTouch(&Instance->TouchPosX, &Instance->TouchPosY);
        /* 注意需要是一个有效的节点(不是墙就行) */
        } while (!Snake_PositionIsVaild(Instance, Instance->TouchPosX, Instance->TouchPosY));
        /* 这里忽略那种蛇填满整个图的场景,我觉得没必要,没有这个必要 */
        
        return true;
    }
    /* 移除出节点尾并销毁一个旧节点 */
    Node = Tail;
    Tail = SnakeNode_GetPrev(Node);
    DL_List_Remove(&Instance->List, &Node->Node);
    Instance->SetRecord2(Tail, Node);
    Instance->Destroy(Node);
    return true;
}
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/
