#if defined(CCL_CCL_vect)
void Logic::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  CCL_INTMODE0_t intmode;
  switch (mode) { // Set RISING, FALLING or CHANGE interrupt trigger for a block output
    case RISING:
      intmode = CCL_INTMODE0_RISING_gc;
      break;
    case FALLING:
      intmode = CCL_INTMODE0_FALLING_gc;
      break;
    case CHANGE:
      intmode = CCL_INTMODE0_BOTH_gc;
      break;
    default: // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #if defined(CCL_TRUTH4)
  if (block.number > 3) {
    const int16_t intmode_bp = (block.number & 0x03) * 2;
    CCL.INTCTRL1 = (CCL.INTCTRL1 & ~(CCL_INTMODE0_gm << intmode_bp)) | (intmode << intmode_bp);
  } else {
    const int16_t intmode_bp = (block.number & 0x03) * 2;
    CCL.INTCTRL0 = (CCL.INTCTRL0 & ~(CCL_INTMODE0_gm << intmode_bp)) | (intmode << intmode_bp);
  }
  #else
  const int16_t intmode_bp = block.number * 2;
  CCL.INTCTRL0 = (CCL.INTCTRL0 & ~(CCL_INTMODE0_gm << intmode_bp)) | (intmode << intmode_bp);
  #endif
  // Store function pointer
  intFuncCCL[block.number] = userFunc;
}

void Logic::detachInterrupt() {
  // Disable interrupt for a given block output
  #if defined(CCL_TRUTH4)
  if (block.number > 4) {
    CCL.INTCTRL1 &= ~(CCL_INTMODE1_gm << ((block.number & 3) * 2));
  } else {
    CCL.INTCTRL0 &= ~(CCL_INTMODE0_gm << (block.number * 2));
  }
  #else
  CCL.INTCTRL0 &= ~(CCL_INTMODE0_gm << (block.number * 2));
  #endif
}

// CCL interrupt service routine
// Use attachIntterupt to activate this.
ISR(CCL_CCL_vect) {
  // Check for block 0 interrupt
  if (CCL.INTFLAGS & CCL_INT0_bm) {
    // Run user function
    intFuncCCL[CCL_INT0_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT0_bm;
  }
  // Check for block 1 interrupt
  if (CCL.INTFLAGS & CCL_INT1_bm) {
    // Run user function
    intFuncCCL[CCL_INT1_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT1_bm;
  }
  // Check for block 2 interrupt
  if (CCL.INTFLAGS & CCL_INT2_bm) {
    // Run user function
    intFuncCCL[CCL_INT2_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT2_bm;
  }
  // Check for block 3 interrupt
  if (CCL.INTFLAGS & CCL_INT3_bm) {
    // Run user function
    intFuncCCL[CCL_INT3_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT3_bm;
  }
  #if defined(TRUTH4)
  // Check for block 4 interrupt
  if (CCL.INTFLAGS & CCL_INT4_bm) {
    // Run user function
    intFuncCCL[CCL_INT4_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT4_bm;
  }
  #endif
  #if defined(TRUTH5)
  // Check for block 5 interrupt
  if (CCL.INTFLAGS & CCL_INT5_bm) {
    // Run user function
    intFuncCCL[CCL_INT5_bp]();
    // Clear flag
    CCL.INTFLAGS |= CCL_INT5_bm;
  }
  #endif // CCL_CCL_vect
}

#endif
