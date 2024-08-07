/*实现目标:
 *    杂项的零碎小功能集合
 *    摘要于外界的chunksum32校验:   Airoha Technology Corp
 *    摘要于外界的crc32校验:        Airoha Technology Corp
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 泛化封装:内存字节逆向
 */
void app_sys_mem_rev_b1(void *addr)
{
}
void app_sys_mem_rev_b2(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 1; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}
void app_sys_mem_rev_b4(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 3; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}
void app_sys_mem_rev_b8(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 7; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}

/*@brief 字节对齐
 */
void * app_sys_align_low(void *addr, uintptr_t align)
{
    return (void *)(((uintptr_t)addr + (0)) & ~(align - 1));
}
void * app_sys_align_high(void *addr, uintptr_t align)
{
    return (void *)(((uintptr_t)addr + (align - 1)) & ~(align - 1));
}
bool app_sys_align_check(void *addr, uintptr_t align)
{
    return (bool  )(((uintptr_t)addr % (align)) == 0);
}

/*@brief 移动指定字符到尾部
 *@param str 字符串
 *@param c   指定字符
 *@param rev rev时移动到头部
 */
void app_sys_str_move(char *str, char c, bool rev)
{
    uint32_t len = strlen(str);
    /* 逆序,移动字符到头部使用 */
    if (rev) {
        char    *arr = str;
        uint32_t pos = len;
        do {/* 通用组件:逆序数组 */
            for (uint32_t idx0 = 0; idx0 < pos / 2; idx0++) {
                 uint32_t idx1 = pos - 1 - idx0;
                 char tmp1 = arr[idx0];
                 char tmp2 = arr[idx1];
                 arr[idx0] = tmp2;
                 arr[idx1] = tmp1;
            }
        } while (0);
    }
    /* 双指针移动字符 */
    if (true) {
        uint32_t pos1 = 0, pos2 = 0;
        for (; pos2 < len; pos2++)
            if (str[pos2]  != c)
                str[pos1++] = str[pos2];
        for (; pos1 < len; pos1++)
                str[pos1] = c;
    }
    /* 逆序,移动字符到头部使用 */
    if (rev) {
        char    *arr = str;
        uint32_t pos = len;
        do {/* 通用组件:逆序数组 */
            for (uint32_t idx0 = 0; idx0 < pos / 2; idx0++) {
                 uint32_t idx1 = pos - 1 - idx0;
                 char tmp1 = arr[idx0];
                 char tmp2 = arr[idx1];
                 arr[idx0] = tmp2;
                 arr[idx1] = tmp1;
            }
        } while(0);
    }
}

/*@brief 循环左旋转字符串
 *@param str 字符串
 *@param ofs 旋转点
 *@param rev rev时循环右旋转
 */
void app_sys_str_rotate(char *str, uint32_t ofs, bool rev)
{
    uint32_t len = strlen(str);
    
    if (rev)
        ofs  = len - ofs;
        ofs %= len;
    if (ofs == 0 || len == 0)
        return;
    
    uint32_t pos[3][2] = {{0, ofs - 1}, {ofs, len - 1}, {0, len - 1},};
    
    for (uint8_t I = 0; I < 3; I++) {
        for (uint32_t pos1 = pos[I][0], pos2 = pos[I][1]; pos1 < pos2; pos1++, pos2--) {
            char tmp1 = str[pos1];
            char tmp2 = str[pos2];
            str[pos1] = tmp2;
            str[pos2] = tmp1;
        }
    }
}

/*@brief 指定字符为分割点逆序字符串
 *@param str 字符串
 *@param c   指定字符
 */
void app_sys_str_reverse(char *str, char c)
{
    uint32_t pos = 0, pos1 = 0, pos2 = 0, len = strlen(str);
    
    if (len == 0)
        return;
    
    while(pos < len) {
        for (pos1 = pos;  str[pos1] == c && str[pos1] != 0; pos1++);
        for (pos2 = pos1; str[pos2] != c && str[pos2] != 0; pos2++);
        for (pos  = pos2--; pos1 < pos2; pos1++, pos2--) {
            char tmp1 = str[pos1];
            char tmp2 = str[pos2];
            str[pos1] = tmp2;
            str[pos2] = tmp1;
        }
    }
    for (pos1 = 0, pos2 = len - 1; pos1 < pos2; pos1++, pos2--) {
            char tmp1 = str[pos1];
            char tmp2 = str[pos2];
            str[pos1] = tmp2;
            str[pos2] = tmp1;
    }
}

