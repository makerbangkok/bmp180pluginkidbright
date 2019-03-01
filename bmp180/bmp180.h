#ifndef __BMP180_H__
#define __BMP180_H__

#include "driver.h"
#include "device.h"
#include "i2c-dev.h"

#define BMP180_POLLING_MS					1000

#define BMP180_ADDR 0x77 // 7-bit address

#define	BMP180_REG_CONTROL 0xF4
#define	BMP180_REG_RESULT 0xF6

#define	BMP180_COMMAND_TEMPERATURE 0x2E
#define	BMP180_COMMAND_PRESSURE0 0x34
#define	BMP180_COMMAND_PRESSURE1 0x74
#define	BMP180_COMMAND_PRESSURE2 0xB4
#define	BMP180_COMMAND_PRESSURE3 0xF4


class BMP180 : public Device {
	private:
		enum {
			 s_detect, s_read_cal, s_read_temp, s_read_hpa, s_error, s_wait
		} state;
		TickType_t tickcnt, polling_tickcnt;
		bool first_read;
		double hpa;
		double temperature;

		int16_t AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
		uint16_t AC4,AC5,AC6;
		double c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;
		char _error;

		double c3,c4,b1;
		double tu, a;
		double pu,s,x,y,z;


	public:
		// constructor
		BMP180(int bus_ch, int dev_addr);
		// override
		void init(void);
		void process(Driver *drv);
		int prop_count(void);
		bool prop_name(int index, char *name);
		bool prop_unit(int index, char *unit);
		bool prop_attr(int index, char *attr);
		bool prop_read(int index, char *value);
		bool prop_write(int index, char *value);
		// method
		double get_hpa(void);
		double get_temperature(void);
};

#endif
