#ifndef _I2C_H_
#define _I2C_H_

#include "stm32sunidMainSmall.h"


#define sunid_I2C                          I2C1
#define sunid_I2C_CLK                      RCC_APB1Periph_I2C1
#define sunid_I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define sunid_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sunid_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sunid_I2C_SDA_PIN                  GPIO_Pin_7                  /* PB.07 */
#define sunid_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sunid_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB


//init I2C1 for touchpad and crypto memory
void I2CInit(void);

#endif
