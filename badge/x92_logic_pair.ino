//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Handling the IR communication.
//

void logicPairSendPresenceCb();
void logicPairAwaitPresenceCb();
void logicPairSleepCb();

Task logicPairSendPresenceTask(1000, 4, &logicPairSendPresenceCb, &scheduler, false);
Task logicPairTask(100, TASK_FOREVER, &logicPairAwaitPresenceCb, &scheduler, false);

uint8_t logicPairPresenceSent;
uint8_t logicPairDumpPage; // current page of badge data we are dumping to master (there are 64 bytes in storage, so there are 32 pages - 2b per page)

// send three presence commands with random delay and then go to sleep
// this task effectively serves as timeout if no other badges are nearby
// it will be disabled once we get other device to talk to us
void logicPairSendPresenceCb() {
  logValue("Send presence task: ", logicPairSendPresenceTask.getRunCounter());
  if (logicPairSendPresenceTask.isFirstIteration()) {
    ledsSetAnim(ledsAnimPresence);
  }
  if (logicPairSendPresenceTask.isLastIteration()) {
    // if we timed out we must disable the other pairing task (so it will not interfere on next wake-up)
    logicPairTask.disable();
    logicGoToSleep();
    return;
  }

  irCommandSend(IR_CMD_PRESENCE, storageMyId, ~storageMyId);
  logicPairPresenceSent = 1;
  logicPairSendPresenceTask.setInterval(random(1600, 2000));
}

// callback for logicPairTask, simply prepares badge for sleep mode
// assumes logicPairSendPresenceTask is already disabled
void logicPairSleepCb() {
  logInfo("Pair sleep task triggered");
  logicPairTask.disable();
  logicGoToSleep();
}

// called as a final third step - we can store new ID and go to sleep afterwards
void logicPairBadgeConfirmed(uint16_t id) {
  // blink eyes differently if we've seen the badge before
  if (storageIdSeen(id)) {
    logValue("Confirmed prev seen badge ", id);
    ledsSetAnim(ledsAnimDuplicate);
  } else {
    logValue("Confirmed new badge ", id);
    ledsSetAnim(ledsAnimAcknowledge);
    storageMarkIdSeen(id);
  }
  // show the other ID
  pixelsShowId(id);

  // light LED on the back to indicate pairing is complete
  ledsSetBuiltin(true);

  // and go to sleep after 3 secs
  logicPairTask.setInterval(3000);
  logicPairTask.setCallback(&logicPairSleepCb);
}

// second stage of pairing logic
// we received presence command from other badge and sent our acknowledge
// now we wait for confirmation
void logicPairAwaitConfirmCb() {
  if (logicPairTask.getRunCounter() > 30) { // timeout in 3 sec
    logInfo("Timeout waiting for confirmation");
    logicPairSleepCb();
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_CONFIRM) {
    // id2 contains our ID
    if (c.fields.id2 != storageMyId) {
      logInfo("This confirm is not ours");
      return;
    }
    // id1 contains valid ID of the other badge
    // TODO : it would be nice to verify if this is actually the one we got in presence command
    if (!storageIdValid(c.fields.id1)) {
      logInfo("Invalid badge id");
      return;
    }

    // Well, we have a match. This function will handle going to sleep after some time.
    logicPairBadgeConfirmed(c.fields.id1);
  }
}

