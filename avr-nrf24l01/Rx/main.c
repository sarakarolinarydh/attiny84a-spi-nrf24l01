#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "../nrf24l01.h"
#include "../nrf24l01-mnemonics.h"

nRF24L01 *setup_rf(void);
void process_message(char *message);
volatile bool rf_interrupt = false;

int main(void)
{
    uint8_t address[5] = {0x01, 0x01, 0x01, 0x01, 0x01};
    // LED
    DDRB |= _BV(PB0);
    PORTB &= ~_BV(PB0);
    sei();
    nRF24L01 *rf = setup_rf();
    nRF24L01_listen(rf, 0, address);
    uint8_t addr[5];
    nRF24L01_read_register(rf, CONFIG, addr, 1);

    while (true)
    {
        if (rf_interrupt)
        {
            rf_interrupt = false;
            while (nRF24L01_data_received(rf))
            {
                nRF24L01Message msg;
                nRF24L01_read_received_data(rf, &msg);
                process_message((char *)msg.data);
            }

            nRF24L01_listen(rf, 0, address);
        }
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

void process_message(char *message)
{
    int nbr = atoi(message);

    for (int i = 0; i < nbr; i++)
    {
        PORTB |= _BV(PB0);
        _delay_ms(250);
        PORTB &= ~_BV(PB0);
        _delay_ms(250);
    }
}

ISR(INT0_vect)
{
    rf_interrupt = true;
}