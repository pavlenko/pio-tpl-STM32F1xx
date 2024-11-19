#ifndef __ATOMICBLOCK_H__
#define __ATOMICBLOCK_H__

#include <stdint.h>
#include <cmsis_compiler.h>

class AtomicBlock
{
private:
    uint32_t mask;
public:
    inline AtomicBlock(): mask(__get_PRIMASK())
    {
        __disable_irq();
    }
    inline ~AtomicBlock()
    {
        __set_PRIMASK(mask);
    }
};

#endif // __ATOMICBLOCK_H__