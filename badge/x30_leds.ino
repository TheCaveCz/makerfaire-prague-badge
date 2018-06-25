//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Animation code for eye LEDS
//


void ledsCycleCb();
void ledsFetchNextCycle();

Task ledsTask(20, TASK_FOREVER, &ledsCycleCb, &scheduler, true);

uint8_t ledsCycles;
int16_t ledsDelta1;
int16_t ledsDelta2;
uint16_t ledsCurrent1;
uint16_t ledsCurrent2;
uint8_t ledsTarget1;
uint8_t ledsTarget2;

const uint8_t *ledsAnimCurrent;
const uint8_t *ledsAnimOriginal;

#define LEDS_ANIM_END 255
#define LEDS_ANIM_LOOP 254

const uint8_t PROGMEM ledsGamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

void ledsUpdate() {
  analogWrite(PIN_LED1, pgm_read_byte(ledsGamma + (ledsCurrent1 >> 8)));
  analogWrite(PIN_LED2, pgm_read_byte(ledsGamma + (ledsCurrent2 >> 8)));
}

void ledsCalcAnimation(const uint8_t cycles, const uint8_t t1, const uint8_t t2) {
  ledsTarget1 = t1;
  ledsTarget2 = t2;
  ledsCycles = cycles;
  if (ledsCycles) {
    int32_t d1 = (int32_t(ledsTarget1) << 8) - int32_t(ledsCurrent1);
    int32_t d2 = (int32_t(ledsTarget2) << 8) - int32_t(ledsCurrent2);

    ledsDelta1 = d1 / ledsCycles;
    ledsDelta2 = d2 / ledsCycles;
  } else {
    ledsCurrent1 = ledsTarget1 << 8;
    ledsCurrent2 = ledsTarget2 << 8;

    ledsFetchNextCycle();
  }
}

void ledsFetchNextCycle() {
  if (!ledsAnimCurrent) return;

  uint8_t c = pgm_read_byte(ledsAnimCurrent);
  if (c == LEDS_ANIM_END) {
    ledsAnimCurrent = NULL;
    return;
  }
  if (c == LEDS_ANIM_LOOP) {
    ledsAnimCurrent = ledsAnimOriginal;
    c = pgm_read_byte(ledsAnimCurrent);
  }

  ledsAnimCurrent++;
  uint8_t t1 = pgm_read_byte(ledsAnimCurrent);
  ledsAnimCurrent++;
  uint8_t t2 = pgm_read_byte(ledsAnimCurrent);
  ledsAnimCurrent++;

  ledsCalcAnimation(c, t1, t2);
}

void ledsCycleCb() {
  if (ledsCycles) {
    ledsCycles--;
    if (ledsCycles) {
      ledsCurrent1 += ledsDelta1;
      ledsCurrent2 += ledsDelta2;
    } else {
      ledsCurrent1 = ledsTarget1 << 8;
      ledsCurrent2 = ledsTarget2 << 8;
      ledsFetchNextCycle();
    }
    ledsUpdate();
  }
}

void ledsSetBuiltin(bool on) {
  digitalWrite(PIN_LED3, on);
}

void ledsPower(bool on) {
  logValue("+ledsPower ", on ? 1 : 0);
  if (on) {
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    digitalWrite(PIN_LED3, LOW);
    ledsCurrent1 = 0;
    ledsCurrent2 = 0;
    ledsAnimCurrent = NULL;
    ledsUpdate();
  } else {
    pinMode(PIN_LED1, INPUT);
    pinMode(PIN_LED2, INPUT);
    pinMode(PIN_LED3, INPUT);
  }
}

void ledsSetAnim(const uint8_t * newAnim) {
  ledsAnimOriginal = ledsAnimCurrent = newAnim;
  ledsFetchNextCycle();
}

void ledsSetup() {
  ledsPower(true);
}