/*@brief 将一个索引下标数组随机打乱
 *@param arr 索引下标数组
 *@param len 索引下标数组长度
 */
void app_sys_idx_shuffle(uint32_t *arr, uint32_t len)
{
    /* 1.先将其序列化 */
    for (uint32_t idx = 0; idx < len; idx++)
        arr[idx] = idx;
    /* 2.逆向乱序 */
    for (uint32_t idx = 0; idx < len; idx++) {
        /* 取一个随机下标 */
        uint32_t idx1 = rand() % (len - idx);
        uint32_t idx2 = len - 1 - idx;
        /* 交换idx1 与 len - 1 - idx */
        uint32_t tmp1 = arr[idx1];
        uint32_t tmp2 = arr[idx2];
        arr[idx1] = tmp2;
        arr[idx2] = tmp1;
    }
}

/*@brief 计算数据流的crc32校验
 *@param data 数据流
 *@param size 数据大小
 */
uint32_t app_sys_crc32(uint8_t *data, uint32_t size)
{
    uint32_t crc32 = 0xFFFFFFFFu;
    
    /* 校验表 */
    static const uint32_t app_sys_crc32_table[256] = 
    {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
        0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
        0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
        0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
        0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
        0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
        0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
        0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
        0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
        0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
        0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
        0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
        0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
        0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
        0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
        0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
        0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
        0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
        0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
        0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
        0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
    };
    
    for (uint32_t idx = 0; idx < size; idx++)
    {
        uint32_t index = (crc32 ^ data[idx]) & 0xff;
        crc32 = (crc32 >> 8) ^ app_sys_crc32_table[index];
    }

    crc32 ^= 0xFFFFFFFFu;
    return crc32;
}

/*@brief 计算数据流的crc8校验
 *@param data 数据流
 *@param size 数据大小
 */
uint8_t app_sys_crc8(uint8_t *data, uint32_t size)
{
    uint8_t crc8 = 0x00;
    
    /* 校验表 */
    static const uint8_t app_sys_crc8_table[256] = 
    {
        0x00,0x31,0x62,0x53,0xc4,0xf5,0xa6,0x97,
        0xb9,0x88,0xdb,0xea,0x7d,0x4c,0x1f,0x2e,
        0x43,0x72,0x21,0x10,0x87,0xb6,0xe5,0xd4,
        0xfa,0xcb,0x98,0xa9,0x3e,0x0f,0x5c,0x6d,
        0x86,0xb7,0xe4,0xd5,0x42,0x73,0x20,0x11,
        0x3f,0x0e,0x5d,0x6c,0xfb,0xca,0x99,0xa8,
        0xc5,0xf4,0xa7,0x96,0x01,0x30,0x63,0x52,
        0x7c,0x4d,0x1e,0x2f,0xb8,0x89,0xda,0xeb,
        0x3d,0x0c,0x5f,0x6e,0xf9,0xc8,0x9b,0xaa,
        0x84,0xb5,0xe6,0xd7,0x40,0x71,0x22,0x13,
        0x7e,0x4f,0x1c,0x2d,0xba,0x8b,0xd8,0xe9,
        0xc7,0xf6,0xa5,0x94,0x03,0x32,0x61,0x50,
        0xbb,0x8a,0xd9,0xe8,0x7f,0x4e,0x1d,0x2c,
        0x02,0x33,0x60,0x51,0xc6,0xf7,0xa4,0x95,
        0xf8,0xc9,0x9a,0xab,0x3c,0x0d,0x5e,0x6f,
        0x41,0x70,0x23,0x12,0x85,0xb4,0xe7,0xd6,
        0x7a,0x4b,0x18,0x29,0xbe,0x8f,0xdc,0xed,
        0xc3,0xf2,0xa1,0x90,0x07,0x36,0x65,0x54,
        0x39,0x08,0x5b,0x6a,0xfd,0xcc,0x9f,0xae,
        0x80,0xb1,0xe2,0xd3,0x44,0x75,0x26,0x17,
        0xfc,0xcd,0x9e,0xaf,0x38,0x09,0x5a,0x6b,
        0x45,0x74,0x27,0x16,0x81,0xb0,0xe3,0xd2,
        0xbf,0x8e,0xdd,0xec,0x7b,0x4a,0x19,0x28,
        0x06,0x37,0x64,0x55,0xc2,0xf3,0xa0,0x91,
        0x47,0x76,0x25,0x14,0x83,0xb2,0xe1,0xd0,
        0xfe,0xcf,0x9c,0xad,0x3a,0x0b,0x58,0x69,
        0x04,0x35,0x66,0x57,0xc0,0xf1,0xa2,0x93,
        0xbd,0x8c,0xdf,0xee,0x79,0x48,0x1b,0x2a,
        0xc1,0xf0,0xa3,0x92,0x05,0x34,0x67,0x56,
        0x78,0x49,0x1a,0x2b,0xbc,0x8d,0xde,0xef,
        0x82,0xb3,0xe0,0xd1,0x46,0x77,0x24,0x15,
        0x3b,0x0a,0x59,0x68,0xff,0xce,0x9d,0xac
    };
    
    for (uint32_t idx = 0; idx < size; idx++)
    {
        crc8 = app_sys_crc8_table[crc8 ^ data[idx]];
    }
    
    return crc8;
}

