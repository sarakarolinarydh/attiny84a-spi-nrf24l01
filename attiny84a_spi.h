#include <util/atomic.h>

#ifndef ATTINY84A_SPI_H
#define ATTINY84A_SPI_H

#define SPI_PORT DDRA
#define USCK_PIN PA4
#define DO_PIN PA5
#define DI_PIN PA6

void spi_slave_init();
void spi_master_init();
uint8_t spi_transfer_slave(uint8_t data);
uint8_t spi_transfer_master(uint8_t data);
void spi_end();

#endif