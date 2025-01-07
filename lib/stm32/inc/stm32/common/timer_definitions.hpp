#pragma once

namespace STM32::Timer
{
    // All timer`s interrupts
    enum class IRQFlags
    {
        UPDATE = TIM_DIER_UIE,
        CC1 = TIM_DIER_CC1IE,
        CC2 = TIM_DIER_CC2IE,
        CC3 = TIM_DIER_CC3IE,
        CC4 = TIM_DIER_CC4IE,
        COM = TIM_DIER_COMIE,
        TRIGGER = TIM_DIER_TIE,
        BREAK = TIM_DIER_BIE,
    };

    // All timer`s DMA requests
    enum class DMAFlags
    {
        UPDATE = TIM_DIER_UDE,
        CC1 = TIM_DIER_CC1DE,
        CC2 = TIM_DIER_CC2DE,
        CC3 = TIM_DIER_CC3DE,
        CC4 = TIM_DIER_CC4DE,
        TRIGGER = TIM_DIER_UDE,
    };
    
    // Timer counter mode
    enum class CounterMode
    {
        UP = 0x00000000U,
        DOWN = TIM_CR1_DIR,
        CenterAligned1 = TIM_CR1_CMS_0,///< center aligned, interrupt with counting down
        CenterAligned2 = TIM_CR1_CMS_1,///< center aligned, interrupt with counting up
        CenterAligned3 = TIM_CR1_CMS, ///< center aligned, interrupt with both
    };

    // Timer master mode
    enum class TriggerMode
    {
        RESET = 0x0 << TIM_CR2_MMS_Pos,///< Reset is used as TRGO
        ENABLE = 0x1 << TIM_CR2_MMS_Pos,///< Counter enable is used as TRGO
        UPDATE = 0x2 << TIM_CR2_MMS_Pos,///< Update event is used as TRGO
        ComparePulse = 0x3 << TIM_CR2_MMS_Pos,///< CC1F set is used as TRGO
        CompareCh1 = 0x4 << TIM_CR2_MMS_Pos,///< OC1REF signal is used as TRGO
        CompareCh2 = 0x5 << TIM_CR2_MMS_Pos,///< OC2REF signal is used as TRGO
        CompareCh3 = 0x6 << TIM_CR2_MMS_Pos,///< OC3REF signal is used as TRGO
        CompareCh4 = 0x7 << TIM_CR2_MMS_Pos,///< OC4REF signal is used as TRGO
    };

    class BasicTimer
    {
        static inline void enable();
        static inline void disable();
    };

    class GPTimer : public BasicTimer
    {
    private:
        template <uint8_t tNumber>
        class Channel
        {
        public:
            static inline void enable();
            static inline void disable();
            static inline bool hasIRQFlag();
            static inline void clrIRQFlag();
            static inline void attachIRQ();
            static inline void detachIRQ();
            static inline void attachDMA();
            static inline void detachDMA();
        };

    public:
        template <uint8_t tNumber>
        class ICapture : public Channel<tNumber>
        {
        public:
            static inline void configure();
        };

        template <uint8_t tNumber>
        class OCompare : public Channel<tNumber>;

        template <uint8_t tNumber>
        class PWMGeneration : public OCompare<tNumber>;

        class SlaveMode;
    };

    class AdvancedTimer : public GPTimer
    {};
}
