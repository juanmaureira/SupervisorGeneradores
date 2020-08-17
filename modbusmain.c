#include <18f6722.h>
//#device adc=8
#use delay(clock=8000000)              //Used to set the reserved FUSE bits

#define   MODEM_SERIAL_BAUD      9600

#use rs232(baud=MODEM_SERIAL_BAUD, xmit=PIN_C6,rcv=PIN_C7, parity=N, stream=MODEM)
#use rs232(baud=2400,xmit=PIN_B1,rcv=PIN_B0,parity=n,stream=COMV)

#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT                    //Internal RC Osc
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOMCLR
   
//#define MODBUS_SLAVE_ADDRESS             0x01
#define   SEGREADPANEL                  10
#define NUMREG                        17
#define   DCD0                        PIN_C0
#define   RTS0                        PIN_C1
#define   DSR0                        PIN_C2
#define CTS0                          PIN_C3
#define   DTR0                        PIN_C4
#define RI0                           PIN_C5   


// Tablas con Valores para Paneles
//  Parametro                      EasyGen        Powerwizard 2.0    Panel6000
//0 Carga Kw                       50259          105                10
//1 Potencia Real Generada         50121-50122    106-107             0
//2 Potencia Aparente Generada     0              138-139             0
//3 Carga VA                       0              140                 0
//4 Potencia Reactiva Generada     50124-50125    141-142            16
//5 Percent Carga VA               50580-50059    143                 0
//6 Potencia KWh                   0              144-145             5
//7 Presion Aceite                 50254          217                 2
//8 Temp. Motor                    50248          219                 3
//9 Tension Bateria                50021          202                 1
//10 RPM                           50246-50247    0                   6
//11 Horas Operacion               50127          204                 4
//12 Dias para Mant                50126          210                 0
//13 Semanas para mant             0              299                 0
//14 Modo Operacion                50120          1053               72
//15 Nivel Comb.                   0              0                   0
//16 Estdo Equipo Yelcho           0              0                   0
//17 Alarmas                       0              0                   0
//18 Direccion Equipoc             00001          1                   0

            //       0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18    19    21
//Mapas de registros y su longitud en respectivos paneles (Orden es correspondencia de lectura)
int16 mapa_pw2[NUMREG] = {105, 106, 138, 140, 141, 143, 144, 200, 201, 202, 203, 204, 212, 299, 301, 805, 1053};
int16 long_pw2[NUMREG] = { 1,   2,   2,   1,   2,   1,   2,   1,   1,   1,   1,   2,   1,   1,   1,   1,    1};

int16 mapa_eg[NUMREG] = {50259, 50121, 0, 0, 50124, 50580, 0, 50254, 50248, 50021, 50246, 50127, 50126, 0, 50120, 0, 0};
int16 long_eg[NUMREG] = {1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1};

int16 mapa_p6[NUMREG] = {10, 0, 0, 0, 16, 0, 5, 2, 3, 1, 6, 4, 0, 0, 72, 0, 0};
int16 long_p6[NUMREG] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

//int16 mapa_pw1[1] = {1}; //Panel no tiene comunicaciones, solo entradas discretas
//int16 long_pw1[1] = {1};

int16 BufferData[27];
//int16 ContBufferData=0;
//int mapa_rg[1] = {1}; //Panel no se ha revisado
//int long_rg[1] = {1};

//Funciones utilizadas
void start_init(void);
void write_holding_reg(void);
void read_holding_reg(void);
void processIO(void);

//int16 contador=0;
int cont;
int mode=25;
int tenmillis;
long int seg=0;
long int timealarma;

#include "modbus.c"
#include "MenuInt_ext.c"
#include "ADH8066.c"
#include "AlarmaPanel.c"
/**************************************************************************
         ECUACION Y CALCULO PARA TIEMPO DE TIMER1
ECUACION
   T=Tcm*Preescaler*(65536-CargaTMR1)
Donde:
   Tcm es el ciclo de máquina= 4/Fosc;
   T=Tiempo de desborde del TMR1
   
Para un tiempo de desborde de T=100ms y Preescaler de 1:8

   CargaTMR1 = 65536 - T/(Tcm*Preescaler)

   Con CargaTMR1 = 0
   -> T=104,8ms

   CargaTMR1 = 65536 - 0,1/((4/20M)*8) = 3036   T=100ms
**************************************************************************/
#int_TIMER4
void timer4_isr(void){
	output_toggle(PIN_B2);
	tenmillis++;
	if(tenmillis==100){
		seg++;
		output_toggle(PIN_B3);
		mode=3;
		if(seg==timerptval){
			seg=0;
			mode=4;
		}	
		tenmillis=0;	
	}
}

