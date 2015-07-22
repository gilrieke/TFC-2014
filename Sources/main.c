/* ###################################################################
 **     Filename    : main.c
**     Project     : prueba_KL25Z
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-07-08, 13:59, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Led_Blue.h"
#include "Led_Green.h"
#include "Led_Red.h"
#include "TU1.h"
#include "Servo.h"
#include "PwmLdd1.h"
#include "WAIT1.h"
#include "Motor_Right_R.h"
#include "Motor_Left_R.h"
#include "SW1.h"
#include "SW2.h"
#include "SW3.h"
#include "Term1.h"
#include "Inhr1.h"
#include "ASerialLdd2.h"
#include "Led_Battery.h"
#include "Motor_Left.h"
#include "PwmLdd2.h"
#include "TU2.h"
#include "Motor_Enable.h"
#include "BitIoLdd1.h"
#include "Motor_Right.h"
#include "PwmLdd3.h"
#include "Camera_Analog.h"
#include "AdcLdd1.h"
#include "Camera_Clock.h"
#include "BitIoLdd3.h"
#include "Camera_SI.h"
#include "BitIoLdd4.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#define SERVO_LEFT 62000
#define SERVO_HALF 61100
#define SERVO_RIGHT 60200

#define PIX_MIN 5
#define PIX_MAX 122
#define WHITE_MAX 175

uint8_t Camera_Values[128];
byte Camera_Threshold;
uint32_t Camera_Delay;
uint8_t posi=0,posf=0,c=0;
uint8_t i=0,h=0,pi=0;
uint8_t posli=0,poslf=0, posm=0;


/* User includes (#include below this line is not maintained by Processor Expert) */
void delay(int time){
	int i;
	for(i=0;i<time;i++){
		
	}
	return;
}

void Motor_Left(word time){
	Motor_Left_SetDutyUS(100-time);
	return;
}

void Motor_Right(word time){
	Motor_Right_SetDutyUS(100-time);
	return;
}

void Servo_Duty(word duty){
	
	return;
}

void Read_Camera(void){
	int i;
	Camera_SI_SetVal();
	Camera_Clock_SetVal();
	Camera_Analog_Measure(TRUE);	
	Camera_SI_ClrVal();
	for(i=0;i<128;i++){
		Camera_Clock_ClrVal();
		Camera_Analog_GetValue8(&Camera_Values[i]);
		delay(60);
		Camera_Clock_SetVal();
		Camera_Analog_Measure(TRUE);
	}
	Camera_Clock_ClrVal();
	return;
}

void APP_thresholding(void){
	
	uint16_t dato_mayor,dato_menor,umbral_viejo,contador_g1,contador_g2;
	uint8_t  i,flag_umbral;
	uint16_t grupo1,grupo2;
	
	dato_mayor = Camera_Values[PIX_MIN];
	dato_menor = Camera_Values[PIX_MIN];
	
	for(i=PIX_MIN+1 ; i<=PIX_MAX ; i++){
		if(dato_mayor < Camera_Values[i] )
			dato_mayor = Camera_Values[i];
		if(dato_menor > Camera_Values[i])
			dato_menor = Camera_Values[i];
	}
		
	umbral_viejo = (dato_mayor + dato_menor)/2;
	
	flag_umbral = 0;
	do{		
		contador_g1 = 1;
		contador_g2 = 1;
		grupo1 = 0;
		grupo2 = 0;
		
		for(i=PIX_MIN ; i<=PIX_MAX ; i++){
			
			if(Camera_Values[i] > umbral_viejo){
				grupo1 = grupo1 + Camera_Values[i];
				contador_g1++;
			}
			else{
				grupo2 = grupo2 + Camera_Values[i];
				contador_g2++;
			}
		}
	
		grupo1 = grupo1 / contador_g1;
		grupo2 = grupo2 / contador_g2;
	
		Camera_Threshold = (grupo1 + grupo2)/2;
		
		if(umbral_viejo == Camera_Threshold)
			flag_umbral = 1;
		else
			umbral_viejo = Camera_Threshold;
		
	}while(flag_umbral != 1);
	
	return;
}


