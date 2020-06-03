SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00)
  {
    data_load_addr = LOADADDR(.data);
    data_start = . ; /* Current location=0x30000000 */
    *(.data)
    data_end = . ; /* Current location=0x30000000 + sizeof(*(.data)) */
  }
  .bss :{*(.bss) *(.common)}
}
