#pragma once

//todo
// write by page number 
// write buffer
namespace STM32
{
    /**
     * @brief Flash API 
     */
    class Flash
    {
    private:
        /**
         * @brief Wait while flash is busy
         */
        static inline void wait();

    public:
        /**
         * @brief Configure flash
         *
         * @param uint32_t System clock frequency
         */
        static inline void configure(uint32_t frequency);

        /**
         * @brief Get flash total size
         *
         * @return Size in bytes
         */
        static constexpr uint32_t getSize();

        /**
         * @brief Get page size
         *
         * @param page Page number
         *
         * @return Size in bytes
         */
        static constexpr uint32_t getPageSize(uint16_t page);

        /**
         * @brief Get page start address
         *
         * @param page Page number
         *
         * @return Address
         */
        static constexpr uint32_t getPageAddress(uint16_t page);

        /**
         * @brief Lock flash for writing
         */
        static inline void lock();

        /**
         * @brief Unlock flash for writing
         */
        static inline void unlock();

        /**
         * @brief Erase single page
         *
         * @param page Page number
         *
         * @return Success state
         */
        static inline bool erasePage(uint16_t page);

        /**
         * @brief Write data
         *
         * @param address Page address
         * @param data Data to write
         *
         * @return Success state
         * TODO 8/16/32/64 blocks mode
         */
        static inline bool write(uint32_t address, uint16_t data);
    };
}