void InitEeprom(void){
int status;
int i;
char c;

delay_ms(250);
disable_interrupts(int_ext);
timeoutmenu=0;

fprintf(COMV,"\r\nLeyendo datos de eeprom");

   for(i=0;i<31;i++){
      fputc('.',COMV);
      apnGPRS[i]=0x00;
      userGPRS[i]=0x00;
      passGPRS[i]=0x00;
      ipGPRS[i]=0x00;
      portGPRS[i]=0x00;
      delay_ms(125);
   }  

fprintf(COMV,"\r\n"); 

contrpt=read_eeprom(30);
delay_ms(5);
   if(contrpt>0&&contrpt<=30){
      status=0b00000000;
      ReadArray(31,apnGPRS,contrpt);
      delay_ms(5);
   }
   else
      status=0b00000001;

contrpt=read_eeprom(80);
delay_ms(5);

   if(contrpt>0&&contrpt<=30){
      status|=0b00000000;
      ReadArray(81,userGPRS,contrpt);
      delay_ms(5);
   }
   else
      status|=0b00000010;

contrpt=read_eeprom(120);
delay_ms(5);

   if(contrpt>0&&contrpt<=30){
      status|=0b00000000;
      ReadArray(121,passGPRS,contrpt);
      delay_ms(5);
   }
   else
      status|=0b00000100;

contrpt=read_eeprom(160);
delay_ms(5);

   if(contrpt>0&&contrpt<=30){
      status|=0b00000000;
      ReadArray(161,ipGPRS,contrpt);
      delay_ms(5);
   }
   else
      status|=0b00001000;

contrpt=read_eeprom(210);
delay_ms(5);

   if(contrpt>0&&contrpt<=30){
      status|=0b00000000;
      ReadArray(211,portGPRS,contrpt);
      delay_ms(5);
   }
   else
      status|=0b00010000;

panel=read_eeprom(0);
delay_ms(5);

   if(panel=='1'||panel=='2'||panel=='3')
      status|=0b00000000;
   else
      status|=0b00100000;

modetransmicion=read_eeprom(20);       
delay_ms(5);

   if(modetransmicion=='1'||modetransmicion=='2')
      status|=0b00000000;
   else
      status|=0b01000000;

timerptval=ReadWord(10);
delay_ms(5);
esclavo=read_eeprom(500);
delay_ms(5);

   if(timerptval>0&&timerptval<65536&&esclavo>=0&&esclavo<=99)
      status|=0b00000000;
   else
      status|=0b10000000;

   if(status==0b00000000){

      fprintf(COMV,"\r\nPanel=%c\r\n",panel);
      fprintf(COMV,"\r\nTiempo de reporte= %lu\r\n",timerptval); 
      fprintf(COMV,"\r\nModo de Transmision = %c\r\n",modetransmicion);         
      fprintf(COMV,"\r\nAPN: %s\r\n",apnGPRS);
      fprintf(COMV,"\r\nUSER: %s\r\n",userGPRS);
      fprintf(COMV,"\r\nPASWORD: %s\r\n",passGPRS);
      fprintf(COMV,"\r\nIP: %s\r\n",ipGPRS);
      fprintf(COMV,"\r\nPort: %s\r\n",portGPRS);
      fprintf(COMV,"\r\nID esclavo: %i\r\n",esclavo);
	
	   timerptval=0.6*timerptval-240;	

	   timealarma=timerptval;
	
      fprintf(COMV,"\r\nDesea iniciar reporte con los parametros guardados [S/N] ");
	  enable_interrupts(int_timer0);
			
         do{

            c=getc(COMV);

            switch(c){
               
               case 'S': case 's':
                   fprintf(COMV,"\r\nIniciando Reporte\r\n");
                   disable_interrupts(int_ext);
                   mode=2;
                break;
               
               case 'N': case 'n':
                  fprintf(COMV,"\r\nPresione cualquier tecla para configure nuevos parametros\r\n");
                  enable_interrupts(int_ext);
                  mode=0;
               break;
               
               default: 
                  fprintf(COMV,"\r\nOpcion no valida\r\n");
               break;
            }

         }while(c!='N'&&c!='n'&&c!='S'&&c!='s'&&timeoutmenu<5000);

		disable_interrupts(int_timer0);

			if(timeoutmenu>=5000){
            	enable_interrupts(int_ext);
                mode=0;
			}

		timeoutmodem=0;
	 	timeoutmenu=0;
   }
   else{
      fprintf(COMV,"\r\nFaltan parametros para iniciar equipo\r\n");
      enable_interrupts(int_ext);
   }
}

//Ciclo principal
void main(void){
boolean status;
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);   
   set_timer1(3036);
   setup_comparator(NC_NC_NC_NC);
   start_init();
   clear_interrupt(INT_TIMER1);
      while(true){
         switch(mode){
            case 1: 
				MenuUser(); 
				break;
            case 2: 
					InitModem();
					enable_interrupts(int_timer4); 
				break;
            case 3: read_holding_reg(); break;
			case 4: 
				  disable_interrupts(int_timer4);
                  status=InitComunication();
                  if(status){
                     write_holding_reg();
					 mode=5;
				  }
                  else{
                     fprintf(COMV,"ERROR DE CONEXION");
					 mode=5;	
				  }
				  clear_interrupt(int_timer4);
				  enable_interrupts(int_timer4);	
            break;

			case 5: 
					disable_interrupts(int_timer4);
					CloseComunication();
					clear_interrupt(int_timer4);
					enable_interrupts(int_timer4);
					mode=0; 
					break;
            default: 
					break;
         }
      }
}

