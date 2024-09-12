#include <stdint.h>
#include <stm32f10x.h>

// Apuntadores del reloj del sistema y puertos
#define RCC_CR       (*((volatile uint32_t *) 0x40021000))          // Registro para controlar el reloj del sistema
#define RCC_CFGR     (*((volatile uint32_t *) 0x40021004))          // Registro para configurar el reloj
#define RCC_APB2ENR  (*((volatile uint32_t *) 0x40021018))          // Registro para habilitar el reloj para periféricos
	
// Apuntadores de puertos GPIO
#define GPIOA_CRH    (*((volatile uint32_t *) 0x40010804))          // Registro de control para el puerto A, pines 8-15
#define GPIOA_ODR    (*((volatile uint32_t *) 0x4001080C))          // Registro de salida de datos del puerto A
	
#define GPIOB_CRL    (*((volatile uint32_t *) 0x40010C00))          // Registro de control para el puerto B, pines 0-7
#define GPIOB_ODR    (*((volatile uint32_t *) 0x40010C0C))          // Registro de salida de datos del puerto B

// Apuntadores de puertos GPIOC
#define GPIOC_CRH    (*((volatile uint32_t *) 0x40011004))          // Registro de control para el puerto C, pines 8-15
#define GPIOC_ODR    (*((volatile uint32_t *) 0x4001100C))          // Registro de salida de datos del puerto C
#define GPIOC_BSRR   (*((volatile uint32_t *) 0x40011010))          // Registro para establecer o restablecer bits de puerto C

// Apuntadores de registros de TIM1
#define TIM1_CR1     (*((volatile uint32_t *) 0x40012C00))          // Registro de control 1 de TIM1
#define TIM1_SMCR    (*((volatile uint32_t *) 0x40012C08))          // Registro de modo esclavo de TIM1
#define TIM1_SR      (*((volatile uint32_t *) 0x40012C10))          // Registro de estado de TIM1
#define TIM1_EGR     (*((volatile uint32_t *) 0x40012C14))          // Registro de generación de eventos de TIM1
#define TIM1_CNT     (*((volatile uint32_t *) 0x40012C24))          // Registro de contador de TIM1
#define TIM1_PSC     (*((volatile uint32_t *) 0x40012C28))          // Registro de prescaler de TIM1
#define TIM1_ARR     (*((volatile uint32_t *) 0x40012C2C))          // Registro de auto-reload de TIM1
#define TIM1_RCR     (*((volatile uint32_t *) 0x40012C30))          // Registro de repetición de TIM1
#define TIM1_CCMR1   (*((volatile uint32_t *) 0x40012C18))          // Registro de captura/comparación 1 de TIM1
#define TIM1_CCER    (*((volatile uint32_t *) 0x40012C20))          // Registro de habilitación de captura/comparación de TIM1


int main(){

    // Configuración del reloj del sistema
    RCC_CR |= RCC_CR_HSION; 							                      // Enciendo el oscilador interno HSI
    while(!(RCC_CR & RCC_CR_HSIRDY)){} 		                  // Espero a que el oscilador HSI esté listo
    RCC_CFGR &= ~(0x000000F3);						                    // Configuro el reloj a su valor por defecto

    // Configuración del puerto B para los pines 0 y 1
    RCC_APB2ENR |= RCC_APB2ENR_IOPBEN; 		                      // Habilito el reloj para el puerto B
		GPIOB_CRL &= ~(0x000000FF);					                    // Limpio los bits de configuración para los pines 0 y 1
    GPIOB_CRL |= 0x00000022;						                      // Configuro los pines 0 y 1 como salidas push-pull a 2 MHz
			
    // Configuración del puerto A para la señal TI2 (TIM1_CH2 en PA9)
    RCC_APB2ENR |= 0x005;							                          // Habilito el reloj para el puerto A y AFIO
    GPIOA_CRH &= ~(0x000000F0);					                      // Limpio los bits de configuración de PA9
    GPIOA_CRH |= 0x00000080;						                        // Configuro PA9 como entrada flotante para TI2
    GPIOA_ODR |= 0x0200;						                          // Establezco PA9 en 1 para usarlo como entrada

    // Configuración del temporizador TIM1
    RCC_APB2ENR |= 0x800; 						                          // Habilito el reloj para TIM1
    TIM1_SMCR &= ~(0xFFFF);					                          // Limpio el registro de modo esclavo

    // Configuración del temporizador TIM1 en modo esclavo usando TI2
    TIM1_SMCR |= 0X0067;  					                            // Configuro TS=110 para TI2FP2 y SMS=111 para modo esclavo de reloj externo
    TIM1_CCMR1 &= ~(0x0300);					                          // Limpio los bits CC1S
    TIM1_CCMR1 |= 0x0100; 						                          // Configuro CC1S = 01 (entrada TI2) y filtro IC1F = 0000
    TIM1_CCMR1 &= ~(0xF000);					                          // Limpio bits innecesarios en CCMR1
    TIM1_CCER &= 0XFFFF;					                              // Desactivo las configuraciones innecesarias de CCER

    // Configuración de los parámetros del temporizador
    TIM1_PSC = 0; 						                                // Configuro el prescaler a 0
    TIM1_ARR = 3; 						                                // Configuro el contador para contar hasta 3
    TIM1_RCR = 0x00;						                              // No uso repetición (repetition count)
    TIM1_CNT = 0; 						                                // Inicializo el contador en 0
    TIM1_CR1 |= 0x000;					                                // Limpio el registro de control 1

    // Inicio del temporizador
    TIM1_EGR |= 0x01; 						                              // Genero un evento de actualización para sincronizar el temporizador
    TIM1_CR1 |= 0x01; 						                              // Habilito el temporizador TIM1

    // Bucle infinito para mostrar el conteo en los LEDs
    while(1){
        GPIOB_ODR = (TIM1_CNT);			                                // Muestro el valor del contador TIM1 en los pines 0 y 1 del puerto B
    }

}
