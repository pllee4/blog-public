ENTRY(nomain)

SECTIONS
{
    . = 0x08048000 + SIZEOF_HEADERS;

    text : {*(.text) *(.data) *(.rodata)}

    /DISCARD/: {*(comment)}
}