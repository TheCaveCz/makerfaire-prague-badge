//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Higher level handling of IR communication.
//


// all commands except IR_CMD_DUMP have higest bit set to 0
//0xA
#define IR_CMD_PRESENCE 0b001010
//0x15
#define IR_CMD_ACKNOWLEDGE 0b010101
//0x12
#define IR_CMD_CONFIRM 0b010010
//0x19
#define IR_CMD_DUMP_REQUEST 0b011001
//0x18
#define IR_CMD_DUMP_ACK 0b011000

//0x20 and more, this is actually a command with address
#define IR_CMD_DUMP 0b100000

union IrCommand {
  struct Fields {
    uint8_t cmd: 6;
    uint16_t id1: 9;
    uint16_t id2: 9;
    uint8_t crc;
  } fields;
  uint32_t number;
  uint8_t bytes[4];
};


// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 2x16 entry CRC table created by Arjen Lentz
// See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
static const uint8_t PROGMEM dscrc2x16_table[] = {
  0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
  0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
  0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
  0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (Use tiny 2x16 entry CRC table)
uint8_t crc8(const uint8_t *addr, uint8_t len) {
  uint8_t crc = 0;

  while (len--) {
    crc = *addr++ ^ crc;  // just re-using crc as intermediate
    crc = pgm_read_byte(dscrc2x16_table + (crc & 0x0f)) ^
          pgm_read_byte(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f));
  }

  return crc;
}

void irCommandDebug(union IrCommand& c, bool sent) {
#if LOG_ENABLED
  logHeader();
  logRaw(sent ? F("Sent IR command ") : F("Received IR command "));
  logNum(c.fields.cmd, HEX);
  logRaw(',');
  logRaw(c.fields.id1);
  logRaw(',');
  logRaw(c.fields.id2);
  logLine();
#endif
}

bool irCommandReceived(IrCommand& c) {
  c.number = irRecvGetCommand();
  if (c.number == 0) return false;

  irCommandDebug(c, false);
  return c.fields.crc == crc8(c.bytes, 3);
}

void irCommandSend(const uint8_t cmd, const uint16_t id1, const uint16_t id2) {
  IrCommand c;
  c.fields.cmd = cmd;
  c.fields.id1 = id1;
  c.fields.id2 = id2;
  c.fields.crc = crc8(c.bytes, 3);

  irCommandDebug(c, true);

  irSendData(c.number);
}
