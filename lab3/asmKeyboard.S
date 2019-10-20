#include <machine/asm.h>
#include "i8042.h"

.intel_syntax #we are more familiar with the intel syntax
.global _C_LABEL(scancode) #granting access to the global varibale scancode
.global _C_LABEL(kbc_asm_ih) #declaring kbc_asmih()

.data
_C_LABEL(scancode):
  .byte 0

.text
_C_LABEL(kbc_asm_ih):
  #starts by checking the status register
    in al, STATUS_REG
    and al, KBC_OUTB
    cmp al, 0x01
    jne fim  #if the output buffer flag isn't set there's nothing to read -> jump to the end
    and al, (KBC_PARE | KBC_TIMEO | KBC_AUXB)
    cmp al, 0x00
    jne fim  #if there was some error (parity, timeout) or if there the data in the buffer is from the mouse -> jump to the end
  #out buffer has good data, now it will read the scancode
    in al, OUT_BUF
    mov byte ptr [_C_LABEL(scancode)], al
fim:ret
