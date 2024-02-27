#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "fsl_adc.h"
#include "fsl_power.h"
#include "fsl_iocon.h"
#include <stdbool.h>
#include <string.h>
#include "fsl_iap.h"
#include "fsl_pint.h"
#if defined(FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER) && FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER
#include "fsl_syscon.h"
#else
#include "fsl_inputmux.h"
#endif

#define BUFFER_SIZE 1500
#define DELAY_MOTOR 100
#define PIN_LUZ_1 20
#define PIN_LUZ_2 21
#define PIN_LUZ_3 22
#define PIN_LUZ_4 23
// PINES PERSIANA 1
#define PIN_PERSIANA_1_1 16
#define PIN_PERSIANA_1_2 17
#define PIN_PERSIANA_1_3 18
#define PIN_PERSIANA_1_4 19
// PINES PERSIANA 2
#define PIN_PERSIANA_2_1 0
#define PIN_PERSIANA_2_2 1
#define PIN_PERSIANA_2_3 4
#define PIN_PERSIANA_2_4 7
// PINES PERSIANA 3
#define PIN_PERSIANA_3_1 1
#define PIN_PERSIANA_3_2 2
#define PIN_PERSIANA_3_3 15
#define PIN_PERSIANA_3_4 14
// PINES PERSIANA 4
#define PIN_PERSIANA_4_1 13
#define PIN_PERSIANA_4_2 12
#define PIN_PERSIANA_4_3 9
#define PIN_PERSIANA_4_4 8
// PINES LUCES EXTERIORES
#define PIN_LUZ_EXT_1 28
#define PIN_LUZ_EXT_2 29
// PIN SENSOR DE MOVIMIENTO PARA INTERRUPCIÓN
#define PINT_PIN_INT0_SRC kSYSCON_GpioPort1Pin0ToPintsel

#define ADC0_CH1		1 // Defino el canal 0
char nombre_wifi[50], password_wifi[50];
uint8_t dataUsart1[64], dataUsart0[BUFFER_SIZE], count1 = 0, flag_usart1 = 0, flag_usart0 = 0;
uint16_t count0 = 0;
int flag_tick, flag_tick_ms, flag_ms = 0;
void delay_ms(int);
void delay_us(int);
void set_levels_motor_1(int,int,int,int);
void set_levels_motor_2(int,int,int,int);
void set_levels_motor_3(int,int,int,int);
void set_levels_motor_4(int,int,int,int);
void step_motor_counterclockwise(int,int);
void step_motor_clockwise(int,int);
char* sendCommandToESP01S(uint8_t*);
char* ESP_ConnectToWiFi(char*, char*);
char* ESP_HTTPGet(char*, char*);
char* ESP_HTTPPost(char*, char*, char*);
char* getHTTPContent(char*);
char* getHTTPStatusCode(char*);
int ESP_CheckWiFiStatus(void);
void parseJSONArray(char*);
char* obtenerAtributo(char*,char*);
void changeDeviceState(char*,int,int);
void USART1_WiFi_SendHandler(void);
void USART1_WiFi_ReceiveHandler(void);
#define MAX_BUFFER_FLASH 256
uint8_t sectorWifiSSID=28, sectorWifiPASS=29, sectorBlinds=30 ;
#define WIFI_SSID_DATA_ADDRESS 0x00007000
#define WIFI_PASS_DATA_ADDRESS 0x00007400
#define BLINDS_DATA_ADDRESS 0x00007800
void saveBlind1Data(const int);
void getBlindsData(int[]);
int blindsData[4];  // Estado inicial de las persianas
void saveWifiData(char* ssid, char* pass);
char* getWifiSSID();
char* getWifiPASS();
const char* terminadores[] = {
    "\r\nOK\r\n",
    "\r\nERROR\r\n",
    "\r\nFAIL\r\n",
    "\r\nCONNECT\r\n",
    "CLOSED\r\n"
};
int respuesta_completa = 0, counter = 0;
const int TIMEOUT = 3000;

typedef struct {
    char  endTime[20];
    int   id;
    int   isScheduled;
    char  name[20];
    char  startTime[20];
    int   state;
    char  type[20];
} JsonDevice;

JsonDevice devices[8];
int flag_devices = 0;
adc_result_info_t adcResultInfoStruct;
uint32_t frequency;

int flag_alarm_detected = 0;

// Callback de interrupción del GPIO1 0
void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	// Se detectó movimiento
	flag_alarm_detected = 1;

}

