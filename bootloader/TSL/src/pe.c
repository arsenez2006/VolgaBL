#include <bl/io.h>
#include <bl/pe.h>
#include <bl/ramfs.h>
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

typedef struct __packed data_directoru {
  dword_t virtual_address;
  dword_t size;
} data_directoru;

typedef struct __packed optional_header {
  word_t         magic;
  byte_t         major_linker_version;
  byte_t         minor_linker_version;
  dword_t        size_of_code;
  dword_t        size_of_initialized_data;
  dword_t        size_of_uninitialized_data;
  dword_t        address_of_entry_point;
  dword_t        base_of_code;

  qword_t        image_base;
  dword_t        section_alignment;
  dword_t        file_alignment;
  word_t         major_operating_system_version;
  word_t         minor_operating_system_version;
  word_t         major_image_version;
  word_t         minor_image_version;
  word_t         major_subsystem_version;
  word_t         minor_subsystem_version;
  dword_t        win32_version_value;
  dword_t        size_of_image;
  dword_t        size_of_headers;
  dword_t        checksum;
  word_t         subsystem;
  word_t         dll_characteristics;
  qword_t        size_of_stack_reserve;
  qword_t        size_of_stack_commit;
  qword_t        size_of_heap_reserve;
  qword_t        size_of_heap_commit;
  dword_t        loader_flags;
  dword_t        number_of_rva_and_sizes;

  data_directoru data_directories[16];
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

typedef struct __packed import_directory {
  dword_t import_lookup_rva;
  dword_t time_stamp;
  dword_t forwarder_chain;
  dword_t name_rva;
  dword_t import_address_table_rva;
} import_directory;

typedef struct __packed export_directory {
  dword_t flags;
  dword_t time_stamp;
  word_t  major_version;
  word_t  minor_version;
  dword_t name_rva;
  dword_t ordinal_base;
  dword_t address_table_count;
  dword_t number_of_name_pointers;
  dword_t export_address_table_rva;
  dword_t name_pointer_rva;
  dword_t ordinal_table_rva;
} export_directory;

#define STATES_MAX 16

static struct {
  pe_load_state states[STATES_MAX + 1];
} _ctx;

bool pe_loader_init(void* start) {
  memset(&_ctx, 0, sizeof _ctx);
  _ctx.states[0].load_addr = (dword_t)start;
  return true;
}

void pe_get_memory_range(dword_t* begin, dword_t* end) {
  size_t i;

  if (begin) {
    *begin = _ctx.states[0].load_addr;
  }
  if (end) {
    for (i = STATES_MAX - 1; i >= 0; --i) {
      if (_ctx.states[i].load_addr != 0) {
        *end = _ctx.states[i].load_addr + _ctx.states[i].image_size;
        break;
      }
    }
  }
}

bool pe_load(char const* filename, pe_load_state** state) {
  void*           pe_addr;
  pe_load_state*  ret;

  dos_header*     dos_hdr;
  pe_header*      pe_hdr;

  section_header* sections;
  size_t          sections_count;

  size_t          i;

  /* Check if already loaded */
  ret = NULL;
  for (i = 0; i < STATES_MAX; ++i) {
    size_t name_len = strlen(_ctx.states[i].name);
    if (name_len == 0) {
      ret = &_ctx.states[i];
      break;
    } else if (!memcmp(_ctx.states[i].name, filename, name_len + 1)) {
      if (state) {
        *state = &_ctx.states[i];
      }
      return true;
    }
  }
  if (ret == NULL) {
    return false;
  }

  /* Open the file */
  if ((pe_addr = ramfs_file(filename, NULL)) == NULL) {
    return false;
  }

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
  memcpy(
      (void*)ret->load_addr, pe_addr, pe_hdr->optional_header.size_of_headers
  );

  /* Load sections */
  for (i = 0; i < sections_count; ++i) {
    memcpy(
        (byte_t*)ret->load_addr + sections[i].virtual_address,
        (char*)pe_addr + sections[i].pointer_to_raw_data,
        sections[i].size_of_raw_data
    );
  }

  /* Fill Load state */
  snprintf(ret->name, sizeof ret->name, "%s", filename);
  ret->image_size = (dword_t)pe_hdr->optional_header.size_of_image;
  ret->entry = ret->load_addr + pe_hdr->optional_header.address_of_entry_point;
  ret->stack_size = pe_hdr->optional_header.size_of_stack_commit;

  if (ret + 1 - &_ctx.states[0] < STATES_MAX) {
    (ret + 1)->load_addr = ret->load_addr + ret->image_size;
  }

  /* Parse import table */
  if (pe_hdr->optional_header.data_directories[1].size != 0) {
    import_directory* dll_dir;
    /* Go through DLLs */
    for (dll_dir = (import_directory*)(ret->load_addr + pe_hdr->optional_header
                                                            .data_directories[1]
                                                            .virtual_address);
         dll_dir->import_lookup_rva != 0;
         ++dll_dir) {
      char              dll_path[256];
      pe_load_state*    dll;
      pe_header*        dll_pe_hdr;
      export_directory* export_dir;
      dword_t*          export_table;
      dword_t*          name_table;
      word_t*           ordinal_table;
      qword_t*          address_table;

      /* Load DLL */
      snprintf(
          dll_path,
          sizeof dll_path,
          "ramfs/%s",
          (char*)(ret->load_addr + dll_dir->name_rva)
      );
      if (!pe_load(dll_path, &dll)) {
        return false;
      }

      /* Get DLL export directory */
      dll_pe_hdr = (pe_header*)(dll->load_addr +
                                (((dos_header*)dll->load_addr)->e_lfanew));
      if (dll_pe_hdr->optional_header.data_directories[0].size == 0) {
        return false;
      }
      export_dir =
          (export_directory*)(dll->load_addr +
                              dll_pe_hdr->optional_header.data_directories[0]
                                  .virtual_address);

      /* Get export tables */
      export_table =
          (dword_t*)(dll->load_addr + export_dir->export_address_table_rva);
      name_table    = (dword_t*)(dll->load_addr + export_dir->name_pointer_rva);
      ordinal_table = (word_t*)(dll->load_addr + export_dir->ordinal_table_rva);

      /* Go through symbols */
      for (address_table =
               (qword_t*)(ret->load_addr + dll_dir->import_address_table_rva);
           *address_table != 0;
           ++address_table) {
        if (*address_table & (1ULL << 63)) {
          /* // TODO: Import by ordinal */
          return false;
        } else {
          /* Import by name */
          word_t      symbol_index;
          char const* symbol_name;

          symbol_index = *(word_t*)(ret->load_addr +
                                    (dword_t)(*address_table & 0xFFFFFFFF));
          symbol_name  = (char*)(ret->load_addr +
                                (dword_t)(*address_table & 0xFFFFFFFF) + 2);

          /* Find symbol in export name table */
          if (!memcmp(
                  symbol_name,
                  (byte_t*)dll->load_addr + name_table[symbol_index],
                  strlen(symbol_name) + 1
              )) {
            i = symbol_index;
          } else {
            bool found = false;
            for (i = 0; i < export_dir->number_of_name_pointers; ++i) {
              if (!memcmp(
                      symbol_name,
                      (byte_t*)dll->load_addr + name_table[i],
                      strlen(symbol_name) + 1
                  )) {
                found = true;
                break;
              }
            }
            if (!found) {
              return false;
            }
          }

          /* Bind symbol */
          *address_table = dll->load_addr + export_table[ordinal_table[i]];
        }
      }
    }
  }

  if (state != NULL) {
    *state = ret;
  }

  return true;
}
