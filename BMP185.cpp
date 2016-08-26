#include <Wire.h>
#include <BMP185.h>

BMP185::BMP185() { };

void BMP185::init() {
	readCalibrationData();
	oss = 0;
}

long BMP185::getTemperature() {
	write8(BMP185_CTRL_MEAS, 0x2E);
	byte _sco = 0x20;
	do	{
		delayMicroseconds(4500);
		_sco = getStartOfConversion();
	} while(_sco > 0);
	long ut = (long)read16(0xF6);
	long x1 = (ut - ac6) * ac5 >> 15;
	long x2 = ((long)mc << 11) / (x1 + md);
	b5 = x1 + x2;
	return ((b5 + 8) >> 4);
}

long BMP185::getPressure() {
	temperature = getTemperature();
	
	switch(oss) {
		case 0:
			write8(BMP185_CTRL_MEAS, 0x34);
			break;
		case 1:
			write8(BMP185_CTRL_MEAS, 0x74);
			delayMicroseconds(3000);
			break;
		case 2:
			write8(BMP185_CTRL_MEAS, 0xB4);
			delayMicroseconds(9000);
			break;
		case 3:
			write8(BMP185_CTRL_MEAS, 0xF4);
			delayMicroseconds(21000);
			break;
	}
	byte _sco = 0x20;
	do	{
		delayMicroseconds(4500);
		_sco = getStartOfConversion();
	} while(_sco > 0);		
	long up = (long)read32(0xF6) >> (8 - oss);
	long b6 = b5 - 4000;
	long x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	long x2 = ac2 * b6 >> 11;
	long x3 = x1 + x2;
	long b3 = (((ac1 * 4 + x3) << oss) + 2) >> 2;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	unsigned long b4 = ac4 * (unsigned long)(x3 + 32768) >> 15;
	unsigned long b7 = ((unsigned long)up - b3) * (50000 >> oss);
	long p;
	if(b7 < 0x80000000) {
		p = (b7 << 1) / b4;
	} else {
		p = (b7 / b4) << 1;
	}
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	return p + ((x1 + x2 + 3791) >> 4);
}

void BMP185::setOversamplig(byte _oss) {
	oss = _oss;
}

void BMP185::readCalibrationData() {
	ac1 = (short)read16(0xAA);
	ac2 = (short)read16(0xAC);
	ac3 = (short)read16(0xAE);
	ac4 = (word)read16(0xB0);
	ac5 = (word)read16(0xB2);
	ac6 = (word)read16(0xB4);
	b1 = (short)read16(0xB6);
	b2 = (short)read16(0xB8);
	mb = (short)read16(0xBA);
	mc = (short)read16(0xBC);
	md = (short)read16(0xBE);
}

byte BMP185::getStartOfConversion() {
	return read8(BMP185_CTRL_MEAS) & 0x20;
}

void BMP185::write8(byte _addr, byte _val) {
	Wire.beginTransmission(BMP185_ADDR);   // start transmission to device 
	Wire.write(_addr); // send register address
	Wire.write(_val); // send value to write  
	Wire.endTransmission(); // end transmission
}

byte BMP185::read8(byte _addr) {
	byte *_buff = read(_addr, 1);
	byte _result = _buff[0];
	free(_buff);
	return _result;
}

word BMP185::read16(byte _addr) {
	byte *_buff = read(_addr, 2);
	word _result = ((word)_buff[0] << 8) | (word)_buff[1];
	free(_buff);
	return _result;
}

unsigned long BMP185::read32(byte _addr) {
	byte *_buff = read(_addr, 3);
	unsigned long _result = ((unsigned long)_buff[0] << 16) | ((unsigned long)_buff[1] << 8) | (unsigned long)_buff[2];
	free(_buff);
	return _result;
}

byte *BMP185::read(byte _addr, byte n) {
	byte *_buff = (byte *)malloc(sizeof(byte) * n);
	Wire.beginTransmission(BMP185_ADDR); // start transmission to device 
	Wire.write(_addr); // sends register address to read from
	Wire.endTransmission(); // end transmission
	  
	Wire.beginTransmission(BMP185_ADDR); // start transmission to device 
	Wire.requestFrom(BMP185_ADDR, n);// send data n-bytes read
	byte i = 0;
	while (Wire.available()) {
		_buff[i] = Wire.read(); // receive DATA
		i += 1;
	}
	Wire.endTransmission(); // end transmission
	return _buff;
}
