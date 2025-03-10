#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "../nrf24l01.h"

#define DEBOUNCE_TIME 20
#define SEQUENCE_FINISHED 500

nRF24L01 *setup_rf(void);
void setup_millis();
unsigned long millis();
void setup_button();
void button_check();

volatile bool rf_interrupt = false;
volatile bool send_message = false;
volatile bool button_interrupt = false;
bool button_pushed = false;
volatile unsigned long millis_value = 0;
unsigned long current_millis = 0;
unsigned long timestamp_button_pushed = 0;
unsigned long timestamp_button_irq = 0;
static int count = 0;

enum Button_state
{
    UP,
    DOWN
} button_state;
int button_state_size = 2;

int main(void)
{
    uint8_t to_address[5] = {0x01, 0x01, 0x01, 0x01, 0x01};
    sei();
    nRF24L01 *rf = setup_rf();
    setup_button();
    setup_millis();
    // LED
    DDRB |= _BV(PB0);
    PORTB &= ~_BV(PB0);

    while (true)
    {
        if (rf_interrupt)
        {
            rf_interrupt = false;
            int success = nRF24L01_transmit_success(rf);
            if (success != 0)
                nRF24L01_flush_transmit_message(rf);
        }

        if (send_message)
        {
            send_message = false;

            PORTB |= _BV(PB0);
            _delay_ms(250);
            PORTB &= ~_BV(PB0);
            _delay_ms(250);

            nRF24L01Message msg;

            int len = snprintf(NULL, 0, "%d", count);
            char *str = malloc(len + 1);
            snprintf(str, len + 1, "%d", count);
            memcpy(msg.data, str, len + 1);
            free(str);
            count = 0;

            msg.length = strlen((char *)msg.data) + 1;
            nRF24L01_transmit(rf, to_address, &msg);
        }

        current_millis = millis();

        if (button_pushed)
        {
            button_pushed = false;
            count++;
            timestamp_button_pushed = current_millis;
        }

        if ((count > 0) && (current_millis - timestamp_button_pushed >= SEQUENCE_FINISHED))
        {
            send_message = true;
        }

        button_check();
    }

    return 0;
}

nRF24L01 *setup_rf(void)
{
    nRF24L01 *rf = nRF24L01_init();

    rf->ss.port = &PORTA;
    rf->ss.pin = PA0;
    rf->ce.port = &PORTA;
    rf->ce.pin = PA1;

    // Interrupt
    MCUCR |= _BV(ISC01);
    GIMSK |= _BV(INT0);

    nRF24L01_begin(rf);

    return rf;
}

void setup_millis()
{
    // Interrupt
    TCCR0A = 1 << WGM01;
    TCCR0B |= 1 << CS01 | 1 << CS00;
    TIMSK0 |= 1 << OCIE0A;
    OCR0A = 115;
}

unsigned long millis()
{
    unsigned long m;
    cli();
    m = millis_value;
    sei();
    return m;
}

void setup_button()
{
    DDRB &= ~_BV(PB1);
    PORTB |= _BV(PB1);
    // Interrupt
    PCMSK1 |= _BV(PCINT9);
    GIMSK |= _BV(PCIE1);
}

void button_check()
{
    if (button_interrupt == true)
    {
        if (current_millis - timestamp_button_irq >= DEBOUNCE_TIME)
        {
            if (button_state == UP)
            {
                button_pushed = true;
            }
            button_state++;
            button_state = button_state % button_state_size;
            button_interrupt = false;
            // Enable button interrupt
            GIMSK |= _BV(PCIE1);
        }
    }
}

ISR(INT0_vect)
{
    rf_interrupt = true;
}

ISR(TIM0_COMPA_vect)
{
    millis_value++;
}

ISR(PCINT1_vect)
{
    // Disable button interrupt
    GIMSK &= ~_BV(PCIE1);
    button_interrupt = true;
    timestamp_button_irq = current_millis;
}
