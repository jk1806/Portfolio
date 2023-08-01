#ifndef FIRMWARE_H
#define FIRMWARE_H

int firmware_main(void);
int anti_tamper_init(void);
void anti_tamper_check(void);
void mpu_init(void);
int mpu_check_access(uint32_t address, uint32_t access_type);

#endif /* FIRMWARE_H */

