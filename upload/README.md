## Programování 

### Fuses

Fuses jsou nastaveny podle následující tabulky. Jediný rozdíl oproti standardnímu Arduino Pro Mini je `Brown-Out Detect level`.
Je nastavený na hodnotu 1.8V místo 2.7V, protože CR2032 baterka má velký vnitřní odpor a při rychlých změnách barvy se děly brown-out resety. 

[Fuse calculator](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=FF&HIGH=DA&EXTENDED=FE&LOCKBIT=0F)

| fuse | hodnota |
| --- | --- |
| low | 0xFF |
| high | 0xDA |
| extended | 0xFE |
| lock | 0x0F |
