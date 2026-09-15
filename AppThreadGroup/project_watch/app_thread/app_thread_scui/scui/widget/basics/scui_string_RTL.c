/*实现目标:
 *    字符串变形(RTL语言连字等)
 *    字符串双向调序(BIDI, 视觉顺序)
 *    取用了lv_txt_ap.c/.h整理得来
 *    取用了lv_bidi.c/.h整理得来
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_CHAR_AP_UN         (0xFFFFFFFF)    /* undefind */
#define SCUI_CHAR_AP_AB         (0x0622)        /* alphabet */

typedef struct {
    uint8_t  char_ofs;
    uint16_t char_end;          /* form:end */
    int8_t   char_ofs_bgn;      /* form:begin */
    int8_t   char_ofs_mid;      /* form:middle */
    int8_t   char_ofs_iso;      /* form:isolated */
    struct {
        uint8_t prev;           /* conjunction to prev */
        uint8_t next;           /* conjunction to next */
    } conj;                     /* conjunction */
} scui_char_ap_t;

const scui_char_ap_t scui_char_ap[] = {
    
    {  0, 0xFE81,  0, 0,  0,  {0, 0}},   // أ
    {  1, 0xFE84, -1, 0, -1,  {1, 0}},   // أ
    {  2, 0xFE86, -1, 0, -1,  {1, 0}},   // ؤ
    {  3, 0xFE88, -1, 0, -1,  {1, 0}},   // ﺇ
    {  4, 0xFE8A,  1, 2, -1,  {1, 1}},   // ئ
    {  5, 0xFE8E, -1, 0, -1,  {1, 0}},   // آ
    {  6, 0xFE90,  1, 2, -1,  {1, 1}},   // ب
    { 92, 0xFB57,  1, 2, -1,  {1, 1}},   // پ
    {  8, 0xFE96,  1, 2, -1,  {1, 1}},   // ت
    {  9, 0xFE9A,  1, 2, -1,  {1, 1}},   // ث
    { 10, 0xFE9E,  1, 2, -1,  {1, 1}},   // ج
    {100, 0xFB7B,  1, 2, -1,  {1, 1}},   // چ
    { 11, 0xFEA2,  1, 2, -1,  {1, 1}},   // ح
    { 12, 0xFEA6,  1, 2, -1,  {1, 1}},   // خ
    { 13, 0xFEAA, -1, 0, -1,  {1, 0}},   // د
    { 14, 0xFEAC, -1, 0, -1,  {1, 0}},   // ذ
    { 15, 0xFEAE, -1, 0, -1,  {1, 0}},   // ر
    { 16, 0xFEB0, -1, 0, -1,  {1, 0}},   // ز
    {118, 0xFB8B, -1, 0, -1,  {1, 0}},   // ژ
    { 17, 0xFEB2,  1, 2, -1,  {1, 1}},   // س
    { 18, 0xFEB6,  1, 2, -1,  {1, 1}},   // ش
    { 19, 0xFEBA,  1, 2, -1,  {1, 1}},   // ص
    { 20, 0xFEBE,  1, 2, -1,  {1, 1}},   // ض
    { 21, 0xFEC2,  1, 2, -1,  {1, 1}},   // ط
    { 22, 0xFEC6,  1, 2, -1,  {1, 1}},   // ظ
    { 23, 0xFECA,  1, 2, -1,  {1, 1}},   // ع
    { 24, 0xFECE,  1, 2, -1,  {1, 1}},   // غ
    { 30, 0x0640,  0, 0,  0,  {1, 1}},   // - (mad, hyphen)
    { 31, 0xFED2,  1, 2, -1,  {1, 1}},   // ف
    { 32, 0xFED6,  1, 2, -1,  {1, 1}},   // ق
    {135, 0xFB8F,  1, 2, -1,  {1, 1}},   // ک
    { 33, 0xFEDA,  1, 2, -1,  {1, 1}},   // ﻙ
    {141, 0xFB93,  1, 2, -1,  {1, 1}},   // گ
    { 34, 0xFEDE,  1, 2, -1,  {1, 1}},   // ل
    { 35, 0xFEE2,  1, 2, -1,  {1, 1}},   // م
    { 36, 0xFEE6,  1, 2, -1,  {1, 1}},   // ن
    { 38, 0xFEEE, -1, 0, -1,  {1, 0}},   // و
    { 37, 0xFEEA,  1, 2, -1,  {1, 1}},   // ه
    { 39, 0xFEF0,  0, 0, -1,  {1, 0}},   // ى
    { 40, 0xFEF2,  1, 2, -1,  {1, 1}},   // ي
    {170, 0xFBFD,  1, 2, -1,  {1, 1}},   // ی
    {  7, 0xFE94, -1, 2, -1,  {1, 0}},   // ة
    {206, 0x06F0, -1, 2,  0,  {0, 0}},   // ۰
    {207, 0x06F1,  0, 0,  0,  {0, 0}},   // ۱
    {208, 0x06F2,  0, 0,  0,  {0, 0}},   // ۲
    {209, 0x06F3,  0, 0,  0,  {0, 0}},   // ۳
    {210, 0x06F4,  0, 0,  0,  {0, 0}},   // ۴
    {211, 0x06F5,  0, 0,  0,  {0, 0}},   // ۵
    {212, 0x06F6,  0, 0,  0,  {0, 0}},   // ۶
    {213, 0x06F7,  0, 0,  0,  {0, 0}},   // ۷
    {214, 0x06F8,  0, 0,  0,  {0, 0}},   // ۸
    {215, 0x06F9,  0, 0,  0,  {0, 0}},   // ۹
    
    // end item
    { 0, 0, 0, 0, 0, {0, 0}}
};

