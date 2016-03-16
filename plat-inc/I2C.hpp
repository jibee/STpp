#ifndef I2C_H
#define I2C_H

#include "Gpio.h"
#include <tr1/functional>

namespace Platform
{
    /** interface definition - 3 implementations should eventually exist:
     * - Polling implementation (I2C_Polling)
     * - Interrupt implementation (I2C_Interrupt)
     * - DMA implementation (I2C_DMA)
     * Presumably DMA and interrupt should be merged into just one implementation
     */ 
    class I2C
    {
	public:
	    enum I2C_Device{
		I2C_1,
		I2C_2,
		I2C_3
	    };
        enum Direction
        {
            Write,
            Read
        };
	    I2C(Gpio& scl, Gpio& sda, I2C_Device d);
	    void set7bitMode();
	    void setOwnAddress(uint16_t addr);
        /** @name Low level I2C operations 
         * @{
         */
        /** Reads a byte on the bus */
	    uint8_t getData();
        /** Write a byte on the bus */
	    void setData(uint8_t data);
        /** Initiates a start condition */
        void start();
        /** Initiates a stop condition */
        void stop();
        /** @} */
        /** @name High level I2C operations 
         * @{
         */
        typedef std::tr1::function<void(void)> WriteCompletionCallback;
        void write(uint8_t address, uint8_t* data, int data_count);
        void write_async(uint8_t address, uint8_t* data, int data_count);
        void write_async(uint8_t address, uint8_t* data, int data_count, WriteCompletionCallback cb);
        typedef std::tr1::function<void(uint8_t*,int)> ReadCompletionCallback;
        void read(uint8_t address, uint8_t* data, int data_count);
        void read_async(uint8_t address, uint8_t* data, int data_count);
        void read_async(uint8_t address, uint8_t* data, int data_count, ReadCompletionCallback cb);
	private:
	    volatile I2C_TypeDef* m_base;
	    bool _StartBitIsSet();
	    bool _AddrBitIsSet();
	    bool _TransmitModeSet();
	    bool _TxEBitSet();
	    bool _ReceiveModeSet();
	    bool _RxNEBitSet();
	    protected:
	    void generateStart();
	    void generateStop();
	    void setAddress(uint8_t address, Direction Write);
    };
}

#endif /* I2C_H */
