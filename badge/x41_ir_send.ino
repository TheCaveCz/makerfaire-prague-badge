//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Non-portable IR send expecting IR led on pin 3 using timer2. Uses NEC protocol timing
//


void irSendData(uint32_t data) {
  //TIMSK2 = 0; //Timer2 Overflow Interrupt

  // we need to disable receiver because of crosstalk
  irRecvEnable(false);

  // low when not sending
  pinMode(PIN_IRTX, OUTPUT);
  digitalWrite(PIN_IRTX, LOW);

  // 38kHz freq on pin 3 using timer2
  const uint8_t pwmval = F_CPU / 2000 / 38;
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = pwmval;
  OCR2B = pwmval / 3;

  // header (16 slots on, 8 slots off)
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(564 * 16);
  TCCR2A &= ~(_BV(COM2B1));
  delayMicroseconds(564 * 8);

  // data
  uint8_t i = 32;
  while (i--) {
    //  for (uint8_t i = 0; i < 32; i++) {
    TCCR2A |= _BV(COM2B1);
    delayMicroseconds(564);
    TCCR2A &= ~(_BV(COM2B1));
    if (data & 0x80000000UL) {
      delayMicroseconds(564 * 3);
    } else {
      delayMicroseconds(564);
    }
    data <<= 1;
  }

  //stop bit
  TCCR2A |= _BV(COM2B1);
  delayMicroseconds(564);
  TCCR2A &= ~(_BV(COM2B1));
  //delayMicroseconds(564 * 3);

  // switch back to input to save power
  pinMode(PIN_IRTX, INPUT);

  // enable receiver again
  irRecvEnable(true);
}

void irSendSetup() {
}

