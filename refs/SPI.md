# Transfer handling:

### Events:
```cpp
enum class SPI_Event {
    TX_BYTE_COMPLETED,
    TX_DATA_COMPLETED,
    RX_BYTE_COMPLETED,
    RX_DATA_COMPLETED,
    TX_RX_DATA_COMPLETED,
    ABORT,
    ERROR,
};
// maybe better add separate weak handlers for skip check pointers in ISR???
typedef void(*SPI_EventHandler)(SPI_Event event, uint16_t index, uint8_t data);
```
TODO place event triggering in flow...

### Bidirectional:
1. Enable SPI by set SPE=1
2. Write byte to DR
3. Repeat until rx data length == N-1
  1. Wait until TXE==1, trigger `TX_BYTE_COMPLETED` and write byte to DR
  2. Wait until RXNE==1 and read byte from DR, trigger `RX_BYTE_COMPLETED`
4. Wait until RXNE==1 and read last byte
5. Wait until TXE==1
6. Wait until BSY==0, trigger `TX_RX_DATA_COMPLETED`
7. Disable SPI by set SPE=0

### Transmit only:
1. Enable SPI by set SPE=1
2. Write byte into DR
3. Wait until TXE==1, trigger `TX_BYTE_COMPLETED`, if has more data - goto step 2
4. Wait until BSY==0, trigger `TX_DATA_COMPLETED`
5. Disable SPI by set SPE=0

### Receive only:
1. Enable SPI by set SPE=1
2. Wait until RXNE=1
3. Read byte from DR, trigger `RX_BYTE_COMPLETED`, if waits more data - goto step 2
4. Disable SPI by set SPE=0