/*@brief 计算数据流的checksum32校验
 *@param data 数据流
 *@param size 数据大小
 */
uint32_t app_sys_checksum32(void *data, uint32_t size)
{
    uint32_t *data4 = (uint32_t *)data, checksum = 0;
    for(uint32_t idx = 0; idx < size / 4; idx++)
        checksum += data4[idx];
    return checksum;
}

/*@brief 计算定积分
 *@param fun 函数
 *@param l 左区间
 *@param r 右区间
 *@param p 精度
 *@param f 模式(1:梯形法;2:抛物线法)
 */
double app_sys_cal_di(double (*fun)(double x), double l, double r, uint32_t p, uint32_t f)
{
    double cur = 0.0, ret = 0.0, ret1 = 0.0, ret2 = 0.0;
    double del_1 = (r - l) / (double)p;
    double del_2 = (r - l) / (double)p * 2.0;
    
    if (fun == NULL || l > r || p == 0)
        return 0.0;
    
    //梯形法公式:
    //(b-a)/n(y(1)+...+y(n-1)+y(0)/2+y(n)/2)
    //此时:--> p = n
    if (f == 1) {
        
        cur = l;
        for (uint32_t idx = 1; idx < p; idx++) {
            cur += del_1;
            ret += fun(cur);
        }
        
        ret += (fun(l) + fun(r)) / 2.0;
        ret *= del_1;
    }
    
    //抛物线法公式(辛普森公式):
    //(b-a)/6n(y(0)+y(2n)+4(y1+...+y(2n-1))+2(y2+...+y(2n-2)))
    //此时:--> p = 2n
    if (f == 2) {
        if (p % 2 != 0)
            p += 1;
        
        //求奇数部分
        cur = l + del_1;
        for (uint32_t idx = 1; idx < p; idx += 2) {
            ret1 += fun(cur);
            cur  += del_2;
        }
        //求偶数部分
        cur = l + del_2;
        for (uint32_t idx = 2; idx < p; idx += 2) {
            ret2 += fun(cur);
            cur  += del_2;
        }
        
        ret += (fun(l) + fun(r));
        ret += ret1 * 4.0 + ret2 * 2.0;
        ret *= del_1 / 3.0;
    }
    
    return ret;
}

/*@brief 牛顿切线法求取方程近似解
 *@param fun0 0阶函数(原函数)
 *@param fun1 1阶函数(原函数的导数)
 *@param s 起始点
 *@param p 精度
 */
double app_sys_cal_nt(double (*fun0)(double x), double (*fun1)(double x), double s, double p)
{
    p = app_sys_abs(p);
    if (fun0 == NULL || fun1 == NULL || p <= 1e-5)
        return 0;
    
    double x = s, x_0, x_1, t;
    
    while (1) {
        // 计算多项式的值与一阶导数的值
        x_0 = fun0(x);
        x_1 = fun1(x);
        //
        if (app_sys_abs(x_1) <= 1e-5)
            break;
        // 计算切点值
        t = x_0 / x_1;
        //
        if (app_sys_abs(t) <= p)
            break;
        //迭代
        x = x - t;
    }
    return x;
}