static bool scui_char_ap_vowel(uint16_t c)
{
    return (c >= 0x064B) && (c <= 0x0652);
}

static uint32_t scui_char_ap_lam_alef(uint32_t ch_curr, uint32_t ch_next)
{
    if (scui_char_ap[ch_curr].char_ofs != 34) return 0;
    if (ch_next == SCUI_CHAR_AP_UN) return 0;
    
    uint32_t ch_code = scui_char_ap[ch_next].char_ofs + SCUI_CHAR_AP_AB;
    
    if(ch_code == 0x0622) return 0xFEF5;    // (lam-alef) mad
    if(ch_code == 0x0623) return 0xFEF7;    // (lam-alef) top hamza
    if(ch_code == 0x0625) return 0xFEF9;    // (lam-alef) bot hamza
    if(ch_code == 0x0627) return 0xFEFB;    // (lam-alef) alef
    
    return 0;
}

static uint32_t scui_char_ap_idx(uint16_t c)
{
    for(uint8_t idx = 0; scui_char_ap[idx].char_end; idx++) {
        if (c == (scui_char_ap[idx].char_ofs + SCUI_CHAR_AP_AB))
            return idx;
        
        //is it an end form
        //is it a  begin form
        //is it a  middle form
        //is it an isolated form
        if (c == scui_char_ap[idx].char_end) return idx;
        if (c == scui_char_ap[idx].char_end + scui_char_ap[idx].char_ofs_bgn) return idx;
        if (c == scui_char_ap[idx].char_end + scui_char_ap[idx].char_ofs_mid) return idx;
        if (c == scui_char_ap[idx].char_end + scui_char_ap[idx].char_ofs_iso) return idx;
        
    }
    
    return SCUI_CHAR_AP_UN;
}

uint32_t scui_utf8_str_bytes_RTL(uint8_t *str_utf8)
{
    uint32_t str_len = scui_utf8_str_bytes(str_utf8);
    uint32_t str_cnt = 0;
    
    for (uint32_t idx = 0; idx < str_len; true) {
        uint32_t ch_enc = 0;
        uint32_t ch_num = scui_utf8_to_uni(&str_utf8[idx], &ch_enc);
        uint32_t ap_idx = scui_char_ap_idx(ch_enc);
        
        if (ap_idx != SCUI_CHAR_AP_UN)
            ch_enc  = scui_char_ap[ap_idx].char_end;
        
        str_cnt += scui_utf8_bytes_uni(ch_enc);
        idx += ch_num;
    }
    
    return str_cnt + 1;
}

/*@brief 字符串处理:RTL连字变形(逻辑字符->呈现形式)
 *@param args 字符串绘制参数
 */
