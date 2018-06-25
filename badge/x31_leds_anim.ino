//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Animation data for eye LEDS
//

const uint8_t ledsAnimWakeupShow[] PROGMEM = {
  0, 0, 0,
  300 / 20, 255, 255,
  1200 / 20, 255, 255,
  80 / 20, 0, 0,
  80 / 20, 255, 255,
  500 / 20, 255, 255,
  80 / 20, 0, 0,
  80 / 20, 255, 255,

  LEDS_ANIM_END
};

const uint8_t ledsAnimPresence[] PROGMEM = {
  500 / 20, 255, 64,
  500 / 20, 64, 255,
  LEDS_ANIM_LOOP
};

const uint8_t ledsAnimAcknowledge[] PROGMEM = {
  500 / 20, 255, 255,
  500 / 20, 64, 64,
  LEDS_ANIM_LOOP
};

const uint8_t ledsAnimDuplicate[] PROGMEM = {
  1000 / 20, 32, 100,
  1000 / 20, 100, 32,
  LEDS_ANIM_LOOP
};

const uint8_t ledsAnimDump[] PROGMEM = {
  500 / 20, 128, 64,
  500 / 20, 128, 128,
  500 / 20, 64, 128,
  500 / 20, 128, 128,
  LEDS_ANIM_LOOP
};
