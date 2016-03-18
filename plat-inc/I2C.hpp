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
        /** List of available I2C bus drivers */
	    enum I2C_Device{
            /** I2C bus driver 1 */
            I2C_1,
            /** I2C bus driver 2 */
            I2C_2,
            /** I2C bus driver 3 */
            I2C_3
	    };
        /** Direction of the bus transaction */
        enum Direction
        {
            /** Write to the target device (master transmitter mode) */
            Write,
            /** Read from the target device (master receiver mode) */
            Read
        };
        /** Constructor
         * 
         * @param scl Gpio to which the SCL line is connected.
         * @param sda Gpio to which the SDA line is connected.
         * @param d I2C device ID
         * */
	    I2C(Gpio& scl, Gpio& sda, I2C_Device d);
        /**
         * Set the I2C interface to work with 7 bit adresses
         * */
	    void set7bitMode();
        /** Sets the I2C interface own address. Not used in master mode */
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
        /** @name Synchronous I2C operations 
         * @{
         * */
        /** Write to the target address.
         * 
         * This function uses active wait and will block until completion. For an ansynchronous
         * version please use write_async instead.
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer to be tranfered
         * @param data_count number of bytes to be transfered
         * */
        void write(uint8_t address, uint8_t* data, int data_count);
        /**
         * Read from the target address
         * 
         * This function uses active wait and will block until completion. For an ansynchronous
         * version please use read_async instead.
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer where data will be written into
         * @param data_count number of bytes to be transfered
         * */
        void read(uint8_t address, uint8_t* data, int data_count);
        /** @} */
        /** @name Asynchronous I2Coperations 
         * @{
         * */
        /** 
         * Definition of the standard function call for write callbacks.
         * 
         * This is a void(void) prototype function.
         * 
         * This function will be called from an interrupt context; make sure
         * that it does not perform any long or blocking operations.
         * */
        typedef std::tr1::function<void(void)> WriteCompletionCallback;
        /** Writes data into the target chip.
         * 
         * This function returns immediately and will write data in the background
         * using interrupts. No notification of completion is given.
         * 
         * The buffer is allocated and deallocated by the caller, and must remain valid for the
         * entire life of the transaction
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer where data will be written into
         * @param data_count number of bytes to be transfered
         * */
        void write_async(uint8_t address, uint8_t* data, int data_count);
        /** Writes data into the target chip.
         * 
         * This function returns immediately and will write data in the background
         * using interrupts. Notification of completion is given when the callback cb
         * is called from an interrupt context.
         * 
         * The buffer is allocated and deallocated by the caller, and must remain valid for the
         * entire life of the transaction
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer where data will be written into
         * @param data_count number of bytes to be transfered
         * @param cb completion callback. Note this callback will be called from an
         * interrupt context.
         * */
        void write_async(uint8_t address, uint8_t* data, int data_count, WriteCompletionCallback cb);
        /** 
         * Definition of the standard function call for read callbacks. The read buffer will
         * be provided as argument to the function call.
         * 
         * This is a void(uint8_t* data, int data_count) prototype function.
         * 
         * This function will be called from an interrupt context; make sure
         * that it does not perform any long or blocking operations.
         * */
        typedef std::tr1::function<void(uint8_t*,int)> ReadCompletionCallback;
        /**
         * Read from the target address
         * 
         * This function returns immediately and will read data in the background
         * using interrupts. No notification of completion is given.

         * The buffer is allocated and deallocated by the caller, and must remain valid for the
         * entire life of the transaction
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer where data will be written into
         * @param data_count number of bytes to be transfered
         * */
        
        void read_async(uint8_t address, uint8_t* data, int data_count);
        /**
         * Read from the target address
         * 
         * This function returns immediately and will read data in the background
         * using interrupts. Notification of completion is given when the callback cb
         * is called from an interrupt context.

         * The buffer is allocated and deallocated by the caller, and must remain valid for the
         * entire life of the transaction
         * 
         * @param address target chip address. Only the lower 7 bits will be taken into account.
         * @param data databuffer where data will be written into
         * @param data_count number of bytes to be transfered
         * @param cb completion callback. Note this callback will be called from an
         * interrupt context.
         * */
        void read_async(uint8_t address, uint8_t* data, int data_count, ReadCompletionCallback cb);
        /** @} */
        /** Interrupt handler for events interrupts */
	    void event();
        /** Interrupt handler for error interrupts */
	    void error();
	private:
	    volatile I2C_TypeDef* m_base;
	    bool _StartBitIsSet();
	    bool _AddrBitIsSet();
	    bool _TransmitModeSet();
	    bool _TxEBitSet();
	    bool _ReceiveModeSet();
	    bool _RxNEBitSet();
	    void start_async(uint8_t address, uint8_t* data, int data_count, Direction arg4);
	    void setAckEnable();
	    void clearAckEnable();
	    void callReadCompletionCallback();
	    void callWriteCompletionCallback();
	    void disableInterrupts();
	    void enableInterrupts();
        /** State of the transmission */
        enum ProtocolState
        {
            /** Bus is being used in synchronous mode - either a read() or a write() */
            Synchronous,
            /** Bus is not being used */
            Idle,
            /** Start signal has been sent, we wait for its (internal) acknoledgement */
            Started,
            /** Adress (7bit) has been sent, we wait for ack to send or receive the data */
            AddressSent,
            /** Data is being transmitted */
            DataTransmission
        };
        /** Direction of the current asynchronous transfer */
        Direction m_activeDirection;
        /** Data buffer being currently transfered */
        uint8_t* m_data;
        /** Position of the next byte to transfer within the data buffer */
        int m_data_position;
        /** State of the current asynchronous transfer */
        ProtocolState m_activeState;
	    int m_data_count;
	    uint8_t m_address;
        ReadCompletionCallback m_read_cb;
        WriteCompletionCallback m_write_cb;
	    protected:
        /** Generate a Start sequence */
	    void generateStart();
        /** GEnerate a stop sequence */
	    void generateStop();
        /** Writes the target address on the bus
         * 
         * @param address target address; only the 7 lowest bits are employed
         * @param direction direction of operation
         * */
	    void setAddress(uint8_t address, Direction direction);
    };
}

#endif /* I2C_H */
