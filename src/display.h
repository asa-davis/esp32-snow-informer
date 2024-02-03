#ifndef DISPLAY_H
#define DISPLAY_H

#define DATA_PIN 32
#define LATCH_PIN 33 // CHANGE TO LATCH
#define CLOCK_PIN 25 // CHANGE TO CLOCK

#include <stdbool.h>
#include <driver/gpio.h>
#include <rom/gpio.h>

void display_init();

void set_display(uint16_t num);

void display_tick();

void set_display_test();

#endif // DISPLAY_H