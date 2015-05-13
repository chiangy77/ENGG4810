#ifndef RTC_H_
#define RTC_H_

#define SLAVE_ADDR	0x68
#define READ		1
#define WRITE		0
#define TIME_ADDR	0x00
#define DATE_ADDR	0x04
#define SEC_ADDR	0x00
#define MIN_ADDR	0x01
#define HOUR_ADDR	0x02
#define DAY_ADDR	0x04
#define MON_ADDR	0x05
#define YEAR_ADDR	0x06

void InitI2C0();
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...);
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg);
void getTime(int *pHour, int *pMinute, int *pSecond);
void getDate (int *pYear, int *pMonth, int *pDay);

#endif /* RTC_H_ */