void scui_string_RTL(scui_string_args_t *args)
{
    uint32_t str_len = args->number;
    uint32_t *ch_enc = args->unicode;
    uint32_t *ch_fin = SCUI_MEM_ALLOC(scui_mem_type_mix, 4 * (str_len + 1));
    
    uint32_t idx_ofs  = 0;
    uint32_t idx_prev = SCUI_CHAR_AP_UN;
    for (uint32_t idx = 0; idx < str_len; true) {
        uint32_t idx_curr = scui_char_ap_idx(ch_enc[idx]);
        uint32_t idx_next = scui_char_ap_idx(ch_enc[idx + 1]);
        
        // Current character is a vowel
        // Skip this character
        if (scui_char_ap_vowel(ch_enc[idx])) {
            ch_fin[idx_ofs] = ch_enc[idx];
            idx++; idx_ofs++;
            continue;
        }
        
        // Next character is a vowel
        // Skip the vowel character to join with the character after it
        if (scui_char_ap_vowel(ch_enc[idx + 1]))
            idx_next = scui_char_ap_idx(ch_enc[idx + 2]);
        
        if (idx_curr == SCUI_CHAR_AP_UN) {
            idx_prev  = SCUI_CHAR_AP_UN;
            ch_fin[idx_ofs] = ch_enc[idx];
            idx++; idx_ofs++;
            continue;
        }
        
        uint8_t conj_to_prev = 0;
        uint8_t conj_to_next = 0;
        
        if (!(idx_prev == SCUI_CHAR_AP_UN || idx == 0))
            conj_to_prev = scui_char_ap[idx_prev].conj.next;
        if (!(idx_next == SCUI_CHAR_AP_UN || idx == str_len - 1))
            conj_to_next = scui_char_ap[idx_next].conj.prev;
        
        uint32_t lam_alef = scui_char_ap_lam_alef(idx_curr, idx_next);
        if (lam_alef != 0) {
            idx_prev  = SCUI_CHAR_AP_UN;
            if (conj_to_prev) lam_alef++;
            ch_fin[idx_ofs] = lam_alef;
            idx += 2; idx_ofs++;
            continue;
        }
        
        uint8_t type = 0;
        if ( conj_to_prev &&  conj_to_next) type = 1;
        if (!conj_to_prev &&  conj_to_next) type = 2;
        if ( conj_to_prev && !conj_to_next) type = 3;
        
        ch_fin[idx_ofs] = scui_char_ap[idx_curr].char_end;
        switch (type) {
        default: SCUI_ASSERT(false); break;
        case 0: ch_fin[idx_ofs] += scui_char_ap[idx_curr].char_ofs_iso; break;
        case 1: ch_fin[idx_ofs] += scui_char_ap[idx_curr].char_ofs_mid; break;
        case 2: ch_fin[idx_ofs] += scui_char_ap[idx_curr].char_ofs_bgn; break;
        case 3: ch_fin[idx_ofs] += 0; break;
        }
        
        idx_prev = idx_curr;
        idx++; idx_ofs++;
    }
    
    for(uint32_t idx = 0; idx < str_len; idx++) ch_enc[idx] = 0;
    for(uint32_t idx = 0; idx < idx_ofs; idx++) ch_enc[idx] = ch_fin[idx];
    ch_enc[idx_ofs]  = 0; args->number = idx_ofs;
    /* 变换后字符数会减少, 原地改写需同步长度 */
    
    SCUI_MEM_FREE(ch_fin);
}

/*****************************************************************************/

/*@brief 字符串处理:双向调序(BIDI, 逻辑顺序->视觉顺序)
 *@param args 字符串绘制参数
 */
void scui_string_BIDI(scui_string_args_t *args);

/*@brief 字符串处理:RTL连字符拆分检查(多行拆行用)
 *@param args  字符串绘制参数
 *@param idx_t 拆分点
 *@retval 是否可以拆分
 */
bool scui_string_RTL_break(scui_string_args_t *args, scui_coord_t idx_t);

/* BIDI 方向 */
typedef enum {
    scui_bidi_dir_ltr     = 0x00,
    scui_bidi_dir_rtl     = 0x01,
    scui_bidi_dir_neutral = 0x20,
    scui_bidi_dir_weak    = 0x21,
} scui_bidi_dir_t;