int main(void) {

	BOARD_InitDebugConsole();
	BOARD_BootClockFRO24M();
	(void) SysTick_Config(SystemCoreClock/100000);
	gpio_pin_config_t out_config ={kGPIO_DigitalOutput, 1};
	gpio_pin_config_t in_config ={kGPIO_DigitalInput, 0};
	GPIO_PortInit(GPIO,1); // configuro puerto CLK
	GPIO_PortInit(GPIO,0); // configuro puerto CLK
	// LUCES EXTERIORES
	GPIO_PinInit(GPIO,0,PIN_LUZ_EXT_1,&out_config);
	GPIO_PinInit(GPIO,0,PIN_LUZ_EXT_2,&out_config);
	// LUCES INTERIORES
	GPIO_PinInit(GPIO,0,PIN_LUZ_1,&out_config);
	GPIO_PinInit(GPIO,0,PIN_LUZ_2,&out_config);
	GPIO_PinInit(GPIO,0,PIN_LUZ_3,&out_config);
	GPIO_PinInit(GPIO,0,PIN_LUZ_4,&out_config);
	// PERSIANA 1
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_1_1,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_1_2,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_1_3,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_1_4,&out_config);
	// PERSIANA 2
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_2_1,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_2_2,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_2_3,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_2_4,&out_config);
	// PERSIANA 3
	GPIO_PinInit(GPIO,1,PIN_PERSIANA_3_1,&out_config);
	GPIO_PinInit(GPIO,1,PIN_PERSIANA_3_2,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_3_3,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_3_4,&out_config);
	// PERSIANA 4
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_4_1,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_4_2,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_4_3,&out_config);
	GPIO_PinInit(GPIO,0,PIN_PERSIANA_4_4,&out_config);

	GPIO_PinInit(GPIO,1,1,&out_config); // LED AZUL
	GPIO_PinInit(GPIO,1,2,&out_config); // LED ROJO
	// Pongo en cero las luces interiores por si acaso
	GPIO_PinWrite(GPIO,0,PIN_LUZ_1,0);
	GPIO_PinWrite(GPIO,0,PIN_LUZ_2,0);
	GPIO_PinWrite(GPIO,0,PIN_LUZ_3,0);
	GPIO_PinWrite(GPIO,0,PIN_LUZ_4,0);
	GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_1,0);
	GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_2,0);
	CLOCK_EnableClock(kCLOCK_Swm); // Activo la matrix de conmutación

	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN1, true); //  Activo el canal 1 P0_6

	CLOCK_DisableClock(kCLOCK_Swm);  // Desactivo la matrix de conmutación
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P0_25);	    // USART1_TXD connect to P0_25
	SWM_SetMovablePinSelect(SWM0, kSWM_USART1_RXD, kSWM_PortPin_P0_24);		// USART1_RXD connect to P0_24
	CLOCK_DisableClock(kCLOCK_Swm);
	CLOCK_Select(kUART1_Clk_From_MainClk);
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P0_27);	    // USART0_TXD connect to P0_27
	SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P0_26);		// USART0_RXD connect to P0_26
	CLOCK_DisableClock(kCLOCK_Swm);
	CLOCK_Select(kUART0_Clk_From_MainClk);

	// Configuración de ADC para el LDR
	CLOCK_Select(kADC_Clk_From_Fro);  // Configuro la frecuencia
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U);

	POWER_DisablePD(kPDRUNCFG_PD_ADC0); // Le damos alimentación al modulo
	frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
	(void) ADC_DoSelfCalibration(ADC0, frequency);

	adc_config_t adcConfigStruct;
	adc_conv_seq_config_t adcConvSeqConfigStruct;
	adcConfigStruct.clockMode = kADC_ClockSynchronousMode;
	adcConfigStruct.clockDividerNumber = 1;
	adcConfigStruct.enableLowPowerMode = false;
	adcConfigStruct.voltageRange = kADC_HighVoltageRange;
	ADC_Init(ADC0, &adcConfigStruct);

	adcConvSeqConfigStruct.channelMask = 1 << ADC0_CH1;
	adcConvSeqConfigStruct.triggerMask = 0;
	adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
	adcConvSeqConfigStruct.enableSingleStep = false;
	adcConvSeqConfigStruct.enableSyncBypass = false;
	adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
	ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct);
	ADC_EnableConvSeqA(ADC0, true);
	ADC_DoSoftwareTriggerConvSeqA(ADC0); // Hago el disparo por software

	/* Default config by using USART_GetDefaultConfig():
	 * config->baudRate_Bps = 9600U;
	 * config->parityMode = kUSART_ParityDisabled;
	 * config->stopBitCount = kUSART_OneStopBit;
	 * config->bitCountPerChar = kUSART_8BitsPerChar;
	 * config->loopback = false;
	 * config->enableRx = false;
	 * config->enableTx = false;
	 * config->syncMode = kUSART_SyncModeDisabled;
	 */
	// USART1 PARA APP DE CONFIG WIFI
	usart_config_t configUSART1;
	USART_GetDefaultConfig(&configUSART1);
	configUSART1.enableRx = true;
	configUSART1.enableTx = true;

	/* Initialize the USART with configuration. */
	USART_Init(USART1, &configUSART1, CLOCK_GetFreq(kCLOCK_MainClk));
	USART_EnableInterrupts(USART1, kUSART_RxReadyInterruptEnable);
	NVIC_EnableIRQ(USART1_IRQn);

	// USART0 PARA MODULO WIFI ESP01S
	usart_config_t configUSART0;
	USART_GetDefaultConfig(&configUSART0);
	configUSART0.enableRx = true;
	configUSART0.enableTx = true;
	configUSART0.baudRate_Bps = 115200U;
	configUSART0.parityMode = kUSART_ParityDisabled;
	configUSART0.stopBitCount = kUSART_OneStopBit;
	configUSART0.bitCountPerChar = kUSART_8BitsPerChar;

	USART_Init(USART0, &configUSART0, CLOCK_GetFreq(kCLOCK_MainClk));
	USART_EnableInterrupts(USART0, kUSART_RxReadyInterruptEnable);
	NVIC_EnableIRQ(USART0_IRQn);
	NVIC_SetPriority(USART0_IRQn, 0);

	// Configuro la interrupción para los sensores de movimiento
	#if defined(FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER) && FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER
		/* Connect trigger sources to PINT */
		SYSCON_AttachSignal(SYSCON, kPINT_PinInt0, PINT_PIN_INT0_SRC);
	#else
		/* Connect trigger sources to PINT */
		INPUTMUX_Init(INPUTMUX);
		INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, PINT_PIN_INT0_SRC);
		/* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
		INPUTMUX_Deinit(INPUTMUX);
	#endif /* FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER */

		/* Initialize PINT */
		PINT_Init(PINT);

		/* Setup Pin Interrupt 0 for rising edge */
		PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, pint_intr_callback);
		/* Enable callbacks for PINT0 by Index */
		PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);

	char* wifi = getWifiSSID();
	strncpy(nombre_wifi, wifi, sizeof(nombre_wifi) - 1);
	char* pass = getWifiPASS();
	strncpy(password_wifi, pass, sizeof(password_wifi) - 1);
	int isConnected = ESP_CheckWiFiStatus();
	if(isConnected) {
	    PRINTF("Conectado a WiFi.\n");
	    GPIO_PinWrite(GPIO,1,1,0); // Prendo led azul
	    GPIO_PinWrite(GPIO,1,2,1); // Apago led rojo
	} else {
	    PRINTF("No está conectado a WiFi.\n");
	    GPIO_PinWrite(GPIO,1,1,1); // Apago led azul
	    GPIO_PinWrite(GPIO,1,2,0); // Prendo led rojo
	    ESP_ConnectToWiFi(nombre_wifi,password_wifi);
	}

	getBlindsData(blindsData);

    while(1) {

    	USART1_WiFi_ReceiveHandler();
    	if(isConnected)
    	{
    	    GPIO_PinWrite(GPIO,1,1,0); // Prendo led azul
    	    GPIO_PinWrite(GPIO,1,2,1); // Apago led rojo
    		counter = 0;
			char* response = ESP_HTTPGet("td2-casa-domotica-api.glitch.me","/api/v1/devices");
			char* statusCode = getHTTPStatusCode(response);
			if (strcmp(statusCode, "200") != 0 || statusCode == NULL)
			{
				// Algo salió mal...
				PRINTF("HTTP Status Code: %s\n", statusCode);
				isConnected = ESP_CheckWiFiStatus();
				continue;
			}
			char* content = getHTTPContent(response);
			if (content != NULL) {

				parseJSONArray(content);

			} else {
				PRINTF("No se pudo encontrar el contenido JSON.\n");
			}
			PRINTF("%d ms\n",counter);
			if(flag_alarm_detected)
			{
				//Se detectó movimiento
				char* response = ESP_HTTPGet("td2-casa-domotica-api.glitch.me","/api/v1/devices/alarms/detected");
				PRINTF("SE DETECTÓ MOVIMIENTO\n");
				flag_alarm_detected = 0;
			}
		} else {
		    GPIO_PinWrite(GPIO,1,1,1); // Apago led azul
		    GPIO_PinWrite(GPIO,1,2,0); // Prendo led rojo
			PRINTF("No está conectado a WiFi.\n");
			ESP_ConnectToWiFi(nombre_wifi,password_wifi);
			isConnected = ESP_CheckWiFiStatus();
		}
    	if(ADC_GetChannelConversionResult(ADC0, ADC0_CH1, &adcResultInfoStruct))
		{
			PRINTF("Canal %d en PIO0_6 tiene un valor = %d\r\n", adcResultInfoStruct.channelNumber, adcResultInfoStruct.result);
			if(adcResultInfoStruct.result > 2500) {
				PRINTF("PRENDE\n");
				GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_1,1);
				GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_2,1);
			} else {
				GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_1,0);
				GPIO_PinWrite(GPIO,0,PIN_LUZ_EXT_2,0);
			}
			ADC_DoSoftwareTriggerConvSeqA(ADC0);
		}
    }
    return 0 ;
}

