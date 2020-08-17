/*
				Rutina para recivir alarmas de paneles
*/
/*
	ALARMAS POWER WIZARD
Alarmas y eventos (codificación SPN y FMI)				
				
Item alarma	Descripción			
0	Alerta de fallo interno (unidad Lauca)			
1	Alerta de falla en el cargador de batería			
2				
3	Alerta de baja tensión en sistema de carga			
4	Alarma de apagado por tensión elevada en batería			
5	Alerta de tensión elevada en batería			
6	Alerta de baja tensión en batería			
7	Alarma de apagado por alta frecuencia			
8	Alarma de apagado por baja frecuencia			
9	Alerta de frecuencia superior a set-point			
10	Alerta de frecuencia inferior a set-point			
11	Alarma de apagado por alto voltaje (generado)			
12	Alarma de apagado por bajo voltaje (generado)			
13	Alerta de alto voltaje (generado)			
14	Alerta de bajo voltaje (generado)			
15	Alarma de apagado por alta temperatura			
16	Alerta de alta temperatura en motor			
17	Alerta de baja temperatura en motor			
18	Alarma de apagado por baja presión de aceite			
19	Alerta de baja presión de aceite			
20	Alarma de apagado por alto nivel de combustible			
21	Alarma de apagado por bajo nivel de combustible			
22	Alerta de nivel alto de combustible			
23	Alerta de nivel bajo de combustible			
24	Alerta de intervalo para mantenimiento			
25	Alerta de switch parada de emergencia activada				

			
				
				

*/

#define		PRESENTE	0
#define		ACTIVA		1

int16 PosPW[20]={1500,1514,1528,1542\
				,1556,1570,1584,1598\
				,1612,1626,1640,1654\
				,1668,1682,1696,1710\
				,1724,1738,1752,1766};

	
union N_Alarmas{
	int32 NAlarma;
		struct{
			boolean	ALERTA_FALLO_LAUCA:1;							//	alarma 0
			boolean	ALERTA_FALLA_CARGADOR_BATERIA:1;				//	alarma 1
			boolean	NOTUSED:1;										//  alarma 2 = a alarma 1 
			boolean ALERTA_BAJA_TENSION_SISTEMA_CARGA:1;			//	alarma 3
			boolean	ALARMA_APAGADO_TENSION_ELEVADA_BATERIA:1;		//	alarma 4
			boolean ALERTA_TENSION_ELEVADA_BATERIA:1;				//	alarma 5
			boolean	ALERTA_BAJA_TENSION_BATERIA:1;					//	alarma 6
			boolean	ALARMA_APAGADO_ALTA_FRECUENCIA:1;				//	alarma 7	
			boolean	ALARMA_APAGADO_BAJA_FRECUENCIA:1;				//	alarma 8
			boolean	ALERTA_FRECUENCIA_SUPERIOR_SETPOINT:1;			//	alarma 9
			boolean	ALERTA_FRECUENCIA_INFERIOR_SETPOINT:1;			//	alarma 10
			boolean	ALARMA_APAGADO_ALTO_VOLTAGE_GENERADO:1;			//	alarma 11
			boolean	ALARMA_APAGADO_BAJO_VOLTAGE_GENERADO:1;			//	alarma 12
			boolean	ALERTA_ALTO_VOLTAGE:1;							//	alarma 13					
			boolean	ALERTA_BAJO_VOLTAGE:1;							//	alarma 14
			boolean	ALARMA_APAGADO_ALTA_TEMPERATURA:1;				//	alarma 15
			boolean	ALERTA_ALTA_TEMPERATURA_MOTOR:1;				//	alarma 16
			boolean	ALERTA_BAJA_TEMPERATURA_MOTOR:1;				//	alarma 17
			boolean	ALARMA_APAGADO_BAJA_PRESION_ACEITE:1;			//	alarma 18
			boolean	ALERTA_BAJA_PRESION_ACEITE:1;					//	alarma 19
			boolean	ALARMA_APAGADO_ALTO_NIVEL_COMBUSTIBLE:1;		//	alarma 20
			boolean	ALARMA_APAGADO_BAJO_NIVEL_COMBUSTIBLE:1;		//	alarma 21
			boolean	ALERTA_ALTO_NIVEL_COMBUSTIBLE:1;				//	alarma 22
			boolean	ALERTA_BAJO_NIVEL_COMBUSTIBLE:1;				//	alarma 23
			boolean	ALERTA_INTERVALO_MANTENIMIENTO:1;				//	alarma 24
			boolean	ALERTA_SWITCH_PARO_EMERGENCIA:1;				//	alarma 25
			boolean	NONE:6;											
		}bits;
}NameAlarma;

