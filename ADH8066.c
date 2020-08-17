/*
         Driver para manejar ADH8066 via GPRS/TCP
*/
#include<string.h>
#define   ON_RST               PIN_F0               // PIN PARA ENCENDER MODEM

#define   MODEM_SERIAL_TIMEOUT   10                  //en ms
#define MODEM_RCV_OFF() {disable_interrupts(INT_RDA);}

//#use rs232(baud=MODEM_SERIAL_BAUD, UART1, parity=N, stream=MODEM)

char rec_1[3] = {"OK"};
char rec_2[6] = {"ERROR"};
char rec_3[8] = {"CONNECT"};
char rec_4[11] = {"NO CARRIER"};
char rec_5[10] = {"IP STATUS"};
char rec_6[11] = {"IP INITIAL"};
char rec_7[8] = {"SHUT OK"};
char rec_8[7] = {"CLOSED"};
char rec_9[6] = {"DEACT"};
char rec_10[6] = {"READY"};
//char rec_11[2] = {"."};
char rec_12[11] = {"POWER DOWN"};
char rec_13[8] = {"+AIPA"};
char rec_14[10] = {"+AIPDCONT"};
char rec_15[6] = {"+AIPO"};
boolean UART2=false;
char ModemBuffer[60];
int modem_in, modem_out;
long int timeoutmodem,timeoutmenu;
//boolean flagmodem=false;
//char userIP[30];
char c;

#int_timer0
void isr_timer0(void){
      timeoutmodem++;
	  timeoutmenu++;	
}

#int_rda
void isr_rda(void){
   UART2=true;
   clear_interrupt(INT_RDA2);
}

void TimeoutOn(void){
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32|RTCC_8_bit);
   enable_interrupts(int_timer0);
   timeoutmodem=0;
}

void TimeoutOff(void){
   disable_interrupts(int_timer0);
   timeoutmodem=0;
}

void ClearBuffer(void){
int i;
   for(i=0;i<60;i++)
      ModemBuffer[i]=0x00;
   modem_in=0;
   modem_out=0;      
}

void MODEM_RCV_ON(void){
   ClearBuffer();
   clear_interrupt(INT_RDA);
   enable_interrupts(int_rda);
}

int ProccessModem(long int timeout){
int respuesta=0;
   timeoutmodem=0;
   TimeoutOn();
      while(timeoutmodem<=timeout){
         if(kbhit(MODEM)){   
         c=getc(MODEM);
            if((c==0x0A||c==0x0D)&&modem_in<60){
               ModemBuffer[modem_in++]=0x20;
            }
            else{
               if(modem_in<60){
                  ModemBuffer[modem_in++]=c;
               }
            }
         }   
      }
   	timeoutmodem=0;   
   	respuesta = 0;
	TimeoutOff();
    if (strstr(ModemBuffer, rec_1)) respuesta = 1; //Ok
    if (strstr(ModemBuffer, rec_2)) respuesta = 2; //Error
    if (strstr(ModemBuffer, rec_3)) respuesta = 3; //Connect
    if (strstr(ModemBuffer, rec_4)) respuesta = 4; //No carrier
    if (strstr(ModemBuffer, rec_5)) respuesta = 5; //IP status
    if (strstr(ModemBuffer, rec_6)) respuesta = 6; //IP initial
    if (strstr(ModemBuffer, rec_7)) respuesta = 7; //Shut ok
    if (strstr(ModemBuffer, rec_8)) respuesta = 8; //Closed
    if (strstr(ModemBuffer, rec_9)) respuesta = 9; //Deact
    if (strstr(ModemBuffer, rec_10)) respuesta = 10; //Ready
 //   if (strstr(ModemBuffer, rec_11)) respuesta = 11; //Encontro IP
    if (strstr(ModemBuffer, rec_12)) respuesta = 12; //Respuesta MODEM APAGADO
 	if (strstr(ModemBuffer, rec_13)) respuesta = 13; //Respuesta AIPA=1, atach red
 	if (strstr(ModemBuffer, rec_14)) respuesta = 14; //Respuesta +AIPDCONT que encontro la direccion IP
	if (strstr(ModemBuffer, rec_15)) respuesta = 15; //Respuesta +AIPO Se pudo conectar al server (IP,PORT), listo para enviar datos

   fprintf(COMV,"\r\nRecibido=%s \r\n",ModemBuffer);
   delay_ms(50);	
   ClearBuffer();
   fprintf(COMV,"\r\nresp=%i\r\n",respuesta);
   delay_ms(50);
   modem_in=0;
   timeoutmodem=0;   
return respuesta;
}

