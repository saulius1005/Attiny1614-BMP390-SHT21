/*
 * I2C.c
 *
 * Created: 2024-03-08 19:49:05
 *  Author: Saulius
 */ 
#include "Settings.h"

void I2C_init(){
	TWI0.CTRLA = TWI_SDAHOLD_OFF_gc
	| TWI_SDASETUP_4CYC_gc;
	TWI0.MBAUD = (uint8_t)TWI0_BAUD(300000, 0.6); //Baud ~400k
	TWI0.MCTRLA = TWI_ENABLE_bm //Enable TWI
	| TWI_FMPEN_bm
	| TWI_SMEN_bm
	| TWI_QCEN_bm; //Quick command enable
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; //Turn bus to idle
}

uint8_t TranssmitAdd(uint8_t Add, uint8_t Read){
 for (uint8_t i = 0; i< 3; i++){
  TWI0.MADDR = ((Add<<1)+Read); //7-1 bites is add and 0 is R/W condition (Add<<1+0) Exp.: 6<<1 = 12+0 = 12 (0xC for MT6701)
  while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));
  if(!(TWI0.MSTATUS & TWI_RXACK_bm)) //if receive ACK
   return 0;
 }
 I2C.Fault = 1; //NACK error
 TWI0.MCTRLB = TWI_MCMD_STOP_gc; // sending stop
 return 1; //if after 3 times attempt to sending Add get NACK
}

uint8_t TranssmitByte(uint8_t Data, uint8_t stop){
 if(!(I2C.Fault)){// only if we get ACK
  TWI0.MDATA = Data; //Sending our data
  while (!(TWI0.MSTATUS & TWI_WIF_bm)); //Waiting until Data sending will be done
  if(!(TWI0.MSTATUS & TWI_RXACK_bm)){ //if receive ACK
   if(stop & 1){// if no more transmiting we should send stop
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    return 0;
   }
   else
    return 0;
  }
 }
 I2C.Fault = 2;
 TWI0.MCTRLB = TWI_MCMD_STOP_gc; // sending stop
 return 2;
}

uint8_t ReadByteACK(){ //1-ACK, 2-NACK
	while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));
	TWI0.MCTRLB = TWI_ACKACT_ACK_gc;
	return TWI0.MDATA;
}

uint8_t ReadByteNACK(){ //1-ACK, 2-NACK
	while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));
	TWI0.MCTRLB = TWI_ACKACT_NACK_gc;
	return TWI0.MDATA;
}

uint8_t Stop(){
while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));
 TWI0.MCTRLB |= TWI_MCMD_STOP_gc; // sending stop to sda
 if(TWI0.MSTATUS & TWI_RXACK_bm) //if receive nack from slave
  return 1;// exit with error 
 return 0; // or if all ok send 0
}

void WriteToReg(uint8_t Add, uint8_t Reg, uint8_t Data){ //single byte write
	TranssmitAdd(Add, W);// write Add and W
	TranssmitByte(Reg, NO_STOP); // sending reg
	TranssmitByte(Data, STOP); // sending data with stop
}

void WriteMulti(uint8_t Add, uint64_t data, uint8_t bytes){ //writing up to 8 Bytes (first 8Bytes act as reg data if needed). Add- i2c slave address, data- data to be send including register if neede, byte- totally sending bytes count (exmpl. if need send 0 value)
	uint8_t dataSplit[8]= {0,0,0,0,0,0,0,0}; //for data spliting
	for(uint8_t i=0; i<bytes; i++){
		dataSplit[i] = data >> (8*((bytes-1)-i));
		data = data - (dataSplit[i] << (8*((bytes-1)-i)));
	}
	TranssmitAdd(Add, W);
	for(uint8_t i = 0; i<bytes; i++){
		TranssmitByte(dataSplit[i], ((bytes-1) == i)); //adding stop only on last Byte
	}
}

uint8_t ReadReg(uint8_t Add, uint8_t Reg){ //single byte read
 uint8_t data = 0;
 for (uint8_t i = 0; i< 3; i++){
  if(TranssmitAdd(Add, W))
   break;
	;// write Add and W
  TranssmitByte(Reg, STOP); // sending reg only with stop
  if(TranssmitAdd(Add, R)) //write Add and R
   break;
	data = ReadByteNACK(); // returning data
	if(Stop() & 1) // if we get ACK
	 I2C.Fault = 2; // if after 3 times still no answer (data reading fail- NACK)
	else{ //othervise repeating command 3 times untill we get ack
	 I2C.Fault = 0; // if answer reset fault message
	 return data;
	}
 }
	return 0;
}

uint64_t ReadMulti(uint8_t Add, uint8_t FReg, uint8_t bytes){
	uint64_t data = 0;
	if(bytes < 9 && bytes !=0 ){ //1- 8b, 2-16b, 3-24b, 4-32b, 5-40b, 6-48b, 7- 56b, 8- 64b
		TranssmitAdd(Add, W);// write Add and W
		TranssmitByte(FReg, STOP); // sending reg only with stop
		TranssmitAdd(Add,R); //write Add and R
		for (uint8_t i= 0; i<bytes; i++){
			if(i == (bytes- 1)){
				data += ReadByteNACK();
				Stop();
			}
			else{
				data += ReadByteACK();
				data <<= 8 ;
			}
		}
	}
	return data;
}