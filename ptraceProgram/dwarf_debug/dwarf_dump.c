#include <libdwarf/libdwarf.h>
#include <libdwarf/dwarf.h>
#include <stdio.h>
#include <stdlib.h>

void print_function_info(Dwarf_Debug dbg, Dwarf_Die die);

void recurse_die(Dwarf_Debug dbg, Dwarf_Die die) {
    Dwarf_Error err;
    char* die_name = NULL;
    Dwarf_Half tag;
    int res;

    res = dwarf_tag(die, &tag, &err);
    if (res != DW_DLV_OK) return;

    if (tag == DW_TAG_subprogram) {
        print_function_info(dbg, die);
    }

    // 递归遍历子节点
    Dwarf_Die child = NULL;
    res = dwarf_child(die, &child, &err);
    if (res == DW_DLV_OK) {
        recurse_die(dbg, child);
        dwarf_dealloc(dbg, child, DW_DLA_DIE);
    }

    // 遍历兄弟节点
    Dwarf_Die sibling = NULL;
    res = dwarf_siblingof(dbg, die, &sibling, &err);
    if (res == DW_DLV_OK) {
        recurse_die(dbg, sibling);
        dwarf_dealloc(dbg, sibling, DW_DLA_DIE);
    }
}

void print_function_info(Dwarf_Debug dbg, Dwarf_Die die) {
    Dwarf_Error err;
    char* name = NULL;
    Dwarf_Addr lowpc = 0;
    Dwarf_Addr highpc_val = 0;
    Dwarf_Addr highpc = 0;

    if (dwarf_diename(die, &name, &err) != DW_DLV_OK) {
        name = "(null)";
    }

    if (dwarf_lowpc(die, &lowpc, &err) != DW_DLV_OK) {
        lowpc = 0;
    }

    int res = dwarf_highpc(die, &highpc_val, &err);
    if (res == DW_DLV_OK) {
        highpc = lowpc + highpc_val;
    } else {
        highpc = 0;
    }

    printf("Function: %s, low_pc = 0x%lx, high_pc = 0x%lx, size = 0x%lx\n",
           name, (unsigned long)lowpc, (unsigned long)highpc,
           (unsigned long)(highpc > lowpc ? highpc - lowpc : 0));

    if (name != NULL) dwarf_dealloc(dbg, name, DW_DLA_STRING);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    Dwarf_Debug dbg = 0;
    Dwarf_Error err;
    int fd = open(argv[1], 0);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (dwarf_init(fd, DW_DLC_READ, NULL, NULL, &dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "dwarf_init failed\n");
        close(fd);
        return 1;
    }

    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version, address_size;
    int res;

    while ((res = dwarf_next_cu_header(dbg, &cu_header_length, &version,
                                      &abbrev_offset, &address_size,
                                      &next_cu_header, &err)) == DW_DLV_OK) {
        Dwarf_Die no_die = NULL;
        Dwarf_Die cu_die = NULL;

        if (dwarf_siblingof(dbg, no_die, &cu_die, &err) != DW_DLV_OK) {
            fprintf(stderr, "Failed to get CU DIE\n");
            break;
        }

        recurse_die(dbg, cu_die);
        dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
    }

    dwarf_finish(dbg, &err);
    close(fd);
    return 0;
}

