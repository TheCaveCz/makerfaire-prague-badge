//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Storing seen IDs.
//

#define STORAGE_MAGIC 0x5a
#define STORAGE_MAGIC2 43
#define STORAGE_COUNT 64
#define STORAGE_MAX_ID 512
#define STORAGE_EEPROM_BASE 3

// each ID is up to 9 bits in size. If we store only the seen/not-seen info (1bit), this needs 512 bits => 64 bytes in EEPROM
uint8_t storageSeenIds[STORAGE_COUNT];
uint16_t storageSeenCount;
uint16_t storageMyId;

uint16_t storageCalcSeenCount() {
  uint16_t result = 0;
  for (uint8_t i = 0; i < STORAGE_COUNT; i++) {
    uint8_t b = storageSeenIds[i];
    // count number of set bits in b
    for (uint8_t j = 0; j < 8; j++) {
      if (b & (1 << j)) result++;
    }
  }
  return result;
}

// ID is only valid when any of 3bit pairs of the ID is not zero
bool storageIdValid(uint16_t id) {
  return ((id & 0b111) != 0) &&
         ((id & 0b111000) != 0) &&
         ((id & 0b111000000) != 0) &&
         (id < STORAGE_MAX_ID);
}

bool storageIdSeen(uint16_t id) {
  if (id >= STORAGE_MAX_ID) return 0;

  return storageSeenIds[id >> 3] & (1 << (id & 0b111)) ? 1 : 0;
}

void storageDebugDump() {
#if LOG_ENABLED
  logInfo("Dumping storage");
  logValue("Seen ids count: ", storageSeenCount);
  for (uint8_t i = 0; i < STORAGE_COUNT; i++) {
    if (storageSeenIds[i] < 16) logRaw('0');
    logNum(storageSeenIds[i], HEX);
    if (i % 8 == 7) {
      logLine();
    } else {
      logRaw(' ');
    }
  }
  logLine();
//  logInfo("Seen ids list");
//  for (uint16_t id = 0; id < STORAGE_MAX_ID; id++) {
//    if (storageIdSeen(id)) {
//      logRaw(id); logRaw(',');
//    }
//  }
//  logLine();
//  logLine();
#endif
}


void storageMarkIdSeen(uint16_t id) {
  logValue("Marking id seen: ", id);

  if (id >= STORAGE_MAX_ID) return;

  uint8_t offset = id >> 3;
  storageSeenIds[offset] |= (1 << (id & 0b111));
  EEPROM.write(STORAGE_EEPROM_BASE + offset, storageSeenIds[offset]);

  storageSeenCount = storageCalcSeenCount();
  storageDebugDump();
}

void storageSetup() {
  EEPROM.get(0, storageMyId);
  logValue("My ID ", storageMyId);
  if (!storageIdValid(storageMyId)) {
    logInfo("Oh-noes, invalid own ID");
  }

  uint8_t magic = EEPROM.read(2);
  uint8_t magic2 = EEPROM.read(128);
  if (magic == STORAGE_MAGIC && magic2 == STORAGE_MAGIC2) {
    logInfo("Storage loaded");

    EEPROM.get(STORAGE_EEPROM_BASE, storageSeenIds);
  } else {
    logInfo("Storage failed, formatting");

    memset(storageSeenIds, 0, sizeof(storageSeenIds));
    EEPROM.put(STORAGE_EEPROM_BASE, storageSeenIds);
    EEPROM.write(2, STORAGE_MAGIC);
    EEPROM.write(128, STORAGE_MAGIC2);
  }

  storageSeenCount = storageCalcSeenCount();
  storageDebugDump();
}

