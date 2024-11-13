#ifndef __STM32_COMMON_H__
#define __STM32_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Generic memory-mapped I/O accessor functions */
#define MMIO8(addr)  (*(volatile uint8_t *)(addr))
#define MMIO16(addr) (*(volatile uint16_t *)(addr))
#define MMIO32(addr) (*(volatile uint32_t *)(addr))
#define MMIO64(addr) (*(volatile uint64_t *)(addr))

#ifdef __cplusplus
}
#endif

#endif // __STM32_COMMON_H__