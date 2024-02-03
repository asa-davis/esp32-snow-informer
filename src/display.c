#include "display.h"

/* MEMBER VARIABLES */

// true for high cycle, false for low cycle
static bool cycle = true;

// bits to send for each cycle
static uint16_t highLowBits[2] = 
{
    0b1000000000000000,
    0b0100000000000000
};

/* PRIVAT CONSTANTS */

// segment to bit position maps for each segment of each digit where positive is for high cycle and negative is for low cycle
// example:
//		segment b in digit 2 (100s) -> segmentBits[1][1] = 10
//		therefore, digit 10 (0b1 << 9) should be set in the high cycle to turn on segment b in digit 2
const int8_t segmentBits[4][7] = 
{
	//   {0,12,11,0,0,0,0},
	//   {8,10,9,-9,-11,-8,-10},
	//   {-7,-6,-5,5,4,7,6},
	//   {1,3,2,-2,-4,-1,-3}
    {0,-3,-4,0,0,0,0},
    {-9,-2,-1,1,3,9,2},
    {10,11,12,-12,-13,-10,-11},
    {-16,-14,-15,15,13,16,14}
};

// the segments to light for each digit 0-9
const uint8_t ds0[6] = {0,1,2,3,4,5};
const uint8_t ds1[2] = {1,2};
const uint8_t ds2[5] = {0,1,3,4,6};
const uint8_t ds3[5] = {0,1,2,3,6};
const uint8_t ds4[4] = {1,2,5,6};
const uint8_t ds5[5] = {0,2,3,5,6};
const uint8_t ds6[6] = {0,2,3,4,5,6};
const uint8_t ds7[3] = {0,1,2};
const uint8_t ds8[7] = {0,1,2,3,4,5,6};
const uint8_t ds9[5] = {0,1,2,5,6};
const uint8_t* digitSegments[10] = {ds0,ds1,ds2,ds3,ds4,ds5,ds6,ds7,ds8,ds9};

// the number of segments to light for each digit 0-9
const uint8_t digitSegmentsSizes[10] = 
{
		6,	// 0
		2,	// 1
		5,	// 2
		5,	// 3
		4,	// 4
		5,	// 5
		6,	// 6
		3,	// 7
		7,	// 8
		5	// 9
};

/* PRIVATE FUNCTIONS */

// send 16 bits out 
static void send_bits(uint16_t bits) 
{
    gpio_set_level(DATA_PIN, 0);
    gpio_set_level(CLOCK_PIN, 0);
    gpio_set_level(LATCH_PIN, 0);
    
    for (uint8_t i = 0;  i < 16; i++) 
    {
        gpio_set_level(CLOCK_PIN, 0);
        
        if (bits & (1 << i))
        {
            gpio_set_level(DATA_PIN, 1);
        }
        else
        {
            gpio_set_level(DATA_PIN, 0);
        }

        gpio_set_level(CLOCK_PIN, 1);
        gpio_set_level(DATA_PIN, 0);
    }

    gpio_set_level(LATCH_PIN, 1);
    gpio_set_level(LATCH_PIN, 0);
}

// get the digits at 1000s, 100s, 10s, and 1s place from a number
static void get_digits(uint16_t num, uint8_t digitsOut[]) {
    digitsOut[0] = num / 1000;
    num -= digitsOut[0] * 1000;
    digitsOut[1] = num / 100;
    num -= digitsOut[1] * 100;
    digitsOut[2] = num / 10;
    num -= digitsOut[2] * 10;
    digitsOut[3] = num;
}

// get the high and low cycle bits to display the num
void get_high_low_bits(uint16_t num, uint16_t highLowOut[]) {
	uint16_t high = 0b0000000010000000;
	uint16_t low =  0b0000000001000000;

	uint8_t digits[4];
	get_digits(num, digits);

	// can only handle '1' in the 1000s place
	if(digits[0] > 1) digits[0] = 0;

	bool numStarted = false;
	for(uint8_t place = 0; place < 4; place++) {
		const uint8_t digit = digits[place];

		if(!numStarted && place < 3) {
			if(digit == 0) continue;
			numStarted = true;
		}

		const uint8_t* segments = digitSegments[digit];
		uint8_t numSegments = digitSegmentsSizes[digit];
		for(uint8_t segmentIndex = 0; segmentIndex < numSegments; segmentIndex++) {
			int8_t bit = segmentBits[place][segments[segmentIndex]];

			if(bit > 0) high = high | (1 << (bit - 1));

			else low = low | (1 << ((-1 * bit) - 1));
		}
	}

	highLowOut[0] = high;
	highLowOut[1] = low;
}

/* PUBLIC FUNCTIONS */

// initialize pins
void display_init()
{
    gpio_pad_select_gpio(DATA_PIN);
    gpio_set_direction(DATA_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(CLOCK_PIN);
    gpio_set_direction(CLOCK_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LATCH_PIN);
    gpio_set_direction(LATCH_PIN, GPIO_MODE_OUTPUT);
}

// set the high and low cycle bits to proper values to display num
void set_display(uint16_t num)
{
    get_high_low_bits(num, highLowBits);
}

// set all lights on
void set_display_test()
{
    highLowBits[0] = 0b1111111110001111;
    highLowBits[1] = 0b1111111101001111;
}

// called every 10ms switches cycles and sends appropriate bits to shift registers
void display_tick()
{
    if(cycle) send_bits(highLowBits[0]);
    else send_bits(highLowBits[1]);
    cycle = !cycle;
}