#define SCUI_BIDI_BRACKET_DEPTH 4

typedef struct {
    uint32_t        bracket_pos;
    scui_bidi_dir_t dir;
} scui_bidi_bracket_t;

static const uint8_t scui_bidi_bracket_left[]  = {"<({["};
static const uint8_t scui_bidi_bracket_right[] = {">)}]"};
static scui_bidi_bracket_t scui_bidi_bracket_stack[SCUI_BIDI_BRACKET_DEPTH];
static uint8_t scui_bidi_bracket_stack_p;

static bool scui_bidi_letter_is_weak(uint32_t letter)
{
    static const char weaks[] = "0123456789";
    
    for (uint32_t idx = 0; weaks[idx] != '\0'; idx++)
        if (letter == (uint32_t)weaks[idx])
            return true;
    
    /* 阿拉伯-印度数字(0x0660-0x0669)与波斯数字(0x06F0-0x06F9): RTL中保持LTR组 */
    if (letter >= 0x0660 && letter <= 0x0669) return true;
    if (letter >= 0x06F0 && letter <= 0x06F9) return true;
    /* 阿拉伯百分号(0x066A): 归weak, 与数字组一并保持 */
    if (letter == 0x066A) return true;
    
    return false;
}

static bool scui_bidi_letter_is_rtl(uint32_t letter)
{
    /* 主要希伯来与阿拉伯区间 */
    if (letter >= 0x0590 && letter <= 0x05FF) return true;
    if (letter >= 0x0600 && letter <= 0x06FF) return true;
    if (letter >= 0x0700 && letter <= 0x08FF) return true;
    /* 希伯来呈现形式 */
    if (letter >= 0xFB1D && letter <= 0xFB4F) return true;
    /* 阿拉伯呈现形式A */
    if (letter >= 0xFB50 && letter <= 0xFDFF) return true;
    /* 阿拉伯呈现形式B */
    if (letter >= 0xFE70 && letter <= 0xFEFF) return true;
    
    return false;
}

static bool scui_bidi_letter_is_neutral(uint32_t letter)
{
    static const char neutrals[] = " \t\n\r.,:;'\"`!?%/\\-=()[]{}<>@#&$|";
    
    for (uint32_t idx = 0; neutrals[idx] != '\0'; idx++)
        if (letter == (uint32_t)neutrals[idx])
            return true;
    
    return false;
}

static scui_bidi_dir_t scui_bidi_letter_dir(uint32_t letter)
{
    if (scui_bidi_letter_is_rtl(letter))     return scui_bidi_dir_rtl;
    if (scui_bidi_letter_is_neutral(letter)) return scui_bidi_dir_neutral;
    if (scui_bidi_letter_is_weak(letter))    return scui_bidi_dir_weak;
    
    return scui_bidi_dir_ltr;
}

/*@brief 括号配对字符交换(逆序时左右括号互换)
 *@param letter 字符
 *@retval 配对后的字符
 */
static uint32_t scui_bidi_char_change_to_pair(uint32_t letter)
{
    for (uint32_t idx = 0; scui_bidi_bracket_left[idx] != '\0'; idx++)
        if (letter == scui_bidi_bracket_left[idx])
            return scui_bidi_bracket_right[idx];
    
    for (uint32_t idx = 0; scui_bidi_bracket_right[idx] != '\0'; idx++)
        if (letter == scui_bidi_bracket_right[idx])
            return scui_bidi_bracket_left[idx];
    
    return letter;
}

/*@brief 括号方向处理
 *@param unicode  字符序列
 *@param next_pos 当前位置(下一个字符)
 *@param len      字符数量
 *@param letter   当前字符
 *@param base_dir 基准方向
 *@retval 括号方向
 */
