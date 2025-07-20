/* _SFR_MEM32(0x001C) is a dummy value. 0x001C is the first GPIOR.
 *  but in practice, it doesn't matter since
 *  #define _SFR_MEM32(mem_addr) _MMIO_DWORD(mem_addr)
 *  and
 *  _MMIO_DWORD(mem_addr) (*(volatile uint32_t *)(mem_addr))
 *
 *  any volatile variable should do the trick just as well
 * _SFR_MEM32(0x001C) = (some 32-bit value) will store a 32-bit value in the 4 GPIO registers, which under typical circumstances takes
 * 4 words instead of 8 words.
 */

void setup() {
  /* test manipulation functions */
  #ifndef MILLIS_USE_TIMERNONE
    stop_millis();
    set_millis(_SFR_MEM32(0x001C));
    restart_millis();
  #endif
}
void loop() {
  #ifndef MILLIS_USE_TIMERNONE
    _SFR_MEM32(0x001C)=millis();
    #ifndef MILLIS_USE_TIMERRTC
      _SFR_MEM32(0x001C)=micros();
    #endif
  #endif
}
