#include "Dma.h"
#include <Log.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <Irq.h>

extern "C" {
    void DMA1_Stream0_IRQHandler();
    void DMA1_Stream1_IRQHandler();
    void DMA1_Stream2_IRQHandler();
    void DMA1_Stream3_IRQHandler();
    void DMA1_Stream4_IRQHandler();
    void DMA1_Stream5_IRQHandler();
    void DMA1_Stream6_IRQHandler();
    void DMA1_Stream7_IRQHandler();
    void DMA2_Stream0_IRQHandler();
    void DMA2_Stream1_IRQHandler();
    void DMA2_Stream2_IRQHandler();
    void DMA2_Stream3_IRQHandler();
    void DMA2_Stream4_IRQHandler();
    void DMA2_Stream5_IRQHandler();
    void DMA2_Stream6_IRQHandler();
    void DMA2_Stream7_IRQHandler();
};

using namespace Platform;
/** Collection of semaphores, one per stream */
xSemaphoreHandle dmaSem[8][2];

DmaStream::DmaStream(DMAController dmaId, Stream streamId, Channel channel): streamId(streamId), dmaId(dmaId)
{
    /** the valid DMA IDs are 1 and 2. We derefence with -1 to get back in bounds */
    vSemaphoreCreateBinary(dmaSem[dmaId-1][streamId]);
    xSemaphoreTake(dmaSem[dmaId-1][streamId], 0);
    /** Enable the power supply for the DMA controller */
    if(DMAController1 == dmaId) RCC->AHB1ENR |= 1 << 21;
    else if(DMAController2 == dmaId) RCC->AHB1ENR |= 1 << 22;
    else abort();

    if(DMAController1 == dmaId)
	dma = DMA1;
    else if(DMAController2 == dmaId)
	dma = DMA2;
    else abort();
    switch(streamId) {
	case S0: case S1: case S2: case S3:
	case S4: case S5: case S6: case S7:
	    /// @smell Berk pointer arithmetics
	    stream = (DMA_Stream_TypeDef*)( (char*)dma + 0x10 + 0x18 * streamId);
	    break;
	default:
	    abort();
	    break;
    }

    currentBuf = 0;
    //Set channel for this stream
    stream->CR &= ~DMA_SxCR_CHSEL;
    //channel &= 7;
    stream->CR |= channel << 25;

    Irq(irqNr()).enable();
    stream->CR |= DMA_SxCR_TCIE;
}

int DmaStream::irqNr()
{
    if(DMAController1 == dmaId) {
	if(streamId <= S6)
	    return 11 + streamId;
	if(S7 == streamId)
	    return DMA1_Stream7_IRQn;
	abort();
    } else if(DMAController2 == dmaId) {
	if(streamId <= S4)
	    return DMA2_Stream0_IRQn + streamId;
	if(streamId <= S7)
	    return DMA2_Stream5_IRQn + streamId-5;
	abort();
    } else abort();
    return 0;
}

DmaStream& DmaStream::setCurrent(bool first)
{
    if(first)
	stream->CR &= ~DMA_SxCR_CT;
    else
	stream->CR |= DMA_SxCR_CT;

    return *this;
}

DmaStream& DmaStream::peripheralFixed(bool fixed)
{
    if(!fixed)
	stream->CR |= DMA_SxCR_PINC;
    else
	stream->CR &= ~DMA_SxCR_PINC;

    return *this;
}

DmaStream& DmaStream::memoryFixed(bool fixed)
{
    if(!fixed)
	stream->CR |= DMA_SxCR_MINC;
    else
	stream->CR &= ~DMA_SxCR_MINC;

    return *this;
}

DmaStream& DmaStream::setDirection(direction d)
{
    stream->CR &= ~DMA_SxCR_DIR;
    switch(d) {
	case P2M:
	    stream->CR |= 0<<6;
	    break;
	case M2P:
	    stream->CR |= 1<<6;
	    break;
	case M2M:
	    stream->CR |= 2<<6;
	    break;
	default:
	    abort();
	    break;
    }

    return *this;
}

DmaStream& DmaStream::peripheralControlled(bool p)
{
    if(p)
	stream->CR |= DMA_SxCR_PFCTRL;
    else
	stream->CR &= ~DMA_SxCR_PFCTRL;

    return *this;
}

DmaStream& DmaStream::numberOfData(uint16_t n)
{
    n &= 0xffff;
    stream->NDTR = n;

    return *this;
}

DmaStream& DmaStream::enable()
{
    setCurrent(currentBuf == 0);
    /* clears the interrupt state */
    if(streamId < S4)
	if(streamId < S2)
	    dma->LIFCR = 0x1f << (6*streamId);
	else
	    dma->LIFCR = 0x1f << (6*streamId+4);
    else
	if(streamId <S6)
	    dma->HIFCR = 0x1f << (6*(streamId-4));
	else
	    dma->HIFCR = 0x1f << (6*(streamId-4)+4);
    /** Why ? */
    dma->HIFCR = 0xffffffff;
    dma->LIFCR = 0xffffffff;

    /** Enables the dma transfer */
    stream->CR |= DMA_SxCR_EN;

    return *this;
}

DmaStream& DmaStream::wait()
{
    if(!(stream->CR & DMA_SxCR_EN))
	return *this;
    xSemaphoreTake(dmaSem[dmaId-1][streamId], portMAX_DELAY);
    while(stream->CR & DMA_SxCR_EN);

    return *this;
}

DmaStream& DmaStream::setPeripheral(volatile void* p)
{
    stream->PAR = (uint32_t)p;
    return *this;
}

DmaStream& DmaStream::setMemory(volatile void* p)
{
    currentBuf = !currentBuf;
    if(currentBuf == 0)
	stream->M0AR = (uint32_t)p;
    else
	stream->M1AR = (uint32_t)p;

    return *this;
}

DmaStream& DmaStream::fifo(bool enabled)
{
    if(enabled)
	stream->FCR |= DMA_SxFCR_DMDIS;
    else
	stream->FCR &= ~DMA_SxFCR_DMDIS;

    return *this;
}

static void irq_handler(DMA_TypeDef *dmab, Platform::DmaStream::DMAController dma, Platform::DmaStream::Stream stream)
{
    long v;
    xSemaphoreGiveFromISR(dmaSem[dma-1][stream], &v);
    portEND_SWITCHING_ISR(v);
    dmab->HIFCR = 0xffffffff;
    dmab->LIFCR = 0xffffffff;
}

void DMA1_Stream0_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S0); }
void DMA1_Stream1_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S1); }
void DMA1_Stream2_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S2); }
void DMA1_Stream3_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S3); }
void DMA1_Stream4_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S4); }
void DMA1_Stream5_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S5); }
void DMA1_Stream6_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S6); }
void DMA1_Stream7_IRQHandler() { irq_handler(DMA1, Platform::DmaStream::DMAController1, Platform::DmaStream::S7); }

void DMA2_Stream0_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S0); }
void DMA2_Stream1_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S1); }
void DMA2_Stream2_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S2); }
void DMA2_Stream3_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S3); }
void DMA2_Stream4_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S4); }
void DMA2_Stream5_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S5); }
void DMA2_Stream6_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S6); }
void DMA2_Stream7_IRQHandler() { irq_handler(DMA2, Platform::DmaStream::DMAController2, Platform::DmaStream::S7); }