static scui_bidi_dir_t scui_bidi_bracket_process(uint32_t *unicode, uint32_t next_pos,
    uint32_t len, uint32_t letter, scui_bidi_dir_t base_dir)
{
    scui_bidi_dir_t bracket_dir = scui_bidi_dir_neutral;
    uint32_t idx_bracket = 0;
    
    /* 是否为左括号 */
    for (; scui_bidi_bracket_left[idx_bracket] != '\0'; idx_bracket++) {
        if (scui_bidi_bracket_left[idx_bracket] != letter)
            continue;
        
        /* 找到匹配的右括号, 若其间有 base_dir 方向字符则括号归 base_dir */
        bool find_end = false;
        uint32_t idx = next_pos;
        while (idx < len) {
            uint32_t letter_next = unicode[idx];
            idx++;
            if (letter_next == scui_bidi_bracket_right[idx_bracket]) {
                /* 右括号找到 */
                find_end = true;
                break;
            }
            scui_bidi_dir_t letter_dir = scui_bidi_letter_dir(letter_next);
            if (letter_dir == base_dir)
                bracket_dir = base_dir;
        }
        
        /* 无匹配右括号 */
        if (!find_end)
            return scui_bidi_dir_neutral;
        
        /* 括号内已找到 base_dir 强字符 */
        if (bracket_dir != scui_bidi_dir_neutral && bracket_dir != scui_bidi_dir_weak)
            break;
        
        /* 括号内无 base_dir 强字符, 查前文强字符 */
        idx = next_pos;
        if (idx)
            idx--;
        while (idx > 0) {
            uint32_t letter_next = unicode[idx];
            idx--;
            scui_bidi_dir_t letter_dir = scui_bidi_letter_dir(letter_next);
            if (letter_dir == scui_bidi_dir_ltr || letter_dir == scui_bidi_dir_rtl) {
                bracket_dir = letter_dir;
                break;
            }
        }
        
        /* 前文有强字符可用 */
        if (bracket_dir != scui_bidi_dir_neutral)
            break;
        
        /* 前文无强字符, 用 base_dir */
        if (idx == 0)
            bracket_dir = base_dir;
        
        break;
    }
    
    /* 当前字符是左括号 */
    if (scui_bidi_bracket_left[idx_bracket] != '\0') {
        if (bracket_dir == scui_bidi_dir_neutral || scui_bidi_bracket_stack_p == SCUI_BIDI_BRACKET_DEPTH)
            return scui_bidi_dir_neutral;
        
        scui_bidi_bracket_stack[scui_bidi_bracket_stack_p].bracket_pos = idx_bracket;
        scui_bidi_bracket_stack[scui_bidi_bracket_stack_p].dir = bracket_dir;
        scui_bidi_bracket_stack_p++;
        return bracket_dir;
    }
    
    /* 当前字符是右括号 */
    if (scui_bidi_bracket_stack_p > 0) {
        if (letter == scui_bidi_bracket_right[scui_bidi_bracket_stack[scui_bidi_bracket_stack_p - 1].bracket_pos]) {
            bracket_dir = scui_bidi_bracket_stack[scui_bidi_bracket_stack_p - 1].dir;
            scui_bidi_bracket_stack_p--;
            return bracket_dir;
        }
    }
    
    return scui_bidi_dir_neutral;
}

/*@brief 下一个run(方向一致字符序列)
 *@param unicode  字符序列
 *@param base_dir 基准方向
 *@param max_len  最大字符数量
 *@param len      本run字符数量
 *@retval run方向
 */
