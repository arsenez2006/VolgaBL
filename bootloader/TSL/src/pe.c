#include <bl/pe.h>
#include <bl/string.h>

typedef struct __packed dos_header {
  word_t  e_magic;
  byte_t  _unused0[58];
  dword_t e_lfanew;
} dos_header;

typedef struct __packed file_header {
  word_t  machine;
  word_t  number_of_sections;
  dword_t time_date_stamp;
  dword_t pointer_to_symbol_table;
  dword_t number_of_symbols;
  word_t  size_of_optional_header;
  word_t  characteristics;
} file_header;

typedef struct __packed optional_header {
  word_t  magic;
  byte_t  major_linker_version;
  byte_t  minor_linker_version;
  dword_t size_of_code;
  dword_t size_of_initialized_data;
  dword_t size_of_uninitialized_data;
  dword_t address_of_entry_point;
  dword_t base_of_code;
  dword_t base_of_data;

} optional_header;

typedef struct __packed pe_header {
  dword_t         signature;
  file_header     file_header;
  optional_header optional_header;
} pe_header;

typedef struct __packed section_header {
  byte_t  name[8];
  dword_t virtual_size;
  dword_t virtual_address;
  dword_t size_of_raw_data;
  dword_t pointer_to_raw_data;
  dword_t pointer_to_relocations;
  dword_t pointer_to_line_numbers;
  word_t  number_of_relocations;
  word_t  number_of_line_numbers;
  dword_t characteristics;
} section_header;

bool pe_load(void* pe_addr, void* load_addr, pe_load_state* state) {
  dos_header*     dos_hdr;
  pe_header*      pe_hdr;

  size_t          sizeof_headers;
  size_t          image_size;

  section_header* sections;
  size_t          sections_count;

  size_t          i;
  /* Verify MZ and PE signatures */
  dos_hdr = pe_addr;
  if (dos_hdr->e_magic != 0x5A4D) {
    return false;
  }
  pe_hdr = (pe_header*)((byte_t*)pe_addr + dos_hdr->e_lfanew);
  if (pe_hdr->signature != 0x00004550) {
    return false;
  }
  if (pe_hdr->optional_header.magic != 0x020B) {
    return false;
  }

  /* Get sections */
  sections_count = pe_hdr->file_header.number_of_sections;
  sections       = (section_header*)((byte_t*)&pe_hdr->optional_header +
                               pe_hdr->file_header.size_of_optional_header);

  /* Load headers */
  sizeof_headers = (byte_t*)pe_hdr - (byte_t*)pe_addr + sizeof(pe_header) -
                   sizeof(optional_header) +
                   pe_hdr->file_header.size_of_optional_header +
                   sizeof(section_header) * sections_count;
  memcpy(load_addr, pe_addr, sizeof_headers);

  /* Load sections */
  for (i = 0; i < sections_count; ++i) {
    memcpy(
        (byte_t*)load_addr + sections[i].virtual_address,
        (char*)pe_addr + sections[i].pointer_to_raw_data,
        sections[i].size_of_raw_data
    );
  }

  /* Calculate load size */
  image_size = sections[i - 1].virtual_address + sections[i - 1].virtual_size;

  /* Fill Load state */
  state->load_addr  = (dword_t)load_addr;
  state->image_size = image_size;
  state->entry =
      pe_hdr->optional_header.address_of_entry_point + (dword_t)load_addr;

  return true;
}