union Cond_Item_Alarma{
	int32 ItemAlarma;
		struct{
			boolean	alm1:1;
			boolean	alm2:1;
			boolean	alm3:1;
			boolean	alm4:1;
			boolean	alm5:1;
			boolean	alm6:1;
			boolean	alm7:1;
			boolean	alm8:1;
			boolean	alm9:1;
			boolean	alm10:1;
			boolean	alm11:1;
			boolean	alm12:1;
			boolean	alm13:1;
			boolean	alm14:1;
			boolean	alm15:1;
			boolean	alm16:1;
			boolean	alm17:1;
			boolean	alm18:1;
			boolean	alm19:1;
			boolean	alm20:1;
			boolean	alm21:1;
			boolean	alm22:1;
			boolean	alm23:1;
			boolean	alm24:1;
			boolean	alm25:1;
			boolean	NONE:7;
		}stat;
}CondAlarma;

int CondItemAlarma[4];
int NAAlarma[4];

struct{
	int16	FlagAndCont;
	int16	Indice;
	int32	IdAlarma; 
}Alarma;

/*
AnalizaAlarmas
Input:     None
Output:    2 No hay alarmas 
		   3 Hay alarmas 
*/
int AnalizaAlarmas(void){
int fmi;
int32 spn;

	fmi=Alarma.IdAlarma&0x0F;
	spn=Alarma.IdAlarma>>5;
	spn=spn&0x00FFFFFF;
	delay_cycles(3);

		switch(spn){
			case 167:
						if(fmi==11||fmi==31){// alarma 1 y alarma2
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FALLA_CARGADOR_BATERIA=false;
									CondAlarma.stat.alm1=PRESENTE;
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FALLA_CARGADOR_BATERIA=true;
									CondAlarma.stat.alm1=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FALLA_CARGADOR_BATERIA=true;
									CondAlarma.stat.alm1=PRESENTE;
								}
							}
							else{
								NameAlarma.bits.ALERTA_FALLA_CARGADOR_BATERIA=false;
								CondAlarma.stat.alm1=PRESENTE;
							}				
						}	

						if(fmi==17){// alarma 3
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_SISTEMA_CARGA=false;
									CondAlarma.stat.alm2=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_SISTEMA_CARGA=true;
									CondAlarma.stat.alm2=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_SISTEMA_CARGA=true;
									CondAlarma.stat.alm2=PRESENTE;
								}		
							}
							else{
								NameAlarma.bits.ALERTA_BAJA_TENSION_SISTEMA_CARGA=false;
								CondAlarma.stat.alm2=PRESENTE;
							}
						}		
			break;
			case 168:
						if(fmi==0){// alarma 4
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_TENSION_ELEVADA_BATERIA=false;
									CondAlarma.stat.alm4=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_TENSION_ELEVADA_BATERIA=true;
									CondAlarma.stat.alm4=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_TENSION_ELEVADA_BATERIA=true;
									CondAlarma.stat.alm4=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_TENSION_ELEVADA_BATERIA=false;
								CondAlarma.stat.alm4=PRESENTE;
							}
						}	
						if(fmi==15){// alarma 5
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_TENSION_ELEVADA_BATERIA=false;
									CondAlarma.stat.alm5=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_TENSION_ELEVADA_BATERIA=true;
									CondAlarma.stat.alm5=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_TENSION_ELEVADA_BATERIA=true;
									CondAlarma.stat.alm5=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_TENSION_ELEVADA_BATERIA=false;
								CondAlarma.stat.alm5=PRESENTE;
							}
						}	
						if(fmi==17){// alarma 6
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_BATERIA=false;
									CondAlarma.stat.alm6=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_BATERIA=true;
									CondAlarma.stat.alm6=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TENSION_BATERIA=true;
									CondAlarma.stat.alm6=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_BAJA_TENSION_BATERIA=false;
								CondAlarma.stat.alm6=PRESENTE;
							}

						}	
			break;
			case 2434:
						if(fmi==0){// alarma 7
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_FRECUENCIA=false;
									CondAlarma.stat.alm7=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_FRECUENCIA=true;
									CondAlarma.stat.alm7=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_FRECUENCIA=true;
									CondAlarma.stat.alm7=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_ALTA_FRECUENCIA=false;
								CondAlarma.stat.alm7=PRESENTE;
							}
						} 
						if(fmi==1){// alarma 8
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_FRECUENCIA=false;
									CondAlarma.stat.alm8=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_FRECUENCIA=true;
									CondAlarma.stat.alm8=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_FRECUENCIA=true;
									CondAlarma.stat.alm8=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_BAJA_FRECUENCIA=false;
								CondAlarma.stat.alm8=PRESENTE;
							}
						}	
						if(fmi==15){// alarma 9
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_SUPERIOR_SETPOINT=false;
									CondAlarma.stat.alm9=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_SUPERIOR_SETPOINT=true;
									CondAlarma.stat.alm9=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_SUPERIOR_SETPOINT=true;
									CondAlarma.stat.alm9=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_FRECUENCIA_SUPERIOR_SETPOINT=false;
								CondAlarma.stat.alm9=PRESENTE;
							}
						} 	
						if(fmi==17){// alarma 10
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_INFERIOR_SETPOINT=false;
									CondAlarma.stat.alm10=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_INFERIOR_SETPOINT=true;
									CondAlarma.stat.alm10=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_FRECUENCIA_INFERIOR_SETPOINT=true;
									CondAlarma.stat.alm10=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_FRECUENCIA_INFERIOR_SETPOINT=false;
								CondAlarma.stat.alm10=PRESENTE;
							}
						} 	
			break;
			case 2440:
						if(fmi==0){// alarma 11
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_VOLTAGE_GENERADO=false;
									CondAlarma.stat.alm11=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_VOLTAGE_GENERADO=true;
									CondAlarma.stat.alm11=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_VOLTAGE_GENERADO=true;
									CondAlarma.stat.alm11=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_ALTO_VOLTAGE_GENERADO=false;
								CondAlarma.stat.alm11=PRESENTE;
							}
						}
						if(fmi==1){// alarma 12
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_VOLTAGE_GENERADO=false;
									CondAlarma.stat.alm12=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_VOLTAGE_GENERADO=true;
									CondAlarma.stat.alm12=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_VOLTAGE_GENERADO=true;
									CondAlarma.stat.alm12=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_BAJO_VOLTAGE_GENERADO=false;
								CondAlarma.stat.alm12=PRESENTE;
							}
						}
						if(fmi==15){// alarma 13
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_VOLTAGE=false;
									CondAlarma.stat.alm13=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_VOLTAGE=true;
									CondAlarma.stat.alm13=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_VOLTAGE=true;
									CondAlarma.stat.alm13=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_ALTO_VOLTAGE=false;
								CondAlarma.stat.alm13=PRESENTE;
							}
						}
						if(fmi==17){// alarma 14
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_VOLTAGE=false;
									CondAlarma.stat.alm14=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_VOLTAGE=true;
									CondAlarma.stat.alm14=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_VOLTAGE=true;
									CondAlarma.stat.alm14=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_BAJO_VOLTAGE=false;
								CondAlarma.stat.alm14=PRESENTE;
							}
						}
			break;
			case 171:
						if(fmi==0){// alarma 15
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_TEMPERATURA=false;
									CondAlarma.stat.alm15=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_TEMPERATURA=true;
									CondAlarma.stat.alm15=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTA_TEMPERATURA=true;
									CondAlarma.stat.alm15=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_ALTA_TEMPERATURA=false;
								CondAlarma.stat.alm15=PRESENTE;
							}
						}
			break;
			case 110:
						if(fmi==15){// alarma 16
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTA_TEMPERATURA_MOTOR=false;
									CondAlarma.stat.alm16=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTA_TEMPERATURA_MOTOR=true;
									CondAlarma.stat.alm16=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTA_TEMPERATURA_MOTOR=true;
									CondAlarma.stat.alm16=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_ALTA_TEMPERATURA_MOTOR=false;
								CondAlarma.stat.alm16=PRESENTE;
							}
						}
						if(fmi==17){// alarma 17
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TEMPERATURA_MOTOR=false;
									CondAlarma.stat.alm17=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TEMPERATURA_MOTOR=true;
									CondAlarma.stat.alm17=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_TEMPERATURA_MOTOR=true;
									CondAlarma.stat.alm17=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_BAJA_TEMPERATURA_MOTOR=false;
								CondAlarma.stat.alm17=PRESENTE;
							}
						}
			break;
			case 100:
						if(fmi==1){// alarma 18
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_PRESION_ACEITE=false;
									CondAlarma.stat.alm18=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_PRESION_ACEITE=true;
									CondAlarma.stat.alm18=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJA_PRESION_ACEITE=true;
									CondAlarma.stat.alm18=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_BAJA_PRESION_ACEITE=false;
								CondAlarma.stat.alm18=PRESENTE;
							}
						}
						if(fmi==17){// alarma 19
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_PRESION_ACEITE=false;
									CondAlarma.stat.alm19=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_PRESION_ACEITE=true;
									CondAlarma.stat.alm19=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJA_PRESION_ACEITE=true;
									CondAlarma.stat.alm19=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_BAJA_PRESION_ACEITE=false;
								CondAlarma.stat.alm19=PRESENTE;
							}
						}
			break;
			case 96:
						if(fmi==0){// alarma 20
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_NIVEL_COMBUSTIBLE=false;
									CondAlarma.stat.alm20=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm20=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_ALTO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm20=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_ALTO_NIVEL_COMBUSTIBLE=false;
								CondAlarma.stat.alm20=PRESENTE;
							}
						}
						if(fmi==1){// alarma 21
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_NIVEL_COMBUSTIBLE=false;
									CondAlarma.stat.alm21=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm21=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALARMA_APAGADO_BAJO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm21=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALARMA_APAGADO_BAJO_NIVEL_COMBUSTIBLE=false;
								CondAlarma.stat.alm21=PRESENTE;
							}
						}
						if(fmi==15){// alarma 22
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_NIVEL_COMBUSTIBLE=false;
									CondAlarma.stat.alm22=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm22=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_ALTO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm22=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_ALTO_NIVEL_COMBUSTIBLE=false;
								CondAlarma.stat.alm22=PRESENTE;
							}
						}
						if(fmi==17){// alarma 23
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_NIVEL_COMBUSTIBLE=false;
									CondAlarma.stat.alm23=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm23=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_BAJO_NIVEL_COMBUSTIBLE=true;
									CondAlarma.stat.alm23=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_BAJO_NIVEL_COMBUSTIBLE=false;
								CondAlarma.stat.alm23=PRESENTE;
							}
						}
			break;
			case 2648:
						if(fmi==31){// alarma 24
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_INTERVALO_MANTENIMIENTO=false;
									CondAlarma.stat.alm24=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_INTERVALO_MANTENIMIENTO=true;
									CondAlarma.stat.alm24=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_INTERVALO_MANTENIMIENTO=true;
									CondAlarma.stat.alm24=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_INTERVALO_MANTENIMIENTO=false;
								CondAlarma.stat.alm24=PRESENTE;
							}
						}
			break;
			case 1237:
						if(fmi==31){// alarma 25
							if(!bit_test(Alarma.FlagAndCont,11)){
								if(!bit_test(Alarma.FlagAndCont,8)&&!bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_SWITCH_PARO_EMERGENCIA=false;
									CondAlarma.stat.alm25=PRESENTE;								
								}
								if(!bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_SWITCH_PARO_EMERGENCIA=true;
									CondAlarma.stat.alm25=ACTIVA;
								}
								if(bit_test(Alarma.FlagAndCont,8)&&bit_test(Alarma.FlagAndCont,10)){
									NameAlarma.bits.ALERTA_SWITCH_PARO_EMERGENCIA=true;
									CondAlarma.stat.alm25=PRESENTE;
								}	
							}
							else{
								NameAlarma.bits.ALERTA_SWITCH_PARO_EMERGENCIA=false;
								CondAlarma.stat.alm25=PRESENTE;
							}
						}	
			break;
		} 	

	NAAlarma[0]=(int)(NameAlarma.NAlarma);
	NAAlarma[1]=(int)(NameAlarma.NAlarma>>8);
	NAAlarma[2]=(int)(NameAlarma.NAlarma>>16);
	NAAlarma[3]=(int)(NameAlarma.NAlarma>>24);

	CondItemAlarma[0]=(int)(CondAlarma.ItemAlarma);
	CondItemAlarma[1]=(int)(CondAlarma.ItemAlarma>>8);
	CondItemAlarma[2]=(int)(CondAlarma.ItemAlarma>>16);
	CondItemAlarma[3]=(int)(CondAlarma.ItemAlarma>>24);

	if(NameAlarma.NAlarma==0x00000000)
		return 	2;	
	else
		return 3;
}
/*
	Se escribe en la posición 1498 un 0 para seleccionar el 
	Control del grupo generador y leer las alarmas
*/
short int WriteAlarma(void){
int status;
int cont;

	do{
		cont++;
		status=modbus_write_single_register(esclavo,1498,0);
	}while(status!=0&&cont<5);
	
	if(status==0)
		return true;
	else
		return false;
}

/*
ChekAlarma
Input:     None
Output:    0 Panel no contesta 
		   1 Lectura de Alarmas fallo
		   2 No hay alarmas	
		   3 Hay alarmas		 
*/

int ChekAlarma(void){
int cont,status,i;

	while(!WriteAlarma()&&cont++>5);

	if(cont>=4) 					
		return 0;		// Si panel no contesta retorna valor 
							// para poner alarma de lauca
	else{
		for(i=0;i<20;i++){
			status=modbus_read_holding_registers(esclavo,(PosPW[i]-1),14);
			delay_cycles(3);

				if(status==0){
					Alarma.FlagAndCont=make16(modbus_rx.data[21],modbus_rx.data[22]);
					Alarma.IdAlarma=make32(modbus_rx.data[23],modbus_rx.data[24],modbus_rx.data[25],modbus_rx.data[26]);
					Alarma.Indice=make16(modbus_rx.data[27],modbus_rx.data[28]);
				}
				else
					return 1;

			return AnalizaAlarmas();
		}
	}	
}