static scui_bidi_dir_t scui_bidi_get_next_run(uint32_t *unicode, scui_bidi_dir_t base_dir,
    uint32_t max_len, uint32_t *len)
{
    uint32_t i = 0;
    /* 防御: 空段/段首为分隔符或0时, 直接返回单字符run, 杜绝 run_len=0 死循环 */
    if (max_len == 0) {
        *len = 0;
        return base_dir;
    }
    if (unicode[0] == '\0' || unicode[0] == '\n' || unicode[0] == '\r') {
        *len = 1;
        return base_dir;
    }
    uint32_t letter = unicode[0];
    scui_bidi_dir_t dir = scui_bidi_letter_dir(letter);
    if (dir == scui_bidi_dir_neutral)
        dir = scui_bidi_bracket_process(unicode, 0, max_len, letter, base_dir);
    
    /* 跳过开头的 neutral/weak */
    while (dir == scui_bidi_dir_neutral || dir == scui_bidi_dir_weak) {
        letter = unicode[i];
        i++;
        dir = scui_bidi_letter_dir(letter);
        if (dir == scui_bidi_dir_neutral)
            dir = scui_bidi_bracket_process(unicode, i, max_len, letter, base_dir);
        
        if (dir == scui_bidi_dir_ltr || dir == scui_bidi_dir_rtl)
            break;
        
        if (i >= max_len) {
            *len = i;
            /* 纯weak/neutral组: 保持LTR顺序, 不按base逆序 */
            return scui_bidi_dir_ltr;
        }
    }
    
    scui_bidi_dir_t run_dir = dir;
    uint32_t i_prev = i;
    
    /* 找到下一个方向不同的字符 */
    scui_bidi_dir_t next_dir = base_dir;
    while (i_prev < max_len && i < max_len &&
        unicode[i] != '\0' && unicode[i] != '\n' && unicode[i] != '\r') {
        letter = unicode[i];
        i++;
        next_dir = scui_bidi_letter_dir(letter);
        if (next_dir == scui_bidi_dir_neutral)
            next_dir = scui_bidi_bracket_process(unicode, i, max_len, letter, base_dir);
        
        /* weak(数字)在RTL上下文归LTR */
        if (next_dir == scui_bidi_dir_weak) {
            if (run_dir == scui_bidi_dir_rtl && base_dir == scui_bidi_dir_rtl)
                next_dir = scui_bidi_dir_ltr;
        }
        
        /* 发现新方向run: 末尾neutral并入本run, 防止单独成run后被按base错位 */
        if ((next_dir == scui_bidi_dir_rtl || next_dir == scui_bidi_dir_ltr) &&
            next_dir != run_dir) {
            *len = i_prev;
            
            return run_dir;
        }
        
        i_prev = i;
    }
    
    /* 处理到末尾: 末尾neutral并入本run(与循环内一致) */
    *len = i_prev;
    
    return run_dir;
}

/*@brief RTL逆序(weak数字组保持LTR)
 *@param unicode 字符序列
 *@param len     字符数量
 */
static void scui_bidi_rtl_reverse(uint32_t *unicode, uint32_t len)
{
    uint32_t *unicode_out = SCUI_MEM_ALLOC(scui_mem_type_mix, 4 * (len + 1));
    uint32_t i = len;
    uint32_t wr = 0;
    
    while (i) {
        i--;
        uint32_t letter = unicode[i];
        
        /* weak(数字)整组保持LTR */
        if (scui_bidi_letter_is_weak(letter)) {
            uint32_t last_weak = i;
            uint32_t first_weak = i;
            while (i) {
                i--;
                letter = unicode[i];
                /* 数字与货币相关字符视为weak组 */
                if (scui_bidi_letter_is_weak(letter) == false &&
                    letter != '.' && letter != ',' && letter != '$' && letter != '%') {
                    i++;
                    first_weak = i;
                    break;
                }
            }
            if (i == 0)
                first_weak = 0;
            
            for (uint32_t idx = first_weak; idx <= last_weak; idx++)
                unicode_out[wr++] = unicode[idx];
        }
        /* 其余字符逆序存储, 括号配对交换 */
        else {
            unicode_out[wr++] = scui_bidi_char_change_to_pair(letter);
        }
    }
    
    for (uint32_t idx = 0; idx < len; idx++)
        unicode[idx] = unicode_out[idx];
    /* 不写 unicode[len]=0: 该位置属于相邻run的有效区, 会被逆序写回覆盖成0
       段尾0由 process_paragraph 统一写 */
    
    SCUI_MEM_FREE(unicode_out);
}

/*@brief 段落双向调序
 *@param unicode  字符序列
 *@param len      字符数量
 *@param base_dir 基准方向
 */
