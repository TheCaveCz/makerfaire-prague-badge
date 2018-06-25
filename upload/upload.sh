#!/bin/bash

echo `head -n 1 badges.txt`
EEPROM=`head -n 1 badges.txt | awk '{ print $3 }'`

avrdude -q -p atmega328p -c usbasp -P usb -e -U lock:w:0x3F:m -U efuse:w:0xFE:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -U flash:w:badge.ino.with_bootloader.eightanaloginputs.hex:i -U eeprom:w:$EEPROM:m -U lock:w:0x0F:m 
STATUS=$?
if [ $STATUS -ne 0 ]; then
  echo "Avrdude failed"
  exit 1
fi

echo `head -n 1 badges.txt`

tail -n +2 badges.txt > badges.tmp
mv badges.tmp badges.txt
