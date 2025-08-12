#ifndef __STM32F0xx_IT_H
#define __STM32F0xx_IT_H

void SysTick_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void NMI_Handler (void);
void SVCall_Handler(void);
void PendSV_Handler(void);
void USB_IRQHandler(void);

#endif