void start_init(void){
	setup_oscillator(OSC_8MHZ);
	enable_interrupts(global);
	setup_timer_4(T4_DIV_BY_16,249,5);
	modbus_init();
	InitEeprom();
}

void putc_server(int8 c){
	fprintf(MODEM,"%u ",c);
	modbus_calc_crc(c);
	delay_ms(1);
}

void write_crc_modbus(void){
int8 crc_low, crc_high;
	crc_high=modbus_serial_crc.b[1];
	crc_low=modbus_serial_crc.b[0];
	putc_server(crc_high);
	putc_server(crc_low);
}

void write_holding_reg(void){
int i;
int station=1, reg_multiple=0x10;
int cantreg=54;	
// IP red global: 200.63.96.254
// Puerto de enlace: 502
// Carlos Walter: (02)6358832

	modbus_serial_crc.d=0xFFFF;

	fprintf(MODEM,"%u %u %u ",station,reg_multiple,cantreg);

	for(i=0;i<27;i++){
		putc_server(make8(BufferData[i],1));
		delay_ms(1);
		putc_server(make8(BufferData[i],0));	
		delay_ms(1);
	}
	 
	for(i=0;i<4;i++){
		putc_server(NAAlarma[i]);
	}

	for(i=0;i<4;i++){
		putc_server(CondItemAlarma[i]);
	}

	write_crc_modbus();

	 fprintf(COMV,"Datos enviados OK");	
	 mode=5;
}

void read_holding_reg(void){
int8 a,b,x,y;
int16 z,c;
byte i,k;
int StablishConection;
int statusalarmas;
int16 len;
disable_interrupts(int_timer4);
k=0;
	for(i=0;i<27;i++)
		BufferData[i]=0x0000;

   fprintf(COMV,"\r\nLeyendo registros de panel");
   delay_ms(250);

   for(i=0;i<NUMREG;i++){
			if(panel=='2'){
            	StablishConection=modbus_read_holding_registers(esclavo,(mapa_pw2[i]-1),long_pw2[i]);
				len=long_pw2[i];
			}
			if(panel=='1'){
				StablishConection=modbus_read_holding_registers(esclavo,(mapa_eg[i]-1),long_eg[i]);
				len=long_eg[i];
			}
			if(panel=='3'){
				StablishConection=modbus_read_holding_registers(esclavo,(mapa_p6[i]-1),long_p6[i]);
				len=long_p6[i];
			}
            if(StablishConection==0){
				
				if(len<2){	
               		x=modbus_rx.data[1];
               		y=modbus_rx.data[2];
               		z=make16(x,y);
					BufferData[k]=z;
					k++;
					delay_ms(1);
				}
				else{
               		x=modbus_rx.data[1];
               		y=modbus_rx.data[2];
               		z=make16(x,y);
					BufferData[k]=z;
					k++;
					delay_ms(1);
					a=modbus_rx.data[3];
					b=modbus_rx.data[4];
               		c=make16(a,b);
					BufferData[k]=c;
					k++;	
					delay_ms(1);
				}
 
			fputc('.',COMV);

				statusalarmas=ChekAlarma();
				
				switch(statusalarmas){
					case 0:
								 
					break;

					case 1:
							
					break;

					case 2:
							timerptval=timealarma;
					break;

					case 3:
							timerptval>>=1;
							if(timerptval<=0) timerptval=0;
					break;

					default: break;
				}
            } 
            else{
                  switch(modbus_rx.error){
					case 1: 
						fprintf(COMV,"\r\nILLEGAL_FUNCTION\r\n");
					break;
					case 2:
						fprintf(COMV,"\r\nILLEGAL_DATA_ADDRESS\r\n");
					break;
					case 3:
						fprintf(COMV,"\r\nILLEGAL_DATA_VALUE\r\n");
					break;
					case 4:
						fprintf(COMV,"\r\nSLAVE_DEVICE_FAILUREr\n");
					break; 	
					case 5:
						fprintf(COMV,"\r\nACKNOWLEDGE\r\n");
					break;
					case 6:
						fprintf(COMV,"\r\nSLAVE_DEVICE_BUSY\r\n");
					break;
					case 8:
						fprintf(COMV,"\r\nMEMORY_PARITY_ERROR\r\n");
					break;
					case 10:
						fprintf(COMV,"\r\nGATEWAY_PATH_UNAVAILABLE\r\n");
					break;
					case 11:
						fprintf(COMV,"\r\nGATEWAY_TARGET_NO_RESPONSE\r\n");
					break;
					case 12:
						fprintf(COMV,"\r\nTIMEOUT\r\n");
					break;
				  }
            }
   }	
	mode=0;
   enable_interrupts(int_timer4);
}