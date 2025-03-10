#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include "attiny84a_spi.h"


void spi_slave_init()
{
    SPI_PORT &= ~_BV(USCK_PIN);
    SPI_PORT |= _BV(DO_PIN);
    SPI_PORT &= ~_BV(DI_PIN);

    /*
    USICR: USI Control Register
    USIWM1, USIWM0, USICS1, USICS0, USICLK: USI config bits
    */
    USICR &= ~(_BV(USIWM1) | _BV(USICS0) | _BV(USICLK));
    USICR |= _BV(USIOIE) | _BV(USIWM0) | _BV(USICS1);
    sei();
}

uint8_t spi_transfer_slave(uint8_t data)
{
    /*
    USIDR: Container of the incoming and outgoing data
    USISR: USI Status Register
    USIOIF: Flag in USISR
    USICR: USI Control Register
    USIWM0, USICS1, USICLK, USITC: USI config bits
    */
    USIDR = data;
    USISR = _BV(USIOIF);
    while ((USISR & _BV(USIOIF)) == 0){};
    return USIDR;
}

void spi_master_init()
{
    SPI_PORT |= _BV(USCK_PIN);
    SPI_PORT |= _BV(DO_PIN);
    SPI_PORT &= ~_BV(DI_PIN);
}

uint8_t spi_transfer_master(uint8_t data)
{
    /*
    USIDR: Container of the incoming and outgoing data
    USISR: USI Status Register
    USIOIF: Flag in USISR
    USICR: USI Control Register
    USIWM0, USICS1, USICLK, USITC: USI config bits
    */
    USIDR = data;
    USISR = _BV(USIOIF);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    while (!(USISR & _BV(USIOIF)))
    {
        USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC);
    }
    return USIDR;
}

void spi_end()
{
    USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
}