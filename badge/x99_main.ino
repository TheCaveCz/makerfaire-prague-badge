//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Setup and loop
//

void setup() {
#if LOG_ENABLED
  Serial.begin(115200);
#endif
  logInfo("Starting");

  powerSetup();
  pixelsSetup();
  ledsSetup();
  irSendSetup();
  irRecvSetup();
  storageSetup();

  // used for random delays when transmitting presence pulse
  randomSeed(storageMyId);

  // blink eyes at start, useful for programming and detecting resets
  // don't mess with leds anim functions, touch outputs directly
  digitalWrite(PIN_LED1, HIGH);
  digitalWrite(PIN_LED2, HIGH);
  delay(150);
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);
  delay(100);
  digitalWrite(PIN_LED1, HIGH);
  digitalWrite(PIN_LED2, HIGH);
  delay(150);
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);

  // we want to sleep immediately after everything is set up
  logicGoToSleep();
  logInfo("Ready");
}

void loop() {
  // after wakeup from sleep logicCheckSleep function will enable tasks needed
  logicCheckSleep();
  scheduler.execute();
}

