#ifndef BL_UTILS_H
#define BL_UTILS_H
#include "types.h"

void set_GDT_entry(GDT_entry* entry, word_t basel, word_t baseh, word_t limitl, word_t limith, GDT_flags flags, GDT_access access);
void set_GDTR48(GDTR48* gdtr, const void* entries, size_t size);

extern word_t get_ds();
extern void load_GDT(GDTR48);
extern void enter_unreal(word_t data_segment);

extern void _add_dwords(dword_t, dword_t, dword_t*);
extern void _sub_dwords(dword_t, dword_t, dword_t*);

#ifdef _VLGBL_CUSTOM_DWORDS
#define add_dwords(a, b, ret) _add_dwords(a, b, (dword_t*)&ret)
#define sub_dwords(a, b, ret) _sub_dwords(a, b, (dword_t*)&ret)
#else
#define add_dwords(a, b, ret) *(dword_t*)&ret = (dword_t)a + (dword_t)b
#define sub_dwords(a, b, ret) *(dword_t*)&ret = (dword_t)a - (dword_t)b
#endif

#endif
