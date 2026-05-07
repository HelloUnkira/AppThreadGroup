/*实现目标:
 *    字符串变形
 *    暂不确定是否有效用
 *    取用了lv_txt_ap.c/.h整理得来
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

uint32_t scui_utf8_str_bytes_ap(uint8_t *str_utf8)
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

void scui_string_args_trans_ap(scui_string_args_t *args)
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
    ch_enc[idx_ofs] = 0;
    
    SCUI_MEM_FREE(ch_fin);
}
