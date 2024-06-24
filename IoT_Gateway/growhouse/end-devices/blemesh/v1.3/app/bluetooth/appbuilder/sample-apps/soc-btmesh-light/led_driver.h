#ifndef _LED_DRIVER_H
#define _LED_DRIVER_H

#include <stdint.h>

/**
 *  State of the LEDs is updated by calling LED_set_state().
 *  The new state is passed as parameter, possible values are defined below.
 */
#define LED_STATE_OFF    0   /* light off (both LEDs turned off)   */
#define LED_STATE_ON     1   /* light on (both LEDs turned on)     */
#define LED_STATE_PROV   2   /* provisioning (LEDs blinking)       */

/* Temperature parameters */
#define DEFAULT_TEMPERATURE  5600
#define DEFAULT_DELTAUV      0

#define MIN_TEMPERATURE      0x0320
#define MAX_TEMPERATURE      0x4e20

/* this signal is used to notify main application when LED level is changed */
#define EXT_SIGNAL_LED_LEVEL_CHANGED        0x1
#define EXT_SIGNAL_LED_TEMPERATURE_CHANGED  0x2

void LEDS_init(void);
void LEDS_SetLevel(uint16_t level, uint16_t delay_ms);
void LEDS_SetTemperature(uint16_t temperature, int16_t deltauv, uint16_t delay_ms);
void LEDS_SetState(int state);
uint16_t LEDS_GetLevel(void);
uint16_t LEDS_GetTemperature(void);
uint16_t LEDS_GetDeltaUV(void);

#endif
