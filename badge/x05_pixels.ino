/*
  Parts of this code are taken from Adafruit NeoPixel library.
  Carefully, this code works only on 8MHz CPU
  Also only on PORTB pins


  ----------------------------------------------------------------------------
  Arduino library to control a wide variety of WS2811- and WS2812-based RGB
  LED devices such as Adafruit FLORA RGB Smart Pixels and NeoPixel strips.
  Currently handles 400 and 800 KHz bitstreams on 8, 12 and 16 MHz ATmega
  MCUs, with LEDs wired for various color orders.  Handles most output pins
  (possible exception with upper PORT registers on the Arduino Mega).

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
  contributions by PJRC, Michael Miller and other members of the open
  source community.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

*/

#define PIXELS_COUNT 3
#define PIXELS_BYTES PIXELS_COUNT*3

// colors GRB

const uint8_t pixelIdColors[3 * 8] PROGMEM = {
  0, 0, 0, // 0 black
  0, 0, 100, // 1 blue
  0, 15, 0, // 2 red
  0, 30, 50, // 3 violet
  15, 0, 0, // 4 green
  15, 0, 35, // 5 cyan
  15, 15, 0, // 6 yellow
  25, 25, 50, // 7 white
};

uint8_t pixels[PIXELS_BYTES];
uint32_t pixelsEndTime;

void pixelsPower(bool on) {
  logValue("+pixelsPower ", on ? 1 : 0);

  if (on) {
    pinMode(PIN_LEDS_WS_ENABLE, OUTPUT);
    digitalWrite(PIN_LEDS_WS_ENABLE, LOW);
    pinMode(PIN_LEDS_WS, OUTPUT);
    digitalWrite(PIN_LEDS_WS, LOW);
  } else {
    pinMode(PIN_LEDS_WS, INPUT);
    pinMode(PIN_LEDS_WS_ENABLE, INPUT);
  }
}

void pixelsSetup() {
  memset(pixels, 0, sizeof(pixels));
  pixelsEndTime = 0;
  pixelsPower(true);
}

void pixelsShow(void) {
  while ((micros() - pixelsEndTime) < 300L);
  uint8_t pixelsPinMask = digitalPinToBitMask(PIN_LEDS_WS);
  volatile uint16_t i = PIXELS_BYTES; // Loop counter
  volatile uint8_t *ptr = pixels,   // Pointer to next byte
                    b   = *ptr++,   // Current byte value
                    hi,             // PORT w/output bit set high
                    lo;             // PORT w/output bit set low

  noInterrupts();

  volatile uint8_t n1, n2;

  hi = PORTB |  pixelsPinMask;
  lo = PORTB & ~pixelsPinMask;
  n1 = lo;
  if (b & 0x80) n1 = hi;

  asm volatile(
    "headB:"                   "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n2]   , %[lo]"    "\n\t"
    "out  %[port] , %[n1]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 6"        "\n\t"
    "mov %[n2]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n1]   , %[lo]"    "\n\t"
    "out  %[port] , %[n2]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 5"        "\n\t"
    "mov %[n1]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n2]   , %[lo]"    "\n\t"
    "out  %[port] , %[n1]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 4"        "\n\t"
    "mov %[n2]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n1]   , %[lo]"    "\n\t"
    "out  %[port] , %[n2]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 3"        "\n\t"
    "mov %[n1]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n2]   , %[lo]"    "\n\t"
    "out  %[port] , %[n1]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 2"        "\n\t"
    "mov %[n2]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n1]   , %[lo]"    "\n\t"
    "out  %[port] , %[n2]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 1"        "\n\t"
    "mov %[n1]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n2]   , %[lo]"    "\n\t"
    "out  %[port] , %[n1]"    "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte] , 0"        "\n\t"
    "mov %[n2]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "sbiw %[count], 1"        "\n\t"
    "out  %[port] , %[hi]"    "\n\t"
    "mov  %[n1]   , %[lo]"    "\n\t"
    "out  %[port] , %[n2]"    "\n\t"
    "ld   %[byte] , %a[ptr]+" "\n\t"
    "sbrc %[byte] , 7"        "\n\t"
    "mov %[n1]   , %[hi]"    "\n\t"
    "out  %[port] , %[lo]"    "\n\t"
    "brne headB"              "\n"
    : [byte] "+r" (b), [n1] "+r" (n1), [n2] "+r" (n2), [count] "+w" (i)
    : [port] "I" (_SFR_IO_ADDR(PORTB)), [ptr] "e" (ptr), [hi] "r" (hi),
    [lo] "r" (lo));

  interrupts();
  pixelsEndTime = micros();
}

// translates ID to pixel color using pixelIdColors table
void pixelsShowId(uint16_t id) {
  logValue("Showing ID ", id);

  const uint8_t* colorBase;
  uint8_t *pixelBase = pixels;

  for (uint8_t j = 0; j < 3; j++) {
    colorBase = pixelIdColors + (id & 0b111) * 3;
    for (uint8_t i = 0; i < 3; i++) {
      *pixelBase++ = pgm_read_byte(colorBase++);
    }
    id >>= 3;
  }

  pixelsShow();
}

// used for debug only
void pixelsShowCol(uint8_t id, uint8_t pos) {
  const uint8_t* colorBase;
  uint8_t *pixelBase = pixels + pos * 3;

  colorBase = pixelIdColors + (id & 0b111) * 3;
  for (uint8_t i = 0; i < 3; i++) {
    *pixelBase++ = pgm_read_byte(colorBase++);
  }
}

void pixelsSet(uint32_t c) {
  uint8_t r, g, b;
  r = (uint8_t)(c >> 16);
  g = (uint8_t)(c >>  8);
  b = (uint8_t)c;

  pixels[0] = r;
  pixels[1] = g;
  pixels[2] = b;
  pixels[3] = r;
  pixels[4] = g;
  pixels[5] = b;
  pixels[6] = r;
  pixels[7] = g;
  pixels[8] = b;

  pixelsShow();
}

