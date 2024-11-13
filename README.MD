### Recommended Memory Partitions

STM32 F103x (page size = 1kb), eeprom uses twice memory

| Partition | Size                  | Example           |
| ---       | ---                   | ---               |
| boot      | N * page              | 6kb = 6 * page    |
| eeprom    | 2 * page              | 2kb = 2 * page    |
| app       | total - boot - eeprom | 56kb = 64 - 6 - 2 |

STM32 F411x (page size = 16kb), eeprom uses twice memory

| Partition | Size                  | Example               |
| ---       | ---                   | ---                   |
| boot      | N * page              | 16kb = 1 * page       |
| eeprom    | 2 * page              | 32kb = 2 * page       |
| app       | total - boot - eeprom | 464kb = 512 - 16 - 32 |

Recommended use emulated eeprom between boot and app partitions


---
cmsis:
RAM:   [          ]   0.5% (used 100 bytes from 20480 bytes)
Flash: [=         ]   5.5% (used 448 bytes from 8192 bytes)

RAM:   [          ]   0.5% (used 100 bytes from 20480 bytes)
Flash: [          ]   4.5% (used 372 bytes from 8192 bytes)