void InitModem(void){
int i,status;
boolean flagerror=false;
   fprintf(COMV,"\r\nInicializando Modem....");
   delay_ms(2000);   
   output_high(DSR0);
   output_high(CTS0);
   output_high(ON_RST);
   delay_ms(250);
   output_low(ON_RST);   
   delay_ms(2500);
   output_high(ON_RST);
   delay_ms(250);
   output_low(DSR0);
   output_low(CTS0);   
   status=ProccessModem(1500);	
   fprintf(MODEM,"AT\r");
   status=ProccessModem(1500);   
      if(status==1){
         fprintf(COMV,"\r\n Modem Inicializado");
      }
      else{
         fprintf(COMV,"\r\nError en Inicializacion\r\n");
         delay_ms(1000);
         fprintf(COMV,"\r\nReiniciando...\r\n");
         fprintf(COMV,"Intento  ");
         for(i=1;i<4;i++){
            fprintf(COMV,"\b%d",i);
            fprintf(MODEM,"AT\r");
            status=ProccessModem(1000);
               if(status)
                  break;
            if(i==3) flagerror=true;                   
         }
         if(flagerror){
            fprintf(COMV,"\r\nModem no detectado\r\n");
         }   
         else{
            fprintf(COMV,"\r\nModem encendido\r\n");
            fprintf(MODEM,"AT\r");
            status=ProccessModem(1000);
            if(status){
               fprintf(COMV,"\r\nModem = OK\r\n");
               fprintf(MODEM,"ATE0\r");
               status=ProccessModem(2500);
               if(status)
                  fprintf(COMV,"\r\nModem = OK\r\n");
               else{
                  fprintf(COMV,"\r\nFalla Modem\r\n");
               }
            }
            else{
               fprintf(COMV,"\r\nFalla Modem\r\n");
            }
         }
      }
   mode=0;   
   MODEM_RCV_OFF();
}

boolean InitComunication(void){
int status;
int conterror,cont,cont1;
boolean conexion=false;  
	fprintf(COMV,"\r\nConectando a: %s\r\n",apnGPRS);
	delay_ms(500);		
	fprintf(COMV,"\r\nEspere\r\n"); 
	delay_ms(500);
	for(conterror=0;conterror<4;conterror++){
   		fprintf(MODEM,"AT+AIPDCONT=\"%s\",\"%s\",\"%s\"\r",apnGPRS,userGPRS,passGPRS);
   		status=ProccessModem(1000);
			if(status==14){
				conterror=5;
				fprintf(COMV,"\r\nOK\r\n");
				delay_ms(500);
					fprintf(COMV,"\r\nObteniendo IP\r\n");
					for(cont=0;cont<4;cont++){
    		        	fprintf(MODEM,"AT+AIPA=1\r");
            			status=ProccessModem(1000);
							if(status==13){
								cont=5;
								fprintf(COMV,"\r\nOK\r\n");
								delay_ms(500);
									fprintf(COMV,"Conectandose a IP %s Puerto %s\r\n",ipGPRS,portGPRS);
									delay_ms(250);
									fprintf(COMV,"\r\nEspere\r\n");
									delay_ms(250);
										for(cont1=0;cont1<4;cont1++){
											fprintf(MODEM,"AT+AIPO=1,,\"%s\",%s,0,,0,2\r",ipGPRS,portGPRS);
											fprintf(COMV,"\r\nAT+AIPO=1,,\"%s\",%s,0,,0,2\r\r\n",ipGPRS,portGPRS);
											status=ProccessModem(3500);
												if(status==15){
													fprintf(COMV,"\r\nOK\r\n");
													delay_ms(500);
													fprintf(COMV,"\r\nConexion establecida\r\n");
													conexion=true;	
													cont1=5;
												}						
										}
						
							}
					} 				
			} 	
	}

return conexion;
}

void CloseComunication(void){
	int status=0;
   status=ProccessModem(5000);
   fprintf(MODEM,"+++");
   status=ProccessModem(5000);
   fprintf(MODEM,"+++");
   status=ProccessModem(5000);  
   fprintf(MODEM,"AT+AIPC=1\r");
   status=ProccessModem(2000);
   fprintf(MODEM,"AT+AIPA=0\r");
   status=ProccessModem(2000);
   mode=0;
	fprintf(COMV,"\r\nSocket cerrado\r\n");	
}