void SysTick_Handler(void)
{
	flag_tick++;
	flag_ms++;
	if(flag_ms == 1000)
	{
		flag_tick_ms++;
		counter++;
		flag_ms = 0;
	}
}

void delay_10us(int us)
{
	flag_tick = 0;

	while(flag_tick <= us);
}

void delay_ms(int ms)
{
	ms = (int) ms/10;
	flag_tick_ms = 0;

	while(flag_tick_ms <= ms);
}

void set_levels_motor_1(int in1, int in2, int in3, int in4)
{

	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_1_1,in1);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_1_2,in2);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_1_3,in3);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_1_4,in4);

}

void set_levels_motor_2(int in1, int in2, int in3, int in4)
{

	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_2_1,in1);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_2_2,in2);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_2_3,in3);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_2_4,in4);

}

void set_levels_motor_3(int in1, int in2, int in3, int in4)
{

	GPIO_PinWrite(GPIO,1,PIN_PERSIANA_3_1,in1);
	GPIO_PinWrite(GPIO,1,PIN_PERSIANA_3_2,in2);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_3_3,in3);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_3_4,in4);

}

void set_levels_motor_4(int in1, int in2, int in3, int in4)
{

	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_4_1,in1);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_4_2,in2);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_4_3,in3);
	GPIO_PinWrite(GPIO,0,PIN_PERSIANA_4_4,in4);

}

