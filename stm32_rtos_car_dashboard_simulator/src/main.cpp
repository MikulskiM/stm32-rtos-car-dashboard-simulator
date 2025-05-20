#include "config.h"

#define GREEN_LED_PIN   GPIO_PIN_12
#define ORANGE_LED_PIN  GPIO_PIN_13
#define BLUE_LED_PIN    GPIO_PIN_15

class LED {
public:
    static void init() {
        __HAL_RCC_GPIOD_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GREEN_LED_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
    }

    static void toggle() {
        HAL_GPIO_TogglePin(LED_PORT, GREEN_LED_PIN);
    }
};

int main() {
    HAL_Init();
    SystemClock_Config();
    UART_Init();
    LED::init();

    while (1) {
        LED::toggle();
        HAL_Delay(500);
    }
}
