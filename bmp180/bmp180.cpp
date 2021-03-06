#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "bmp180.h"

BMP180::BMP180(int bus_ch, int dev_addr) {
	channel = bus_ch;
	address = dev_addr;
	polling_ms = BMP180_POLLING_MS;
}

void BMP180::init(void) {
	first_read = true;
	hpa = 0;
	temperature = 0;
	state = s_detect;
}

int BMP180::prop_count(void) {
	return 2;
}

bool BMP180::prop_name(int index, char *name) {
	if (index == 0) {
		snprintf(name, DEVICE_PROP_NAME_LEN_MAX, "%s", "temperature");
		return true;
	}
	else
	if (index == 1) {
		snprintf(name, DEVICE_PROP_NAME_LEN_MAX, "%s", "hPa");
		return true;
	}

	// not supported
	return false;
}

bool BMP180::prop_unit(int index, char *unit) {
	if (index == 0) {
		snprintf(unit, DEVICE_PROP_UNIT_LEN_MAX, "%sC", "\xc2\xb0");
		return true;
	}
	else
	if (index == 1) {
		snprintf(unit, DEVICE_PROP_UNIT_LEN_MAX, "%s", "hPa");
		return true;
	}

	// not supported
	return false;
}

bool BMP180::prop_attr(int index, char *attr) {
	if ((index == 0) || (index == 1)) {
		get_attr_str(attr, PROP_ATTR_READ_FLAG | PROP_ATTR_TYPE_NUMBER_FLAG); // read only, number
		return true;
	}

	// not supported
	return false;
}

bool BMP180::prop_read(int index, char *value) {
	if (index == 0) {
		snprintf(value, DEVICE_PROP_VALUE_LEN_MAX, "%f", temperature);
		return true;
	}
	else
	if (index == 1) {
		snprintf(value, DEVICE_PROP_VALUE_LEN_MAX, "%f", hpa);
		return true;
	}

	// not supported
	return false;
}

bool BMP180::prop_write(int index, char *value) {
	// not supported
	return false;
}

