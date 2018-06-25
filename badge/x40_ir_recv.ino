//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Handling of infrared receiver. This code is using interrupts and is very timing sensitive.
//

#define IR_RECV_MAX_CMDS 8
#define IR_RECV_CMDS_MASK 0b111

#define IR_STATE_DISABLED 0
#define IR_STATE_H0 1
#define IR_STATE_D1 2
#define IR_STATE_D0 3
#define IR_STATE_MATCH 4
#define IR_STATE_INVALID 5

volatile uint32_t irRecvLastTime;
volatile uint8_t irRecvState;
volatile uint32_t irRecvIsrData;
volatile uint8_t irRecvIsrCount;

// circular buffer for received commands
uint32_t irRecvCommands[IR_RECV_MAX_CMDS];
volatile uint8_t irRecvCommandsEnd;
volatile uint8_t irRecvCommandsBegin;



void irRecvIsr() {
  if (irRecvState == IR_STATE_DISABLED) return;

  uint32_t t = micros();
  // Since we are decoding NEC IR protocol we can affort to scale the values down to multiples of 128 microseconds.
  // NEC protocol is based on 562.5usec intervals so this gives us enough precision and tolerance to decode it.
  uint8_t delta = (t - irRecvLastTime) >> 7;

  // Normally sensor is active low (when receiving carrier freq. it outputs low) so the actual received data are inverted.
  // But we always measure time since change and this means we need to invert the value again to check previous state.
  // No need for double invert, we will just use original value and we are ok.
  // In IR code we commonly use word MARK for 1, SPACE for 0
  if (digitalRead(PIN_IRRX)) {
    // Set high bit if the delta time was for received mark - this simplifies comparisons later.
    delta |= 0b10000000;
  } else {
    delta &= 0b01111111;
  }

  irRecvLastTime = t;

  if (delta >= (128 | 65)  && delta <= (128 | 80)) {
    // mark for 9ms => this might be header
    irRecvState = IR_STATE_H0;

  } else if (irRecvState == IR_STATE_H0 && delta >= 30 && delta <= 40) {
    // space for 4.5ms => this is definitely header => reset state and prepare for decoding
    irRecvState = IR_STATE_D1;
    irRecvIsrData = 0;
    irRecvIsrCount = 0;

  } else if (irRecvState == IR_STATE_D1 && delta >= (128 | 3) && delta <= (128 | 5)) {
    // mark for 562usec

    if (irRecvIsrCount == 32) {
      // if we have enough bits this is stop bit => finish decoding
      irRecvState = IR_STATE_INVALID;
      irRecvCommands[irRecvCommandsBegin++] = irRecvIsrData;
      irRecvCommandsBegin &= IR_RECV_CMDS_MASK;
      if (irRecvCommandsBegin == irRecvCommandsEnd) {
        irRecvCommandsEnd++;
        irRecvCommandsEnd &= IR_RECV_CMDS_MASK;
      }

    } else {
      // not enough bits yet? increase bit counter and expect more
      irRecvState = IR_STATE_D0;
      irRecvIsrCount++;
    }

  } else if (irRecvState == IR_STATE_D0 && delta >= 11 && delta <= 15) {
    // space for 1687usec => this bit is ONE
    irRecvState = IR_STATE_D1;
    irRecvIsrData <<= 1;
    irRecvIsrData |= 1;

  } else if (irRecvState == IR_STATE_D0 && delta >= 3 && delta <= 5) {
    // space for 562usec => this bit is ZERO
    irRecvState = IR_STATE_D1;
    irRecvIsrData <<= 1;

  } else {
    // jump into invalid state if there is no match for current state and delta
    // only header mark can pull out of invalid state
    irRecvState = IR_STATE_INVALID;
  }

}

uint32_t irRecvGetCommand() {
  uint32_t result = 0;
  noInterrupts();
  if (irRecvCommandsBegin != irRecvCommandsEnd) {
    result = irRecvCommands[irRecvCommandsEnd];
    irRecvCommandsEnd++;
    irRecvCommandsEnd &= IR_RECV_CMDS_MASK;
  }
  interrupts();
  return result;
}


void irRecvCommandsClear() {
  noInterrupts();
  irRecvCommandsEnd = irRecvCommandsBegin;
  irRecvState = IR_STATE_INVALID;
  interrupts();
}

void irRecvEnable(const bool e) {
  irRecvState = e ? IR_STATE_INVALID : IR_STATE_DISABLED;
}

void irRecvPower(const bool on) {
  logValue("+irRecvPower ", on ? 1 : 0);
  if (on) {
    pinMode(PIN_IRRX_EN, OUTPUT);
    digitalWrite(PIN_IRRX_EN, HIGH);
  } else {
    detachInterrupt(digitalPinToInterrupt(PIN_IRRX));
    pinMode(PIN_IRRX_EN, INPUT);
  }
  delay(10); // we need to wait before decoder module starts after shutdown
  if (on) {
    irRecvState = IR_STATE_INVALID;
    irRecvLastTime = micros();
    attachInterrupt(digitalPinToInterrupt(PIN_IRRX), irRecvIsr, CHANGE);
  }
}

void irRecvSetup() {
  pinMode(PIN_IRRX, INPUT);
  memset(irRecvCommands, 0, sizeof(irRecvCommands));
  irRecvCommandsEnd = 0;
  irRecvCommandsBegin = 0;

  irRecvPower(true);
}