// second stage of pairing logic
// we received dump command so now we are sending all the IDs we've seen
void logicPairDumpCb() {
  ledsSetBuiltin(true);
  if (logicPairTask.getRunCounter() >= 30) { // timeout in 3 sec
    logInfo("Timeout waiting for dump confirmation");
    logicPairSleepCb();
    return;
  }

  // Send dump data and wait for confirmation for 1 sec, if we have no confirmation, repeat two other times and then go to sleep.
  // We need to transmit 64 bytes in total. We can fit two bytes in a command, so this makes for 32 pages (2byte page size).
  // Since we need to stuff two bytes of data and also a 5 bit page id -> we need to use special value of 0b100000 for IR_CMD_DUMP and OR it with page id
  if ((logicPairTask.getRunCounter() % 10) == 1) {
    logValue("Dumping page ", logicPairDumpPage);
    irCommandSend(IR_CMD_DUMP | logicPairDumpPage, storageSeenIds[logicPairDumpPage << 1], storageSeenIds[(logicPairDumpPage << 1) + 1]);
    return;
  }

  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_DUMP_ACK) {
    // our dump was acknowledged
    // id1 must contain our ID
    if (c.fields.id1 != storageMyId) {
      logInfo("This dump ack is not ours");
      return;
    }
    // and id2 must contain our page
    if (c.fields.id2 != logicPairDumpPage) {
      logInfo("Dump ack for invalid page");
      return;
    }

    // increase page counter
    logicPairDumpPage++;
    // blink with LED
    ledsSetBuiltin(false);
    if (logicPairDumpPage < STORAGE_COUNT / 2) {
      // we still have some pages left
      logicPairTask.enable(); // reset run counter for next page
    } else {
      // 32 pages are out and acknowledged -> we are done, go to sleep after dumping
      logInfo("Dumping finished");
      logicPairSleepCb();
    }
  }
}

// first stage of pairing logic
// awaits either foreign presence command or response to our presence command
void logicPairAwaitPresenceCb() {
  IrCommand c;
  if (!irCommandReceived(c)) return;

  if (c.fields.cmd == IR_CMD_PRESENCE) {
    // we got presence command from other badge
    // validate and respond with acknowledge
    // then wait for IR_CMD_CONFIRM command

    if (c.fields.id1 != (~c.fields.id2 & 0x1ff)) {
      logInfo("Invalid presence ID");
      return;
    }
    if (c.fields.id1 == storageMyId) {
      logInfo("Got own id on presence, ignoring");
      return;
    }

    logicPairSendPresenceTask.disable(); // stop sending our presence commands
    irCommandSend(IR_CMD_ACKNOWLEDGE, storageMyId, c.fields.id1);

    logicPairTask.enable(); // this resets the run counter - needed for timeout on next callback
    logicPairTask.setCallback(&logicPairAwaitConfirmCb);

  } else if (c.fields.cmd == IR_CMD_ACKNOWLEDGE) {
    // we got acknowledge command from other badge (which received our presence command)
    // send confirm command and mark the other badge as seen

    // id2 must contain our ID
    if (c.fields.id2 != storageMyId) {
      logInfo("This ack is not ours");
      return;
    }
    // id1 must contain valid badge ID
    if (!storageIdValid(c.fields.id1)) {
      logInfo("Invalid badge id");
      return;
    }
    if (!logicPairPresenceSent) {
      logInfo("Got ack without presence");
      return;
    }

    logicPairSendPresenceTask.disable(); // stop sending presence commands
    irCommandSend(IR_CMD_CONFIRM, storageMyId, c.fields.id1);
    logicPairBadgeConfirmed(c.fields.id1); // this will handle going to sleep after some time

  } else if (c.fields.cmd == IR_CMD_DUMP_REQUEST) {
    // We got dump command from badge printer
    if (!logicPairPresenceSent) {
      logInfo("Got dump without presence");
      return;
    }
    // id1 must contain our ID
    if (c.fields.id1 != storageMyId) {
      logInfo("This dump is not ours");
      return;
    }

    // enter dump mode
    ledsSetAnim(ledsAnimDump);
    logicPairSendPresenceTask.disable(); // stop sending presence commands
    logicPairDumpPage = 0;

    logicPairTask.setCallback(&logicPairDumpCb);
    logicPairTask.enable(); // reset run counter
  }
}

void logicStartPair() {
  // blink the eyes
  ledsSetAnim(ledsAnimWakeupShow);

  // prepare IR receiver - discard any old commands
  irRecvCommandsClear();
  logicPairPresenceSent = 0;

  // start sending presence commands in 2.5sec and watch for timing out when nothing gets received
  logicPairSendPresenceTask.restartDelayed(2500);

  // watch for presence commands from other badges
  // 100ms interval is enough because 32 bits of IR command takes approx 60-80ms to transmit
  logicPairTask.setCallback(&logicPairAwaitPresenceCb);
  logicPairTask.setInterval(100);
  logicPairTask.restart();
}