void BMP180::process(Driver *drv) {
	I2CDev *i2c = (I2CDev *)drv;
	uint8_t byte, data[2], data2[3], tempdata[2];
	int16_t value;

	//printf("state : %d\r\n",state);
	switch (state) {
		case s_detect:
			// stamp polling tickcnt
			polling_tickcnt = get_tickcnt();
			// detect i2c device
			if (i2c->detect(channel, address) == ESP_OK) {

				// clear error flag
				error = false;
				// get current tickcnt
				tickcnt = get_tickcnt();
				state = s_read_cal;
			} else {
				state = s_error;
			}
		break;

		case s_read_cal:

			byte = 0xAA;
			if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
				AC1 = (int16_t)((data[0]<<8)|data[1]);
				//printf("%d\r\n",AC1);

			  byte = 0xAC;
				if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
					AC2 = (int16_t)((data[0]<<8)|data[1]);
					//printf("%d\r\n",AC2);

					byte = 0xAE;
					if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
						AC3 = (int16_t)((data[0]<<8)|data[1]);
						//printf("%d\r\n",AC3);

						byte = 0xB0;
						if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
							AC4 = (int16_t)((data[0]<<8)|data[1]);
							//printf("%d\r\n",AC4);

							byte = 0xB2;
							if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
								AC5 = (int16_t)((data[0]<<8)|data[1]);
								//printf("%d\r\n",AC5);

								byte = 0xB4;
								if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
									AC6 = (int16_t)((data[0]<<8)|data[1]);
									//printf("%d\r\n",AC6);

									byte = 0xB6;
									if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
										VB1 = (int16_t)((data[0]<<8)|data[1]);
										//printf("%d\r\n",VB1);

										byte = 0xB8;
										if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
											VB2 = (int16_t)((data[0]<<8)|data[1]);
											//printf("%d\r\n",VB2);

											byte = 0xBA;
											if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
												MB = (int16_t)((data[0]<<8)|data[1]);
												//printf("%d\r\n",MB);

												byte = 0xBC;
												if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
													MC = (int16_t)((data[0]<<8)|data[1]);
													//printf("%d\r\n",MC);

													byte = 0xBE;
													if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
														MD = (int16_t)((data[0]<<8)|data[1]);
														//printf("%d\r\n",MD);

														// Compute floating-point polynominals:
														c3 = 160.0 * pow(2,-15) * AC3;
														c4 = pow(10,-3) * pow(2,-15) * AC4;
														b1 = pow(160,2) * pow(2,-30) * VB1;
														c5 = (pow(2,-15) / 160) * AC5;
														c6 = AC6;
														mc = (pow(2,11) / pow(160,2)) * MC;
														md = MD / 160.0;
														x0 = AC1;
														x1 = 160.0 * pow(2,-13) * AC2;
														x2 = pow(160,2) * pow(2,-25) * VB2;
														y0 = c4 * pow(2,15);
														y1 = c4 * c3;
														y2 = c4 * b1;
														p0 = (3791.0 - 8.0) / 1600.0;
														p1 = 1.0 - 7357.0 * pow(2,-20);
														p2 = 3038.0 * 100.0 * pow(2,-36);

														tickcnt = get_tickcnt();
														state = s_read_temp;
													} else {
														state = s_error;
													}
												} else {
													state = s_error;
												}
											} else {
												state = s_error;
											}
										} else {
											state = s_error;
										}
									} else {
										state = s_error;
									}
								} else {
									state = s_error;
								}
							} else {
								state = s_error;
							}
						} else {
							state = s_error;
						}
					} else {
						state = s_error;
					}
				} else {
					state = s_error;
				}
			} else {
				state = s_error;
			}

		break;

		case s_read_temp:
				data[0] = BMP180_REG_CONTROL;
				data[1] = BMP180_COMMAND_TEMPERATURE;
				if (i2c->write(channel, address, data, 2) == ESP_OK) {
					// clear error flag
					error = false;

					if (is_tickcnt_elapsed(tickcnt, 4.5)) {
					byte = BMP180_REG_RESULT;
					if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
						//tu = (int16_t)((data[0]<<8)|data[1]);
						//tu = ((data[0]<<8)|data[1]);
						//tu = (data[0] * 256.0) + data[1];
						tu = (data[0] * 256.0) + data[1];

						//a = c5 * (tu - c6);
						//temperature = a + (mc / (a + md));
						//printf("tu:%lf\r\n",tu);
						//printf("a:%lf\r\n",a);
						//printf("temp:%lf\r\n",temperature);

						state = s_read_hpa;
					} else {
						state = s_error;
					}
				}
			} else {
					state = s_error;
			}

		break;

		case s_read_hpa:
				data2[0] = BMP180_REG_CONTROL;
				data2[1] = BMP180_COMMAND_PRESSURE2;
				if (i2c->write(channel, address, data2, 2) == ESP_OK) {
					// clear error flag
					error = false;

					if (is_tickcnt_elapsed(tickcnt, 13.5)) {
					byte = BMP180_REG_RESULT;
					if (i2c->read(channel, address, &byte, 1, data2, 3) == ESP_OK) {
						//tu = (int16_t)((data[0]<<8)|data[1]);
						//tu = ((data[0]<<8)|data[1]);
						//tu = (data[0] * 256.0) + data[1];
						pu = (data2[0] * 256.0) + data2[1] + (data2[2]/256.0);

						a = c5 * (pu - c6);		//it wied pu and tu has swap
						temperature = a + (mc / (a + md));

						s = temperature - 25.0;
						x = (x2 * pow(s,2)) + (x1 * s) + x0;
						y = (y2 * pow(s,2)) + (y1 * s) + y0;
						z = (tu - x) / y;
						hpa = (p2 * pow(z,2)) + (p1 * z) + p0;
						printf("tu:%lf\r\n",tu);
						printf("pu:%lf\r\n",pu);
						//printf("s:%lf\r\n",s);
						//printf("x:%lf\r\n",x);
						//printf("y:%lf\r\n",y);
						//printf("z:%lf\r\n",z);
						printf("temp:%lf\r\n",temperature);
						printf("hpa:%lf\r\n",hpa);
						printf("\r\n");

						// set initialized flag
						initialized = true;

						// load polling tickcnt
						tickcnt = polling_tickcnt;
						state = s_wait;
					} else {
						state = s_error;
					}
				}
			} else {
					state = s_error;
			}

		break;


		case s_error:
			// set error flag
			error = true;
			// clear initialized flag
			initialized = false;
			// set first read flag
			first_read = true;
			// get current tickcnt
			tickcnt = get_tickcnt();
			// goto wait and retry with detect state
			state = s_wait;
			break;

		case s_wait:
			// wait polling_ms timeout
			if (is_tickcnt_elapsed(tickcnt, polling_ms)) {
				state = s_detect;
			}
			break;
	}
}

double BMP180::get_hpa(void) {
	return hpa;
}

double BMP180::get_temperature(void) {
	return temperature;
}
