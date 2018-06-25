//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Basic functions for badge behavior.
//

bool logicNeedSleep;

// forward declarations of functions, they are called out of Scheduler
void logicStartShow();
void logicStartPair();

// toggle flag for sleep mode - doesn't sleep immediately but on next scheduler cycle
void logicGoToSleep() {
  logicNeedSleep = true;
}

// go to sleep if requested and after wake-up start the right action
void logicCheckSleep() {
  if (!logicNeedSleep) return;

  uint8_t res = powerEnterSleep();
  logicNeedSleep = false;
  if (res & 1) { // pair button
    logicStartPair();
  } else { // show button
    logicStartShow();
  }
}

