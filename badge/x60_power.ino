//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Sleep mode management.
//

void powerSetup() {
  // disable ADC to save power
  ADCSRA &= ~(1 << ADEN);

  // enable Pin-On-Change interrupts
  PCMSK0 = _BV(PCINT1) | _BV(PCINT0);

  // enable pullups for wakeup buttons
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

uint8_t powerEnterSleep() {
  // disable all peripherals to save power
  pixelsPower(false);
  ledsPower(false);
  irRecvPower(false);

  logInfo("Going to sleep");
  logFlush();


  uint8_t res = 0;
  do {
    // prepare pin-change interrupts
    cli();
    PCIFR |= _BV(PCIF0);
    PCICR = _BV(PCIE0);

    // enable possibility to enter sleep mode
    sleep_enable();
    // disable BOD - needs to be done exactly before sleep
    sleep_bod_disable();
    // enable interrupts (on change interrupt) to wake the cpu
    sei();
    // go to sleep finally
    sleep_cpu();
    // disable possibility to enter sleep mode
    sleep_disable();
    // enable interrupts (just for sure)
    sei();

    // let things cool a little
    delay(1);
    // check button status
    if (digitalRead(PIN_SW1) == LOW) res |= 1;
    if (digitalRead(PIN_SW2) == LOW) res |= 2;
    // if by any chance no buttons are pressed, go to sleep again
  } while (res == 0);

  pixelsPower(true);
  ledsPower(true);
  irRecvPower(true);

  logValue("Wakeup reason ", res);

  return res;
}

// this needs to be here
// interrupt handler for pin-on-change interrupt
// only disables this interrupt
ISR (PCINT0_vect) {
  PCICR &= ~(_BV(PCIE0));
}

