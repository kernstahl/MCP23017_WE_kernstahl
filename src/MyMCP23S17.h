/*****************************************
This is a library for the MCP23017/MCP23S17/MCP23018/MCP23S18 I/O Port Expander.

You'll find some examples including the wiring which should enable you to use the library. 
Furthermore I have added a list of commands.

LICENSE: MIT (https://github.com/wollewald/MCP23017_WE/blob/master/LICENSE)

In case you like it, it would be cool if you give it a star.

If you find bugs, don't hesitate to raise an issue!

Written by Wolfgang (Wolle) Ewald
https://wolles-elektronikkiste.de/en/port-expander-mcp23017-2 (English)
https://wolles-elektronikkiste.de/portexpander-mcp23017       (German)

*******************************************/


#pragma once

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include "MyMCP23S17_config.h"
#include <SPI.h>

typedef enum MCP_PORT {A, B} mcp_port;
typedef enum MCP_ENABLE {OFF, ON} mcp_enable;

class MyMCP23S17{

    public:

        /* Registers */
        static constexpr uint8_t IODIRA  {0x00};  
        static constexpr uint8_t IODIRB  {0x01}; 
        static constexpr uint8_t IOCONA  {0x0A}; 
        static constexpr uint8_t IOCONB  {0x0B};
        static constexpr uint8_t INTCAPA {0x10};
        static constexpr uint8_t INTCAPB {0x11};
        static constexpr uint8_t INTCONA {0x08};
        static constexpr uint8_t INTCONB {0x09};
        static constexpr uint8_t INTFA   {0x0E};  
        static constexpr uint8_t INTFB   {0x0F};
        static constexpr uint8_t GPINTENA{0x04};
        static constexpr uint8_t GPINTENB{0x05};
        static constexpr uint8_t DEFVALA {0x06};
        static constexpr uint8_t DEFVALB {0x07};
        static constexpr uint8_t IPOLA   {0x02}; 
        static constexpr uint8_t GPIOA   {0x12};  
        static constexpr uint8_t GPIOB   {0x13};
        static constexpr uint8_t INTPOL  {0x01};  
        static constexpr uint8_t INTODR  {0x02};
        static constexpr uint8_t MIRROR  {0x06};  
        static constexpr uint8_t GPPUA   {0x0C};
        static constexpr uint8_t GPPUB   {0x0D};
        static constexpr uint8_t SPI_READ{0x01};

        static constexpr uint32_t SPI_CLOCKSPEED = 10000000;

        static constexpr uint8_t OPCODE_WRITE = 0b01000000;
        static constexpr uint8_t OPCODE_READ = 0b01000001;

        /* constructors */
        MyMCP23S17(SPIClass *s, uint8_t cs, uint8_t rp = 99) : _spi{s}, SPI_Address{0x20}, resetPin{rp}, csPin{cs} {}

        /* Public functions */
        bool Init();
        void reset(); 

        void setPinMode(uint8_t, mcp_port, uint8_t);
        void setPortMode(uint8_t, mcp_port);
        void setPortMode(uint8_t val, mcp_port, uint8_t pu);

        void setPin(uint8_t pin, mcp_port port, uint8_t pinLevel, bool useTransaction = true);

        void setPinBatch(uint8_t pin, mcp_port port, uint8_t pinLevel) {
            setPin(pin, port, pinLevel, false);
        }

        void togglePin(uint8_t pin, mcp_port port, bool useTransaction = true);

        void togglePinBatch(uint8_t pin, mcp_port port) {
            togglePin(pin, port, false);
        }

        void setPinX(uint8_t, mcp_port, uint8_t, uint8_t); 

        void setAllPins(mcp_port port, uint8_t pinLevel, bool useTransaction = true); 

        void setAllPinsBatch(mcp_port port, uint8_t pinLevel) {
            setAllPins(port, pinLevel, false);
        }

        void setPort(uint8_t portLevel, mcp_port port, bool useTransaction = true);   

        void setPortBatch(uint8_t portLevel, mcp_port port) {
            setPort(portLevel, port, false);
        }

        void setPorts(uint8_t portLevelA, uint8_t portLevelB, bool useTransaction = true); 

        void setPortsBatch(uint8_t portLevelA, uint8_t portLevelB) {
            setPorts(portLevelA, portLevelB, false);
        }

        void setPortX(uint8_t, uint8_t, mcp_port); 
        void setInterruptPinPol(uint8_t); 
        void setIntOdr(uint8_t);  
        void setInterruptOnChangePin(uint8_t, mcp_port); 
        void setInterruptOnDefValDevPin(uint8_t, mcp_port, uint8_t);  
        void setInterruptOnChangePort(uint8_t, mcp_port); 
        void setInterruptOnDefValDevPort(uint8_t, mcp_port, uint8_t);
        void deleteAllInterruptsOnPort(mcp_port); 
        void setPinPullUp(uint8_t, mcp_port, uint8_t); 
        void setPortPullUp(uint8_t, mcp_port);
        uint8_t getPortPullUp(mcp_port);
        void setIntMirror(uint8_t);
        uint8_t getIntFlag(mcp_port);

        bool getPin(uint8_t pin, mcp_port port, bool useTransaction = true);

        bool getPinBatch(uint8_t pin, mcp_port port) {
            return getPin(pin, port, false);
        }
        
        uint8_t getPort(mcp_port, bool useTransaction = true);

        uint8_t getPortBatch(mcp_port port) {
            return getPort(port, false);
        }

        uint8_t getIntCap(mcp_port);
        void setSPIClockSpeed(unsigned long clock); 
        void softReset();

        void startBatch();
        void endBatch();

#ifdef DEBUG_MyMCP23S17  // see MyMCP23S17_config.h
        void printAllRegisters();
        void printBin(uint8_t val);
#endif
        
    protected:

        void setIoCon(uint8_t, mcp_port);
        uint8_t getIoCon(mcp_port);
        void setGpIntEn(uint8_t, mcp_port);
        uint8_t getGpIntEn(mcp_port);
        void setIntCon(uint8_t, mcp_port);
        uint8_t getIntCon(mcp_port);
        void setDefVal(uint8_t, mcp_port);
        uint8_t getDefVal(mcp_port);
        
        void write(uint8_t reg, uint8_t val, bool useTransaction = true);
        void write(uint8_t reg, uint8_t valA, uint8_t valB, bool useTransaction = true);
        uint8_t read(uint8_t reg, bool useTransaction = true);

        void setCsPinMode();
        void setCsPinLow();
        void setCsPinHigh();

        SPIClass *_spi;
        SPISettings mySPISettings;
        const uint8_t SPI_Address;
        const uint8_t resetPin;
        const uint8_t csPin;
        uint8_t ioDirA, ioDirB;
        uint8_t gpioA, gpioB;
};

