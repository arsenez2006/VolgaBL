#include <bl/utils.h>

void set_GDT_entry(GDT_entry *entry, word_t basel, word_t baseh, word_t limitl, word_t limith, GDT_flags flags, GDT_access access) {
    *(word_t*)&entry->data[0] = limitl;
    *(word_t*)&entry->data[2] = basel;
    *(byte_t*)&entry->data[4] = (byte_t)(baseh & 0xFF);
    *(byte_t*)&entry->data[5] = (byte_t)access;
    *(byte_t*)&entry->data[6] = (byte_t)((limith & 0x0F) | (flags << 4));
    *(byte_t*)&entry->data[7] = (byte_t)(baseh >> 8);
}

void set_GDTR48(GDTR48 *gdtr, const void *entries, size_t size) {
    *(word_t*)&gdtr->data[0] = (word_t)(size - 1);

    dword_t segment;
    assign_word_to_dword(segment, get_ds() << 4);

    dword_t offset;
    assign_word_to_dword(offset, (uintptr_t)entries);

    add_dwords(segment, offset, gdtr->data[2]);
}
