#ifndef _KBC_H_ 

#include "i8254.h"

#include "lcom/timer.h"

//reads information from kbc
int read_kbc(uint32_t *data, int port);
//issues commands to kbc
int kbc_issue_command(uint32_t cmd, int port);
//reads information (interrupt handlers)
int read_information(uint8_t *data, uint8_t IRQ);
//reads command byte
int read_cmd_b(uint32_t *cmdb);
//writes to command byte
int issue_cmd_b(uint32_t cmdb);
//reads information for polling
int read_information_pl(uint8_t *data,uint8_t IRQ);



#endif // _KBC_H_
