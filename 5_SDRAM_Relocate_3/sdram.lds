SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00)
  {
    _data_load_addr = LOADADDR(.data);
    _data_start = . ; /* Current location=0x30000000 */
    *(.data)
    _data_end = . ; /* Current location=0x30000000 + sizeof(*(.data)) */
  }
  _bss_start=.;
  .bss :{*(.bss) *(.common)}
  _bss_end=.;
}
