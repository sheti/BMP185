#ifndef BMP185_h
#define BMP185_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define BMP185_ADDR 0x77 //0x77 default I2C address
#define BMP185_CTRL_MEAS 0xF4

class BMP185 {
	public:  
		BMP185();
		void init();
		long getPressure();
		long getTemperature();
		void setOversamplig(byte _oss);
		long temperature;
	private:
		byte oss;
		short ac1, ac2, ac3;
		word ac4, ac5, ac6;
		short b1, b2;
		short mb;
		short mc;
		short md;
		long b5;
		byte getStartOfConversion();
		void readCalibrationData();
		void write8(byte _addr, byte _val);
		byte read8(byte _addr);
		word read16(byte _addr);
		unsigned long read32(byte _addr);
		byte *read(byte _addr, byte n);
};
#endif