void step_motor_counterclockwise(int steps, int id)
{
	switch(id){
			case 1:
				for(int i = 0 ; i<steps ; i++)
				{
					set_levels_motor_1(0,0,0,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(0,0,1,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(0,0,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(0,1,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(0,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(1,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(1,0,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_1(1,0,0,1);
					delay_10us(DELAY_MOTOR);
				}
				break;
			case 2:
				for(int i = 0 ; i<steps ; i++)
				{
					set_levels_motor_2(0,0,0,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(0,0,1,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(0,0,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(0,1,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(0,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(1,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(1,0,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_2(1,0,0,1);
					delay_10us(DELAY_MOTOR);
				}
				break;
			case 3:
				for(int i = 0 ; i<steps ; i++)
				{
					set_levels_motor_3(0,0,0,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(0,0,1,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(0,0,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(0,1,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(0,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(1,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(1,0,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_3(1,0,0,1);
					delay_10us(DELAY_MOTOR);
				}
				break;
			case 4:
				for(int i = 0 ; i<steps ; i++)
				{
					set_levels_motor_4(0,0,0,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(0,0,1,1);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(0,0,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(0,1,1,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(0,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(1,1,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(1,0,0,0);
					delay_10us(DELAY_MOTOR);
					set_levels_motor_4(1,0,0,1);
					delay_10us(DELAY_MOTOR);
				}
				break;
		    default:
			  	break;
		}
}

void step_motor_clockwise(int steps, int id)
{
	switch(id){
		case 1:
			for(int i = 0; i < steps; i++)
			{
				set_levels_motor_1(1,0,0,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(1,0,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(1,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(0,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(0,1,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(0,0,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(0,0,1,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_1(0,0,0,1);
				delay_10us(DELAY_MOTOR);
			}
			break;
		case 2:
			for(int i = 0; i < steps; i++)
			{
				set_levels_motor_2(1,0,0,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(1,0,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(1,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(0,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(0,1,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(0,0,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(0,0,1,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_2(0,0,0,1);
				delay_10us(DELAY_MOTOR);
			}
			break;
		case 3:
			for(int i = 0; i < steps; i++)
			{
				set_levels_motor_3(1,0,0,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(1,0,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(1,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(0,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(0,1,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(0,0,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(0,0,1,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_3(0,0,0,1);
				delay_10us(DELAY_MOTOR);
			}
			break;
		case 4:
			for(int i = 0; i < steps; i++)
			{
				set_levels_motor_4(1,0,0,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(1,0,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(1,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(0,1,0,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(0,1,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(0,0,1,0);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(0,0,1,1);
				delay_10us(DELAY_MOTOR);
				set_levels_motor_4(0,0,0,1);
				delay_10us(DELAY_MOTOR);
			}
			break;
		default:
			break;
	}
}

void USART1_IRQHandler(void)
{
	dataUsart1[count1] = USART_ReadByte(USART1);
	count1++;
	flag_usart1 = 1;
}

void USART1_WiFi_ReceiveHandler(void)
{
	if(flag_usart1 != 0 && count1 > 0)
	{
		delay_ms(1000);
		if(dataUsart1[0] == 'G' && dataUsart1[1] == 'E' && dataUsart1[2] == 'T')
		{
			// Esta solicitando los datos
			PRINTF("LA APP ESTA PIDIENDO DATOS\n");
			USART1_WiFi_SendHandler();
		} else
		{
			char *token;
			memset(nombre_wifi, 0, sizeof(nombre_wifi)); // Inicializa todos los elementos a cero
			memset(password_wifi, 0, sizeof(password_wifi)); // Inicializa todos los elementos a cero

			// Primera llamada a strtok para obtener "wifi"
			token = strtok(dataUsart1, "\n");
			if (token != NULL) {
				strncpy(nombre_wifi, token, sizeof(nombre_wifi) - 1);
				nombre_wifi[sizeof(nombre_wifi) - 1] = '\0'; // Asegura que la cadena esté terminada correctamente
			}

			// Segunda llamada a strtok para obtener "pass"
			token = strtok(NULL, "\n");
			if (token != NULL) {
				strncpy(password_wifi, token, sizeof(password_wifi) - 1);
				password_wifi[sizeof(password_wifi) - 1] = '\0'; // Asegura que la cadena esté terminada correctamente
			}

			// Imprime los resultados
			PRINTF("LA APP ME ENVIO LOS DATOS DE WIFI\n");
			PRINTF("Wifi: %s\n", nombre_wifi);
			PRINTF("Clave Wifi: %s\n", password_wifi);
			saveWifiData(nombre_wifi, password_wifi);
		}
		PRINTF("%s\n", dataUsart1);
		memset(dataUsart1, 0, sizeof(dataUsart1)); // Inicializa todos los elementos a cero
		count1 = 0;
	}
}

void USART1_WiFi_SendHandler(void)
{
    // Crear un objeto JSON con los campos "nombre_wifi" y "password_wifi"
    char jsonBuffer[100]; // Ajusta el tamaño del búfer según tus necesidades
    snprintf(jsonBuffer, sizeof(jsonBuffer), "{\"nombre_wifi\":\"%s\",\"password_wifi\":\"%s\"}", nombre_wifi, password_wifi);
    PRINTF("Cadena JSON: %s\n", jsonBuffer);
    // Enviar la cadena JSON a través del puerto UART
    USART_WriteBlocking(USART1, jsonBuffer, strlen(jsonBuffer));
}

void USART0_IRQHandler(void)
{
	dataUsart0[count0] = USART_ReadByte(USART0);
	count0++;
	flag_usart0 = 1;
	// Vamos a verificar los terminadores contra el byte recién recibido y los bytes anteriores
	for (int i = 0; i < sizeof(terminadores) / sizeof(terminadores[0]); i++) {
		int len = strlen(terminadores[i]);

		// Solo verifica si hay suficientes caracteres en el buffer para el terminador actual
		if (count0 >= len) {
			// Compara el terminador con los últimos caracteres recibidos
			if (memcmp(&dataUsart0[count0 - len], terminadores[i], len) == 0) {
				count0 = 0;
				respuesta_completa = 1;
				break;
			}
		}
	}
}

void USART0_WiFi_ReceiveHandler(void)
{
	if(flag_usart0 != 0 && count0 > 0)
	{
		delay_ms(500);
		//PRINTF("%s\n", dataUsart0);
		memset(dataUsart0, 0, sizeof(dataUsart0)); // Inicializa todos los elementos a cero
		count0 = 0;
	}
}

char* sendCommandToESP01S(uint8_t* comando)
{
	//PRINTF("%s\n",comando);
	static char response[BUFFER_SIZE]; // Variable temporal para almacenar la respuesta. Debe ser estática para que no se pierda al salir de la función.
	respuesta_completa = 0; // Resetear el flag antes de enviar el comando
	for(int i = 0; i<strlen(comando) ; i++)
	{
		USART_WriteByte(USART0, comando[i]);
		delay_ms(1);
		//while (!(USART1->STAT & 0x8));		// Espera que el TXD este en reposo
	}
	int tiempo_espera = 0;
	while(!respuesta_completa && tiempo_espera < TIMEOUT)
	{
		delay_ms(10); // Espera 10ms
		tiempo_espera += 10;
	}
	if(tiempo_espera >= TIMEOUT)
	{
		//No se recibió respuesta en el tiempo esperado
		PRINTF("Se agotó el tiempo de espera");
	}
	// Ya se puede leer el dato
	//PRINTF("%s\n", dataUsart0);
	// Copia el contenido de dataUsart0 a response antes de borrarlo
	strncpy(response, dataUsart0, sizeof(response) - 1);
	memset(dataUsart0, 0, sizeof(dataUsart0)); // Inicializa todos los elementos a cero
	count0 = 0;
	return response;
}

char* ESP_ConnectToWiFi(char* ssid, char* password)
{
    char command[256];
    sprintf(command, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    char* response = sendCommandToESP01S(command);

    // Verificar la respuesta para determinar si la conexión fue exitosa
    if (strstr(response, "OK") != NULL) {
        return "Conexión WiFi exitosa.";
    } else if (strstr(response, "ERROR") != NULL || strstr(response, "FAIL") != NULL) {
        return "Error al conectar al WiFi.";
    } else {
        return "Respuesta desconocida del módulo ESP8266.";
    }
}


char* ESP_HTTPGet(char* host, char* path)
{
    char command[256];
    char* response;

    // Establecer conexión TCP con el servidor
    sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", host);
    response = sendCommandToESP01S(command);

    // Preparar y enviar la solicitud GET con encabezados adicionales
	sprintf(command, "GET %s HTTP/1.1\r\n"
					 "Host: %s\r\n"
					 "Connection: close\r\n"
					 "User-Agent: ESP01S/1.0\r\n" // Identifica al cliente como un ESP01S
					 "Accept: */*\r\n"            // Indica que el cliente acepta cualquier tipo de medio
					 "\r\n",
					 path, host);
    char lengthCommand[50];
    sprintf(lengthCommand, "AT+CIPSEND=%d\r\n", strlen(command));
    response = sendCommandToESP01S(lengthCommand);
    // Verificar que el ESP8266 esté listo para recibir los datos
    if (strstr(response, ">") != NULL) {
        response = sendCommandToESP01S(command);
    }

    return response;
}


char* ESP_HTTPPost(char* host, char* path, char* data)
{
    char command[512];
    char* response;

    // Establecer conexión TCP con el servidor
    sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", host);
    response = sendCommandToESP01S(command);

    // Preparar y enviar la solicitud POST
    sprintf(command, "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s", path, host, strlen(data), data);
    char lengthCommand[50];
    sprintf(lengthCommand, "AT+CIPSEND=%d\r\n", strlen(command));
    response = sendCommandToESP01S(lengthCommand);
    // Verificar que el ESP8266 esté listo para recibir los datos
    if (strstr(response, ">") != NULL) {
        response = sendCommandToESP01S(command);
    }

    return response;
}

int ESP_CheckWiFiStatus(void) {
    char* response = sendCommandToESP01S("AT+CWJAP?\r\n");

    // Verificar la respuesta para determinar el estado de la conexión WiFi
    if (strstr(response, "No AP") != NULL) {
        return 0; // No está conectado
    } else if (strstr(response, "+CWJAP:") != NULL) {
        return 1; // Está conectado
    }
    return 0; // Por defecto, asumimos que no está conectado si no se reconoce la respuesta
}

char* getHTTPStatusCode(char* response) {
    static char statusCode[4]; // Para almacenar el código de estado
    char* httpPos = strstr(response, "HTTP/1.1");
    if (httpPos == NULL) {
        httpPos = strstr(response, "HTTP/1.0");
    }
    if (httpPos != NULL) {
        strncpy(statusCode, httpPos + 9, 3); // +9 para saltar "HTTP/1.1 "
        statusCode[3] = '\0'; // Terminador de cadena
        return statusCode;
    }
    return NULL; // Si no se encuentra
}

char* getHTTPContent(char* response) {
	static char* content[BUFFER_SIZE];
    // Buscar el primer \r\n\r\n
    char* headersEnd = strstr(response, "\r\n\r\n");

    if (headersEnd != NULL) {
        // Mover el puntero al inicio del contenido después del primer \r\n\r\n
        headersEnd += 4;

        // Buscar el segundo \r\n\r\n
        headersEnd = strstr(headersEnd, "\r\n\r\n");
        if (headersEnd != NULL) {
            // Mover el puntero al inicio del contenido después del segundo \r\n\r\n
            headersEnd += 4;

            // Buscar el tercer \r\n\r\n
            headersEnd = strstr(headersEnd, "\r\n\r\n");
            if (headersEnd != NULL) {
                // Mover el puntero al inicio del contenido después del tercer \r\n\r\n
                headersEnd += 4;

                // Buscar la palabra "CLOSED"
                char* contentEnd = strstr(headersEnd, "CLOSED");
                if (contentEnd != NULL) {
                    snprintf(content, sizeof(content), "%.*s", (int)(contentEnd - headersEnd), headersEnd);
                    return content;
                }
            }
        }
    }
    return NULL;
}

void parseJSONArray(char* content) {

	char* cleanedJson = content + 1;
	cleanedJson[strlen(cleanedJson) - 1] = '\0';

	// Inicializa un puntero para recorrer la cadena JSON
	char* current = cleanedJson;
	int c = 0;
	// Itera sobre la cadena y busca objetos JSON entre '{' y '}'
	while (*current != '\0') {
		if (*current == '{') {
			// Encuentra el inicio de un objeto JSON
			char* object_start = current;
			int brace_count = 1;

			// Encuentra el final del objeto JSON
			while (*current != '\0') {
				current++;
				if (*current == '{') {
					brace_count++;
				} else if (*current == '}') {
					brace_count--;
					if (brace_count == 0) {
						// Imprime el objeto JSON
						char buffer[1024]; // Se crea un buffer para almacenar la cadena formateada
						snprintf(buffer, sizeof(buffer), "%.*s\n", (int)(current - object_start + 1), object_start);
						JsonDevice device;
						memset(device.endTime, 0, sizeof(device.endTime)); // Inicializa todos los elementos a cero
						memset(device.startTime, 0, sizeof(device.startTime)); // Inicializa todos los elementos a cero
						memset(device.name, 0, sizeof(device.name)); // Inicializa todos los elementos a cero
						memset(device.type, 0, sizeof(device.type)); // Inicializa todos los elementos a cero
						char *temp = obtenerAtributo(buffer, "\"endTime\"");

						// Copia carácter por carácter desde la variable temporal a device.startTime
						for (int i = 0; temp[i] != '\0' ; i++) {
							device.endTime[i] = temp[i];
						}
						temp = obtenerAtributo(buffer, "\"startTime\"");

						// Copia carácter por carácter desde la variable temporal a device.startTime
						for (int i = 0; temp[i] != '\0' ; i++) {
							device.startTime[i] = temp[i];
						}
						temp = obtenerAtributo(buffer, "\"name\"");

						// Copia carácter por carácter desde la variable temporal a device.startTime
						for (int i = 0; temp[i] != '\0' ; i++) {
							device.name[i] = temp[i];
						}
						temp = obtenerAtributo(buffer, "\"type\"");

						// Copia carácter por carácter desde la variable temporal a device.startTime
						for (int i = 0; temp[i] != '\0' ; i++) {
							device.type[i] = temp[i];
						}
						device.id = atoi(obtenerAtributo(buffer,"\"id\""));
						device.isScheduled = atoi(obtenerAtributo(buffer,"\"isScheduled\""));
						device.state = atoi(obtenerAtributo(buffer,"\"state\""));
						if(flag_devices == 0)
						{
							// Es la primer consulta
							if(strcmp(device.type, "blind") == 0)
							{
								// Consulto en Flash los estadios anteriores
								if(device.state != blindsData[device.id-1])
								{
									changeDeviceState(device.type,device.id,device.state);
									// Guardo el estado de la persiana
									blindsData[device.id-1] = device.state;
									saveBlindsData(blindsData);
								}
							} else {
								changeDeviceState(device.type,device.id,device.state);
							}
						} else {

							if(device.state != devices[c].state)
							{
								changeDeviceState(device.type,device.id,device.state);
							}
							if(strcmp(device.type, "blind") == 0)
							{
								// Guardo el estado de la persiana
								blindsData[device.id-1] = device.state;
								saveBlindsData(blindsData);
							}

						}

						devices[c] = device;
						c++;

						break;
					}
				}
			}
		}
		current++;
	}
	flag_devices = 1;
	//PRINTF("TERMINO CONSULTA\n");

}

// Función para obtener el valor de un atributo
char* obtenerAtributo(char *cadena,  char *atributo) {

	static char valor[20];
	memset(valor, 0, sizeof(valor)); // Inicializa todos los elementos a cero
	if (strcmp(atributo, "\"state\"") == 0 || strcmp(atributo, "\"isScheduled\"") == 0)
	{

		// Buscamos la palabra
		char *inicio = strstr(cadena, atributo);
		if (inicio == NULL) {
			return NULL; // No se encontró
		}
		for(int i = 0 ; i<strlen(atributo) ; i++)
		{
			inicio++;
		}

		// Avanzamos a los primeros : después del atributo
		inicio = strchr(inicio, ':');
		if (inicio == NULL) {
			return NULL; // No se encontró
		}

		// Avanzamos a la siguiente coma
		inicio++;
		char *fin = strchr(inicio, ',');
		if (fin == NULL) {
			// No se encontró coma
			fin = strchr(inicio, '}'); // Por ende debe ser el último elemento y hay que buscar la llave de cierre
		}

		// Copiamos el valor entre las comillas en un buffer temporal
		snprintf(valor, sizeof(valor), "%.*s", (int)(fin - inicio), inicio);

		if(strcmp(valor, "true") == 0)
		{
			return "1";
		} else {
			return "0";
		}

	} else
	{
		// Buscamos la palabra
		char *inicio = strstr(cadena, atributo);
		if (inicio == NULL) {
			return NULL; // No se encontró
		}
		for(int i = 0 ; i<strlen(atributo) ; i++)
		{
			inicio++;
		}

		// Avanzamos a la primera comilla doble después del atributo
		inicio = strchr(inicio, '\"');
		if (inicio == NULL) {
			return NULL; // No se encontró una comilla doble
		}

		// Avanzamos a la siguiente comilla doble
		inicio++;
		char *fin = strchr(inicio, '\"');
		if (fin == NULL) {
			return NULL; // No se encontró la segunda comilla doble
		}

		// Copiamos el valor entre las comillas en un buffer temporal
		snprintf(valor, sizeof(valor), "%.*s", (int)(fin - inicio), inicio);
	}

	return valor;
}


void changeDeviceState(char* type, int id, int state)
{
	if(strcmp(type, "light") == 0)
	{
		// Es una luz
		if(state){
			// Prender
			PRINTF("PRENDO LUZ %d\n",id);
			prenderLuz(id);
		} else {
			// Apagar
			PRINTF("APAGO LUZ %d\n",id);
			apagarLuz(id);
		}
	} else if(strcmp(type, "blind") == 0) {
		// Es una persiana
		if(state){
			// Abrir
			abrirPersiana(id);
			PRINTF("ABRO PERSIANA %d\n",id);
		} else {
			// Cerrar
			cerrarPersiana(id);
			PRINTF("CIERRO PERSIANA %d\n",id);
		}
	}
}

void prenderLuz(int id)
{
	switch(id){
		case 1:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_1,1);
			break;
		case 2:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_2,1);
			break;
		case 3:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_3,1);
			break;
		case 4:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_4,1);
			break;
	    default:
		  	break;
	}
}

void apagarLuz(int id)
{
	switch(id){
		case 1:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_1,0);
			break;
		case 2:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_2,0);
			break;
		case 3:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_3,0);
			break;
		case 4:
			GPIO_PinWrite(GPIO,0,PIN_LUZ_4,0);
			break;
	    default:
		  	break;
	}
}

void abrirPersiana(int id)
{
	if(id == 2 || id == 4)
	{
		step_motor_clockwise(1000, id);
	} else if(id == 1 || id == 3) {
		step_motor_counterclockwise(1000, id);
	}
}

void cerrarPersiana(int id)
{
	if(id == 2 || id == 4)
	{
		step_motor_counterclockwise(1000, id);
	} else if(id == 1 || id == 3) {
		step_motor_clockwise(1000, id);
	}
}

void saveWifiData(char* ssid, char* pass)
{
    uint8_t ssidBuffer[MAX_BUFFER_FLASH]; // Búfer temporal para SSID
    uint8_t passBuffer[MAX_BUFFER_FLASH]; // Búfer temporal para contraseña

    // Copiar SSID y contraseña a los búferes temporales
    strncpy((char*)ssidBuffer, ssid, sizeof(ssidBuffer));
    strncpy((char*)passBuffer, pass, sizeof(passBuffer));

    // Guardar el SSID
    PRINTF("Borrado de Memoria Flash\n");
    if (IAP_PrepareSectorForWrite(sectorWifiSSID, sectorWifiSSID) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }
    if (IAP_EraseSector(sectorWifiSSID, sectorWifiSSID, 24000000) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_EraseSector \n");
    }

    PRINTF("Escritura de Memoria Flash (SSID)\n");
    if (IAP_PrepareSectorForWrite(sectorWifiSSID, sectorWifiSSID) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }
    status_t respSSID = IAP_CopyRamToFlash(WIFI_SSID_DATA_ADDRESS, &ssidBuffer[0], MAX_BUFFER_FLASH, 24000000);
    if (respSSID != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_CopyRamToFlash (SSID)\n");
    }

    // Guardar la contraseña
    PRINTF("Borrado de Memoria Flash\n");
    if (IAP_PrepareSectorForWrite(sectorWifiPASS, sectorWifiPASS) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }
    if (IAP_EraseSector(sectorWifiPASS, sectorWifiPASS, 24000000) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_EraseSector \n");
    }

    PRINTF("Escritura de Memoria Flash (contraseña)\n");
    if (IAP_PrepareSectorForWrite(sectorWifiPASS, sectorWifiPASS) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite \n");
    }
    status_t respPASS = IAP_CopyRamToFlash(WIFI_PASS_DATA_ADDRESS, &passBuffer[0], MAX_BUFFER_FLASH, 24000000);
    if (respPASS != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_CopyRamToFlash (contraseña)\n");
    }
}


char* getWifiSSID()
{
	PRINTF("Lectura de Memoria Flash\n");

	// Dirección de memoria donde se guardaron los datos
	uint8_t *flashData = (uint8_t *)WIFI_SSID_DATA_ADDRESS;

	// Buffer dinámico para almacenar los datos leídos
	char* dataRead = (char*)malloc(MAX_BUFFER_FLASH);

	// Copia los datos desde la memoria flash a la RAM
	for (int i = 0; i < MAX_BUFFER_FLASH; i++) {
		dataRead[i] = *flashData++;
	}

	// Imprime los datos en la consola
	PRINTF("%s", dataRead);

	return dataRead;
}

char* getWifiPASS()
{
	PRINTF("Lectura de Memoria Flash\n");

	// Dirección de memoria donde se guardaron los datos
	uint8_t *flashData = (uint8_t *)WIFI_PASS_DATA_ADDRESS;

	// Buffer dinámico para almacenar los datos leídos
	char* dataRead = (char*)malloc(MAX_BUFFER_FLASH);

	// Copia los datos desde la memoria flash a la RAM
	for (int i = 0; i < MAX_BUFFER_FLASH; i++) {
		dataRead[i] = *flashData++;
	}

	// Imprime los datos en la consola
	PRINTF("%s", dataRead);

	return dataRead;
}

void saveBlindsData(const int data[4]) {

	if (IAP_PrepareSectorForWrite(sectorBlinds, sectorBlinds) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite\n");
        return;
    }

    if (IAP_EraseSector(sectorBlinds, sectorBlinds, 24000000) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_EraseSector\n");
        return;
    }

    if (IAP_PrepareSectorForWrite(sectorBlinds, sectorBlinds) != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_PrepareSectorForWrite\n");
        return;
    }

    status_t resp = IAP_CopyRamToFlash(BLINDS_DATA_ADDRESS, data, 64, 24000000);

    if (resp != kStatus_IAP_Success) {
        PRINTF("ERROR IAP_CopyRamToFlash\n");
    }
}

void getBlindsData(int data[]) {
    // Dirección de memoria donde se guardaron los datos
    int *flashData = (int *)BLINDS_DATA_ADDRESS;

    // Leer el estado de persianas desde la memoria flash
    for (int i = 0; i < 4; i++) {
        data[i] = *flashData++;
    }
}
