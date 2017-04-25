#enables interrupts globally then returns to the caller with jr
.global enable_interrupt
.text
enable_interrupt:
  ei
  jr $ra
  nop