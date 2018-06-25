//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Showing all the seen IDs
//

void logicShowTaskCb();

Task logicShowTask(1000, TASK_FOREVER, &logicShowTaskCb, &scheduler, false);

uint16_t logicLastShownId;

void logicShowTaskCb() {
  logValue("Show task: ", logicShowTask.getRunCounter());

  if (logicShowTask.isLastIteration()) {
    logicGoToSleep();
    return;
  }

  // find next ID to show
  // id=0 is invalid so we are OK here
  do {
    logicLastShownId++;
    if (logicLastShownId >= STORAGE_MAX_ID) {
      logInfo("ID overflow when iterating!");
      logicLastShownId = 0;
      break;
    }
  } while (!storageIdSeen(logicLastShownId));

  // and show it
  pixelsShowId(logicLastShownId);
}

void logicStartShow() {
  // blink the eyes
  ledsSetAnim(ledsAnimWakeupShow);

  // show my id
  pixelsShowId(storageMyId);

  // and prepare for the show
  logicLastShownId = 0;
  uint16_t iter = storageSeenCount + 1; // +1 for waiting at the end (before going to sleep)
  logicShowTask.setIterations(iter);
  logicShowTask.setInterval(iter < 8 ? 1000 : 8000 / iter); // keep fixed show time to save battery for eager collectors
  logicShowTask.restartDelayed(2500); // time for eyes animation and showing my own ID
}

