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
    enum class MasterMode
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

    template <uint32_t tRegsAddr, IRQn_Type tIRQn, typename tClock>
    class BasicTimer
    {
        //config: prescaller, period, one-pulse(?)
        static inline void configure();
static inline void setPrescaller(uint16_t);
static inline void setAutoReload(uint16_t);
        static inline void enable();
        static inline void disable();
        static inline void start();
        static inline void stop();
        static inline void setValue(uint16_t);
        static inline uint16_t getValue();
        static inline void attachIRQ(IRQFlags flags);
        static inline void detachIRQ(IRQFlags flags);
        static inline void attachDMARequest();
        static inline void detachDMARequest();
    };

    template <uint32_t tRegsAddr, IRQn_Type tIRQn, typename tClock>
    class GPTimer : public BasicTimer<tRegsAddr, tIRQn, tClock>
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
            // Capture polarity
            enum class Polarity
            {
                RISING = 0,
                FALLING = TIM_CCER_CC1P,
                BOTH = TIM_CCER_CC1P | TIM_CCER_CC1NP,
            };
                
            // Capture mode
            enum class Mode
            {
                DIRECT = TIM_CCMR1_CC1S_0,
                INDIRECT = TIM_CCMR1_CC1S_1,
                TRC = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC1S_1,
            };

            static inline void configure();
        };

        template <uint8_t tNumber>
        class OCompare : public Channel<tNumber>
        {
        public:
            // Output polarity
            enum Polarity
            {
                HIGH = 0,
                LOW  = TIM_CCER_CC1P,
            };

            // Output mode
            enum Mode
            {
                TIMING          = 0,
                ACTIVE          = TIM_CCMR1_OC1M_0,
                INACTIVE        = TIM_CCMR1_OC1M_1,
                TOGGLE          = TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1,
                PWM1            = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2,
                PWM2            = TIM_CCMR1_OC1M,
                FORCED_ACTIVE   = TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2,
                FORCED_INACTIVE = TIM_CCMR1_OC1M_2,
            };

            static inline void configure();
        };

        template <uint8_t tNumber>
        class PWMGeneration : public OCompare<tNumber>
        {
        public:
            // PWM Fast mod
            enum class FastMode
            {
                DISABLE = 0x00000000U,
                ENABLE  = TIM_CCMR1_OC1FE,
            };

            static inline void configure();
        };

        class SlaveMode;
    };

    template <uint32_t tRegsAddr, IRQn_Type tIRQn, typename tClock>
    class AdvancedTimer : public GPTimer<tRegsAddr, tIRQn, tClock>
    {
    public:
        static inline void setRepetitionCounter(uint8_t counter);
        static inline uint8_t getRepetitionCounter();
    };
}