void APP_binarization(void){
	
	uint8_t i;
	
	for(i=PIX_MIN ; i<=PIX_MAX ; i++){
		if(Camera_Values[i] > Camera_Threshold)
			Camera_Values[i] = 1;
		else
			Camera_Values[i] = 0; 
	}
	delay(10);
	return;
}

void APP_segmentation(void){
	
	return;
}

int lineas(){
	posf=0,posi=0,i=0,c=0, poslf=0,posli=0;
	
Lectura:
	for(i=posf+1;i<128;i++){
		if(Camera_Values[i]==0){
			posi=i;
			break;
		}
	}
	for(i;i<128;i++){
		if(Camera_Values[i]!=Camera_Values[posi]){
			posf=i-1;
			//s======
			if(posi==posf){
				goto Lectura;
			}
			int ti=4,t=0;
			t=posf-posi;
			if(t<ti){
				goto Lectura;
			}
			else{
			//=============
				posli=posi;
				poslf=posf;
				c++;
				goto Lectura;
			}
		}
	}
	if(c==0 || c>1){
		posi=0;
		posf=0;
	}
	posm=((poslf+posli)/2);
}
//================================

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */

{
  /* Write your local variable definition here */
	word i,j;
	uint8_t init_flag;
	
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  /* Write your code here */
  
  Camera_Analog_Calibrate(TRUE);
  Motor_Enable_SetVal();
  Camera_Delay = 500;
  init_flag = 0;  
  for(;;) {
	  	
	  while(init_flag == 0){
		  Led_Battery_PutVal(Led_Battery_DeviceData, 1);
		  while(SW1_GetVal(SW1_DeviceData) == 0){}
		  while(SW1_GetVal(SW1_DeviceData)){}
		  delay(100);
		  
		  Led_Battery_PutVal(Led_Battery_DeviceData, 2);
		  Read_Camera();
		  do{
			  for(i=0; i<4 ; i++){
				  APP_thresholding();
				  APP_binarization();
				  for(j=PIX_MIN;j<PIX_MAX;j++){
					  Term1_SendNum(Camera_Values[i]);
				  }
				  Term1_CRLF();
				  Read_Camera();
				  delay(Camera_Delay);
			  }
			  if(Camera_Values[PIX_MIN] > WHITE_MAX && Camera_Values[PIX_MAX] > WHITE_MAX)
				  init_flag = 1;
			  
			  else
				  Camera_Delay += 1000;			  
		  }while(init_flag != 1);
		  Led_Battery_PutVal(Led_Battery_DeviceData, 3);
	  }
	  
	  while(SW1_GetVal(SW1_DeviceData) == 0){}
	  while(SW1_GetVal(SW1_DeviceData)){}
	  
	  while(init_flag == 1){
		  Read_Camera();
		  APP_thresholding();
		  APP_binarization();
		  lineas();
		  for(i=PIX_MIN;i<PIX_MAX;i++){
			  Term1_SendNum(Camera_Values[i]);
		  }
		  Term1_CRLF();
		  delay(Camera_Delay);
	  }
	  
	  /*Servo_SetRatio16(SERVO_LEFT);
	  WAIT1_Waitms(1000);
	  Servo_SetRatio16(SERVO_HALF);
	  WAIT1_Waitms(1000);
	  Servo_SetRatio16(SERVO_RIGHT);
	  WAIT1_Waitms(1000);*/
	  
	  /*for(i=0;i<100;i++){
		  Motor_Left(i);
		  Motor_Right(i);
		  WAIT1_Waitms(250);
	  }
	  
	  for(i=100;i>0;i--){
		  Motor_Left(i);
		  Motor_Right(i);
		  WAIT1_Waitms(250);
	  }

	  
	  for(i=0;i<100;i++){
		  Motor_Left_R_SetDutyUS(Motor_Left_R_DeviceData,100-i);
		  Motor_Right_R_SetDutyUS(Motor_Right_R_DeviceData,100-i);
		  WAIT1_Waitms(250);
	  }
	  
	  for(i=100;i>0;i--){
		  Motor_Left_R_SetDutyUS(Motor_Left_R_DeviceData,100-i);
		  Motor_Right_R_SetDutyUS(Motor_Right_R_DeviceData,100-i);
		  WAIT1_Waitms(250);
	  }*/
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
