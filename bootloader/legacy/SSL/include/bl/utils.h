#ifndef BL_UTILS_H
#define BL_UTILS_H
#include "types.h"

void set_GDT_entry(GDT_entry* entry, word_t basel, word_t baseh, word_t limitl, word_t limith, GDT_flags flags, GDT_access access);
void set_GDTR48(GDTR48* gdtr, const void* entries, size_t size);

extern word_t get_ds();
extern void load_GDT(GDTR48);
extern void enter_unreal(word_t data_segment);
extern void init_crc32();
extern void crc32(const void* data, word_t data_size, dword_t* out);

extern void _add_dwords(dword_t, dword_t, dword_t*);
extern void _sub_dwords(dword_t, dword_t, dword_t*);
extern void _mul_dwords(dword_t, dword_t, dword_t*);
extern void _div_dwords(dword_t, dword_t, dword_t*);
extern void _mod_dwords(dword_t, dword_t, dword_t*);

extern void _add_qwords(qword_t, qword_t, qword_t*);

#ifdef _VLGBL_CUSTOM_DWORDS
#define dword_is_zero(a)                    (a.datal == 0 && a.datah == 0)
#define zero_dword(a)                       do { a.datal = 0; a.datah = 0; } while(0)
#define assign_word_to_dword(dword, word)   do { zero_dword(dword); dword.datal = word; } while(0)
#define assign_dword_to_word(word, dword)   word = dword.datal
#define add_dwords(a, b, ret)               _add_dwords(a, b, (dword_t*)&ret)
#define sub_dwords(a, b, ret)               _sub_dwords(a, b, (dword_t*)&ret)
#define mul_dwords(a, b, ret)               _mul_dwords(a, b, (dword_t*)&ret)
#define div_dwords(a, b, ret)               _div_dwords(a, b, (dword_t*)&ret)
#define mod_dwords(a, b, ret)               _mod_dwords(a, b, (dword_t*)&ret)
#else
#define dword_is_zero(a)                    (a == 0)
#define zero_dword(a)                       a = 0
#define assign_word_to_dword(dword, word)   dword = (dword_t)word
#define assign_dword_to_word(word, dword)   word = (word_t)dword
#define add_dwords(a, b, ret)               *(dword_t*)&ret = (dword_t)a + (dword_t)b
#define sub_dwords(a, b, ret)               *(dword_t*)&ret = (dword_t)a - (dword_t)b
#define mul_dwords(a, b, ret)               *(dword_t*)&ret = (dword_t)a * (dword_t)b
#define div_dwords(a, b, ret)               *(dword_t*)&ret = (dword_t)a / (dword_t)b
#define mod_dwords(a, b, ret)               *(dword_t*)&ret = (dword_t)a % (dword_t)b
#endif

#ifdef _VLGBL_CUSTOM_QWORDS
#define qword_is_zero(a)                    (dword_is_zero(a.datal) && dword_is_zero(a.datah))
#define zero_qword(a)                       do { zero_dword(a.datal); zero_dword(a.datah); } while(0)
#define assign_word_to_qword(qword, word)   do { zero_qword(qword); qword.datal.datal = word; } while(0)
#define assign_dword_to_qword(qword, dword) do { zero_qword(qword); qword.datal = dword; } while(0)
#else
#define qword_is_zero(a)                    (a == 0)
#define zero_qword(a)                       a = 0
#define assign_word_to_qword(qword, word)   qword = (qword_t)word
#define assign_dword_to_qword(qword, dword) qword = (qword_t)dword
#endif

#endif
