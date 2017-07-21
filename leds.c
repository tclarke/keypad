#include <bcm2835.h>
#include <stdbool.h>
#include "leds.h"

#define RED_LED 21 
#define GREEN_LED 20 

bool initLeds()
{
    if (!bcm2835_init())
    {
        return false;
    }

    bcm2835_gpio_fsel(RED_LED, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(GREEN_LED, BCM2835_GPIO_FSEL_OUTP);
    clearLeds();
    return true;
}

void cleanupLeds()
{
    clearLeds();
    bcm2835_close();
}

void setRed()
{
    bcm2835_gpio_write(GREEN_LED, LOW);
    bcm2835_gpio_write(RED_LED, HIGH);
}

void setGreen()
{
    bcm2835_gpio_write(RED_LED, LOW);
    bcm2835_gpio_write(GREEN_LED, HIGH);
}

void clearLeds()
{
    bcm2835_gpio_write(RED_LED, LOW);
    bcm2835_gpio_write(GREEN_LED, LOW);
}
