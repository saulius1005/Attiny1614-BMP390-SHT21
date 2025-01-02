/*
 * Attiny1614 BMP390 SHT21.c
 *
 * Created: 2024-03-08 19:45:58
 * Author : Saulius
 */ 
#include "Settings.h"
int main(void){
 GPIO_init();
 I2C_init();
 SHT21_Settings_Read(); //Reading current settings
 SHT21.Resolution = RH_11b_T_11b; //Changing RH and T measurement resolution
 SHT21.Heater = ON; //heater on adds ~ 0.5-1.5C according to datasheet
 SHT21.OTP_DISABLE = ON; //keep changed settings (if ON restore factory settings (not recommended to use according to datasheet))
 SHT21.Battery = ON; //2.25V detection is off
 SHT21_Settings_Write(); //Write changed settings
 while (1){
  Separator(SHT21_Read(NO_HOLD_MASTER_RH_MES), IGNORE_CRC);
  Separator(SHT21_Read(HOLD_MASTER_T_MES), USE_CRC);
 // SHT21_Reset(ON); //Restore factory default settings and reset SHT21 (use this instead of OTP_DISABLE)
 }
}
