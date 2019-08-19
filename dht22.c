/* Fast DHT Lirary
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by Adafruit Industries. MIT license.
 */

#include "dht22.h"
#include "myriota_user_api.h"

// #include <avr/io.h>
// #include <util/delay.h>
// #include <avr/interrupt.h>

#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85

// For usleep
#define CLOCK_RATE 48000000 // Clock rate 48MHz
#define CYCLE_PER_US (CLOCK_RATE / 1000000)
#define CYCLES_PER_LOOP 3

static inline void usleep(uint32_t usec)
{
    uint32_t loop = usec * CYCLE_PER_US / CYCLES_PER_LOOP;
    asm volatile("0:"
                 "SUBS %[count], 1;"
                 "BNE 0b;"
                 : [count] "+r"(loop));
}

void dht_init(struct dht22 *dht, uint8_t pin)
{
    dht->pin = pin;
    /* Setup the pins! */
    // Direction OUTPUT
    GPIOSetModeOutput(dht->pin);
    GPIOSetHigh(dht->pin);
    //PORT_DHT |= (1 << dht->pin);
    //DDR_DHT &= ~(1 << dht->pin);
}

static uint8_t dht_read(struct dht22 *dht)
{
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint16_t counter = 0;

    /*
     * Pull the pin High and wait 250 milliseconds
     */
    GPIOSetModeOutput(dht->pin);
    GPIOSetHigh(dht->pin);
    usleep(250 * 1000); //ms

    dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;

    /** START SIGNAL */
    /* Now pull it low for ~20 milliseconds */
    GPIOSetLow(dht->pin);
    usleep(20 * 1000); //20ms

    //cli(); Disable interrupts
    GPIOSetHigh(dht->pin);
    usleep(40);

    GPIOSetModeInput(dht->pin, GPIO_PULL_UP);

    /* Read the timings */
    for (i = 0; i < DHT_MAXTIMINGS; i++)
    {
        counter = 0;
        while (GPIOGet(dht->pin) == last_state)
        {
            counter++;
            usleep(3);

            if (counter == 255)
                break;
        }
        last_state = GPIOGet(dht->pin);

        if (counter == 255)
            break;

        /* Ignore first 3 transitions */
        if ((i >= 4) && (i % 2 == 0))
        {
            /* Shove each bit into the storage bytes */
            dht->data[j / 8] <<= 1;
            if (counter > DHT_COUNT)
                dht->data[j / 8] |= 1;
            j++;
        }
    }

    printf("%i %i %i %i\n", dht->data[0], dht->data[1], dht->data[2], dht->data[3]);

    sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];

    printf("checksum = %d", sum);

    if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))
    {
        printf("checksum passed");
        return 1;
    }
    else
    {
        printf("checksum failed\n");
    }
    return 0;
}

uint8_t dht_read_temp(struct dht22 *dht, float *temp)
{
    if (dht_read(dht))
    {
        *temp = dht->data[2] & 0x7F;
        *temp *= 256;
        *temp += dht->data[3];
        *temp /= 10;

        if (dht->data[2] & 0x80)
            *temp *= -1;
        return 1;
    }
    return 0;
}

uint8_t dht_read_hum(struct dht22 *dht, float *hum)
{
    if (dht_read(dht))
    {
        *hum = dht->data[0];
        *hum *= 256;
        *hum += dht->data[1];
        *hum /= 10;
        if (*hum == 0.0f)
            return 0;
        return 1;
    }
    return 0;
}

uint8_t dht_read_data(struct dht22 *dht, float *temp, float *hum)
{
    if (dht_read(dht))
    {
        /* Reading temperature */
        *temp = dht->data[2] & 0x7F;
        *temp *= 256;
        *temp += dht->data[3];
        *temp /= 10;

        if (dht->data[2] & 0x80)
            *temp *= -1;
        printf("temp = %f", *temp);

        /* Reading humidity */
        *hum = dht->data[0];
        *hum *= 256;
        *hum += dht->data[1];
        *hum /= 10;
        if (*hum == 0.0f)
            return 0;

        printf("humidity = %f", *hum);
        return 1;
    }
    return 0;
}
