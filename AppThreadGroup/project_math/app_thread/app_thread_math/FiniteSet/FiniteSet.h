#ifndef FINITE_SET_H
#define FINITE_SET_H

#include <stdint.h>
#include <stdbool.h>

/* 有限全集集合 */
#define FINITE_FULLSET_BYTE     4
#define FINITE_FULLSET_BITS     32
#define FINITE_FULLSET_TYPE     uint32_t

typedef struct {
    FINITE_FULLSET_TYPE *Set;   /* 集合实体 */
    uint32_t Length;            /* 集合实体长度 */
    uint32_t Number;            /* 集合当前数量 */
} FiniteFullSet;

/* 有限子集集合 */
#define FINITE_SUBSET_BYTE     1
#define FINITE_SUBSET_BITS     8
#define FINITE_SUBSET_TYPE     uint8_t
typedef struct {
    FINITE_SUBSET_TYPE *BitMap; /* 集合位图 */
    uint32_t Length;            /* 集合位图长度 */
    FiniteFullSet *Full;        /* 集合绑定的全集 */
} FiniteSubSet;

/*有限全集集合(FiniteFullSet: FullSet)
 *不提供完整实现,仅仅提供构造和更新规则
 *FullSet->Set      = Set;      //静态或动态分配的实体(最大长度是Length)
 *FullSet->Length   = Length;
 *FullSet->Number   = Number;   //活跃元素数量:在Set中布局为Set[0 ~ Number - 1]
 */

/*有限子集集合(FiniteSubSet: SubSet)
 *不提供完整实现,仅仅提供构造和更新规则
 *SubSet->BitMap    = BitMap;   //静态或动态分配的实体(最大长度是Length)
 *SubSet->Length    = Length;   //子集Length为绑定全集的(Length / FINITE_SUBSET_BITS + 1)
 *SubSet->Full      = FullSet;  //所绑定的全集
 */

/* 这份案例只会提供最核心的部分内容 */

static bool FiniteSubSet_ElementSet(FiniteSubSet *Sub, FINITE_FULLSET_TYPE Element)
{
    for (uint32_t Index = 0; Index < Sub->Full->Number; Index++)
        if (Sub->Full->Set[Index] == Element) {
            uint32_t Bits_N = Index / FINITE_SUBSET_BITS;
            uint32_t Bits_2 = Index % FINITE_SUBSET_BITS;
            Sub->BitMap[Bits_N] |= (1 << Bits_2);
            return true;
        }
    return false;
}

static bool FiniteSubSet_ElementClear(FiniteSubSet *Sub, FINITE_FULLSET_TYPE Element)
{
    for (uint32_t Index = 0; Index < Sub->Full->Number; Index++)
        if (Sub->Full->Set[Index] == Element) {
            uint32_t Bits_N = Index / FINITE_SUBSET_BITS;
            uint32_t Bits_2 = Index % FINITE_SUBSET_BITS;
            Sub->BitMap[Bits_N] &= ~(1 << Bits_2);
            return true;
        }
    return false;
}

static bool FiniteSubSet_ElementCheck(FiniteSubSet *Sub, FINITE_FULLSET_TYPE Element)
{
    for (uint32_t Index = 0; Index < Sub->Full->Number; Index++)
        if (Sub->Full->Set[Index] == Element) {
            uint32_t Bits_N = Index / FINITE_SUBSET_BITS;
            uint32_t Bits_2 = Index % FINITE_SUBSET_BITS;
            if (Sub->BitMap[Bits_N] & (1 << Bits_2))
                return true;
            return false;
        }
    return false;
}

static uint32_t FiniteSubSet_ElementGets(FiniteSubSet *Sub, FINITE_FULLSET_TYPE *Element)
{
    uint32_t Number = 0;
    for (uint32_t Index = 0; Index < Sub->Full->Number &&
                             Index < Sub->Length * FINITE_SUBSET_BITS; Index++) {
        uint32_t Bits_N = Index / FINITE_SUBSET_BITS;
        uint32_t Bits_2 = Index % FINITE_SUBSET_BITS;
    
        if (Sub->BitMap[Bits_N] & (1 << Bits_2))
            Element[Number++] = Sub->Full->Set[Index];
    }
    return Number;
}

/* 下面是子集集合运算:要求作用于同一全集,子集空间占用完全一致 */

/* 求补集:Sub1 = ~Sub2 */
static inline void FiniteSubSet_Not(FiniteSubSet *Sub1, FiniteSubSet *Sub2)
{
    for (uint32_t Index = 0; Index < Sub1->Length; Index++)
        Sub1->BitMap[Index] = ~Sub2->BitMap[Index];
}

/* 求交集:Sub1 = Sub2 & Sub3 */
static inline void FiniteSubSet_And(FiniteSubSet *Sub1, FiniteSubSet *Sub2, FiniteSubSet *Sub3)
{
    for (uint32_t Index = 0; Index < Sub1->Length; Index++)
        Sub1->BitMap[Index] = Sub2->BitMap[Index] & Sub3->BitMap[Index];
}

/* 求并集:Sub1 = Sub2 | Sub3 */
static inline void FiniteSubSet_Or(FiniteSubSet *Sub1, FiniteSubSet *Sub2, FiniteSubSet *Sub3)
{
    for (uint32_t Index = 0; Index < Sub1->Length; Index++)
        Sub1->BitMap[Index] = Sub2->BitMap[Index] | Sub3->BitMap[Index];
}

/* 求差集:Sub1 = Sub2 - Sub3 */
static inline void FiniteSubSet_Sub(FiniteSubSet *Sub1, FiniteSubSet *Sub2, FiniteSubSet *Sub3)
{
    for (uint32_t Index = 0; Index < Sub1->Length; Index++)
        Sub1->BitMap[Index] = Sub2->BitMap[Index] & ~Sub3->BitMap[Index];
}

#endif
