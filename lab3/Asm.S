#include  <machine/asm.h>


#include "i8042.h"
#include "i8254.h"


.global _C_LABEL(data) /* variable to save information for*/
.global _C_LABEL(ih_bool) /*to check for errors*/

.global  _C_LABEL(kbc_asm_ih)

.text
_C_LABEL(kbc_asm_ih):
xor %eax,%eax /* eax =0*/
    inb $STAT_REG,%al  /*loads information from status register*/
    /*tests if output buffer is full*/ 
    testb $OBF,%al       
    jz error_buff_full   
    testb $PAR_ERR, %al   
    jnz error_parr
    testb $TO_ERR, %al
    jnz error_timeout
    testb $AUX_SET, %al
    jnz error_aux
    /*loads information from output buffer*/
    inb  $OUT_BUF ,%al
    /* loads information from output buffer to data(variable)*/
    movb %al,data
    xor %eax, %eax
    jmp end


/*error handling*/
error_buff_full:
movb $1, ih_bool
jmp end

error_parr:
movb $2, ih_bool
jmp end


error_timeout:
movb $3, ih_bool
jmp end

error_aux:
movb $4, ih_bool
jmp end


end:
ret 