static void scui_bidi_process_paragraph(uint32_t *unicode, uint32_t len, scui_bidi_dir_t base_dir)
{
    /* 拷贝段落到临时缓冲(源), 处理结果写回unicode */
    uint32_t *src = SCUI_MEM_ALLOC(scui_mem_type_mix, 4 * (len + 1));
    for (uint32_t idx = 0; idx < len; idx++)
        src[idx] = unicode[idx];
    src[len] = 0;
    
    uint32_t run_len = 0;
    scui_bidi_dir_t run_dir;
    uint32_t rd = 0;
    uint32_t wr = (base_dir == scui_bidi_dir_rtl) ? len : 0;
    
    scui_bidi_bracket_stack_p = 0;
    
    /* 处理开头 neutral */
    scui_bidi_dir_t dir = base_dir;
    while (rd < len) {
        uint32_t letter = src[rd];
        rd++;
        dir = scui_bidi_letter_dir(letter);
        if (dir == scui_bidi_dir_neutral)
            dir = scui_bidi_bracket_process(src, rd, len, letter, base_dir);
        if (dir != scui_bidi_dir_neutral && dir != scui_bidi_dir_weak)
            break;
    }
    
    if (rd) {
        rd--;
        if (base_dir == scui_bidi_dir_ltr) {
            /* 开头neutral保持原位 */
            for (uint32_t idx = 0; idx < rd; idx++)
                unicode[wr++] = src[idx];
        } else {
            /* 开头neutral逆序放置 */
            wr -= rd;
            for (uint32_t idx = 0; idx < rd; idx++)
                unicode[wr + idx] = src[idx];
            scui_bidi_rtl_reverse(&unicode[wr], rd);
        }
    }
    
    /* 处理各run */
    while (rd < len) {
        run_dir = scui_bidi_get_next_run(&src[rd], base_dir, len - rd, &run_len);
        
        if (base_dir == scui_bidi_dir_ltr) {
            if (run_dir == scui_bidi_dir_ltr) {
                for (uint32_t idx = 0; idx < run_len; idx++)
                    unicode[wr + idx] = src[rd + idx];
            } else {
                for (uint32_t idx = 0; idx < run_len; idx++)
                    unicode[wr + idx] = src[rd + idx];
                scui_bidi_rtl_reverse(&unicode[wr], run_len);
            }
            wr += run_len;
        } else {
            wr -= run_len;
            if (run_dir == scui_bidi_dir_ltr) {
                for (uint32_t idx = 0; idx < run_len; idx++)
                    unicode[wr + idx] = src[rd + idx];
            } else {
                for (uint32_t idx = 0; idx < run_len; idx++)
                    unicode[wr + idx] = src[rd + idx];
                scui_bidi_rtl_reverse(&unicode[wr], run_len);
            }
        }
        
        rd += run_len;
        /* 防御: run不前进说明输入异常, 跳出避免死循环 */
        if (run_len == 0)
            break;
    }
    
    /* 段尾0只写src缓冲(见前), 不写原数组: 段尾可能不是数组末尾(多行),
       写0会覆盖段落分隔的换行符, 破坏后续段落划分 */
    SCUI_MEM_FREE(src);
}

void scui_string_BIDI(scui_string_args_t *args)
{
    if (args->utf8 == NULL || args->number == 0 ||
        args->name == SCUI_HANDLE_INVALID)
        return;
    
    /* base_dir固定为当前语言RTL状态, 不随文本内容更迭 */
    if (!scui_lang_RTL())
        return;
    
    scui_bidi_dir_t base_dir = scui_bidi_dir_rtl;
    uint32_t len = args->number;
    
    /* 按换行符分段处理 */
    uint32_t par_s = 0;
    while (par_s < len) {
        /* 跳过换行符 */
        while (par_s < len && (args->unicode[par_s] == '\n' || args->unicode[par_s] == '\r'))
            par_s++;
        if (par_s >= len)
            break;
        
        uint32_t par_e = par_s;
        while (par_e < len && args->unicode[par_e] != '\n' && args->unicode[par_e] != '\r')
            par_e++;
        
        scui_bidi_process_paragraph(&args->unicode[par_s], par_e - par_s, base_dir);
        par_s = par_e;
    }
}

bool scui_string_RTL_break(scui_string_args_t *args, scui_coord_t idx_t)
{
    /* 非RTL语言保持原拆分规则 */
    if (!scui_lang_RTL())
        return true;
    
    /* 越界位置可以拆分 */
    if (idx_t < 0 || idx_t >= args->number)
        return true;
    
    /* RTL: 连字符(-)不可折开 */
    if (args->unicode[idx_t] == '-')
        return false;
    
    return true;
}
