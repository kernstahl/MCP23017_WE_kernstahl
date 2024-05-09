/*****************************************
This is a library for the MCP23017/MCP23S17/MCP23018/MCP23S18 I/O Port Expander

You'll find some examples including the wiring which should enable you to use the library. 
Furthermore I have added a list of commands.

You are free to use it, change it or build on it. In case you like it, it would be cool 
if you give it a star.

If you find bugs, please inform me!

Written by Wolfgang (Wolle) Ewald
https://wolles-elektronikkiste.de/en/port-expander-mcp23017-2 (English)
https://wolles-elektronikkiste.de/portexpander-mcp23017       (German)

*******************************************/

#include "MyMCP23S17.h"

bool MyMCP23S17::Init(){

#ifdef MyMCP23S17_USE_ESP32_REG_WRITE
    if (csPin > 31) {
        return false;
    }
#endif

#ifdef MyMCP23S17_USE_HW_CS
    _spi->setHwCs(true);
#endif

    setCsPinMode();
    setCsPinHigh();
    
    if(resetPin < 99){ 
        pinMode(resetPin, OUTPUT); 
        digitalWrite(resetPin, HIGH);
    }

    softReset();
    
    // setIoCon(0b00001000, A); // enable SPI hardware address
    // setIoCon(0b00001000, B); // enable SPI hardware address

    setIoCon(0b00000000, A); // disable SPI hardware address
    setIoCon(0b00000000, B); // disable SPI hardware address

    setIntCon(0b10101010, A);
    if(read(INTCONA) != 0b10101010){
        return false;
    }

    setIntCon(0, A);

    ioDirA = 0b00000000;
    ioDirB = 0b00000000;
    gpioA = 0b00000000;
    gpioB = 0b00000000;
    
    mySPISettings = SPISettings(SPI_CLOCKSPEED, MSBFIRST, SPI_MODE0); 

    return true;
};

void MyMCP23S17::reset(){
    digitalWrite(resetPin,LOW);
    delay(10);
    digitalWrite(resetPin, HIGH);
    delay(10);
}

void MyMCP23S17::setPinMode(uint8_t pin, mcp_port port, uint8_t pinState){
    uint8_t gppu = getPortPullUp(port);
    if(port==A){
        if(pinState==OUTPUT){
            ioDirA &= ~(1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT){
            ioDirA |= (1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT_PULLUP){
            ioDirA |= (1<<pin);
            gppu |= (1<<pin);
        }
        write(GPPUA, gppu);
        write(IODIRA, ioDirA); 
    }
    else if(port==B){
        if(pinState==OUTPUT){
            ioDirB &= ~(1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT){
            ioDirB |= (1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT_PULLUP){
            ioDirB |= (1<<pin);
            gppu |= (1<<pin);
        }
        write(GPPUB, gppu);
        write(IODIRB, ioDirB);
    }       
}

void MyMCP23S17::setPortMode(uint8_t portState, mcp_port port){
    if(port==A){
        ioDirA = ~portState;
        write(IODIRA, ioDirA);
        write(GPPUA, 0);
    }
    else if(port==B){
        ioDirB = ~portState;
        write(IODIRB, ioDirB);
        write(GPPUB, 0);
    }
}

void MyMCP23S17::setPortMode(uint8_t portState, mcp_port port, uint8_t pu){
    uint8_t gppu = getPortPullUp(port);
    if(pu != INPUT_PULLUP){
        return;
    }
    if(port==A){
        ioDirA = ~portState;
        gppu = ~portState;
        write(GPPUA, gppu);
        write(IODIRA, ioDirA); 
    }
    else if(port==B){
        ioDirB = ~portState;
        gppu = ~portState;
        write(GPPUB, gppu);
        write(IODIRB, ioDirB);
    }
}

void MyMCP23S17::setPin(uint8_t pin, mcp_port port, uint8_t pinLevel, bool useTransaction){

    if(port==A){
        if(pinLevel==HIGH){
            gpioA |= (1<<pin); 
        }
        else if(pinLevel==LOW){
            gpioA &= ~(1<<pin); 
        }
        write(GPIOA, gpioA, useTransaction);
    }

    if(port==B){
        if(pinLevel==HIGH){
            gpioB |= (1<<pin); 
        }
        else if(pinLevel==LOW){
            gpioB &= ~(1<<pin); 
        }
        write(GPIOB, gpioB, useTransaction);
    }
}

void MyMCP23S17::togglePin(uint8_t pin, mcp_port port, bool useTransaction){
    if(port==A){
        if(((gpioA) & (1<<pin))==0){
            gpioA |= (1<<pin); 
        }
        else if(((gpioA) & (1<<pin)) >= 1){
            gpioA &= ~(1<<pin); 
        }
        write(GPIOA, gpioA, useTransaction);
    }
    if(port==B){
        if(((gpioB) & (1<<pin))==0){
            gpioB |= (1<<pin); 
        }
        else if(((gpioB) & (1<<pin)) >= 1){
            gpioB &= ~(1<<pin); 
        }
        write(GPIOB, gpioB, useTransaction);
    }
}

void MyMCP23S17::setPinX(uint8_t pin, mcp_port port, uint8_t pinState, uint8_t pinLevel){
    uint8_t gppu = getPortPullUp(port);
    if(port==A){
        if(pinState==OUTPUT){
            ioDirA &= ~(1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT){
            ioDirA |= (1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT_PULLUP){
            ioDirA |= (1<<pin);
            gppu |= (1<<pin);
        }
        if(pinLevel==HIGH){
            gpioA |= (1<<pin); 
        }
        else if(pinLevel==LOW){
            gpioA &= ~(1<<pin); 
        }
        write(GPPUA, gppu);
        write(IODIRA, ioDirA);
        write(GPIOA, gpioA);
    }
    if(port==B){
        if(pinState==OUTPUT){
            ioDirB &= ~(1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT){
            ioDirB |= (1<<pin);
            gppu &= ~(1<<pin);
        }
        else if(pinState==INPUT_PULLUP){
            ioDirB |= (1<<pin);
            gppu |= (1<<pin);
        }
        if(pinLevel==HIGH){
            gpioB |= (1<<pin); 
        }
        else if(pinLevel==LOW){
            gpioB &= ~(1<<pin); 
        }
        write(GPPUB, gppu);
        write(IODIRB, ioDirB);
        write(GPIOB, gpioB);
    }
}

void MyMCP23S17::setAllPins(mcp_port port, uint8_t pinLevel, bool useTransaction){
    if(port==A){
        if(pinLevel==HIGH){
            gpioA = 0b11111111;
        }
        else if (pinLevel==LOW){
            gpioA = 0b00000000;
        }
        write(GPIOA, gpioA, useTransaction);
    }
    if(port==B){
        if(pinLevel==HIGH){
            gpioB = 0b11111111;
        }
        else if (pinLevel==LOW){
            gpioB = 0b00000000;
        }
        write(GPIOB, gpioB, useTransaction);
    }
}

void MyMCP23S17::setPort(uint8_t portLevel, mcp_port port, bool useTransaction){
    if(port==A){
        gpioA = portLevel;
        write(GPIOA, gpioA, useTransaction);
    }
    else if(port==B){
        gpioB = portLevel;
        write(GPIOB, gpioB, useTransaction);
    }
}

void MyMCP23S17::setPorts(uint8_t portLevelA, uint8_t portLevelB, bool useTransaction){
    gpioA = portLevelA;
    gpioB = portLevelB;
    write(GPIOA, gpioA, gpioB, useTransaction);
}

void MyMCP23S17::setPortX(uint8_t portState, uint8_t portLevel, mcp_port port){
    if(port==A){
        ioDirA = ~portState;
        gpioA = portLevel;
        write(IODIRA, ioDirA);
        write(GPIOA, gpioA);
    }
    else if(port==B){
        ioDirB = ~portState;
        gpioB = portLevel;
        write(IODIRB, ioDirB);
        write(GPIOB, gpioB);
    }
}

void MyMCP23S17::setInterruptPinPol(uint8_t level){
    uint8_t ioConA = getIoCon(A);
    uint8_t ioConB = getIoCon(B);
    if(level==HIGH){
        ioConA |= (1<<INTPOL);
        ioConB |= (1<<INTPOL);
    }
    if(level==LOW){
        ioConA &= ~(1<<INTPOL);
        ioConB &= ~(1<<INTPOL);
    }
    write(IOCONA, ioConA);
    write(IOCONB, ioConB);
}   

void MyMCP23S17::setIntOdr(uint8_t openDrain){
    uint8_t ioConA = getIoCon(A);
    uint8_t ioConB = getIoCon(B);
    if(openDrain){
        ioConA |= (1<<INTODR);
        ioConB |= (1<<INTODR);
    }
    else{
        ioConA &= ~(1<<INTODR);
        ioConB &= ~(1<<INTODR);
    }
    write(IOCONA, ioConA);
    write(IOCONB, ioConB);
}   

void MyMCP23S17::setInterruptOnChangePin(uint8_t pin, mcp_port port){
    uint8_t gpIntEn = getGpIntEn(port);
    if(port==A){
        ioDirA |= (1<<pin); 
        gpIntEn |= (1<<pin);
        write(IODIRA, ioDirA);
        write(GPIOA, gpioA);
        write(GPINTENA, gpIntEn);
    }
    else if (port==B){
        ioDirB |= (1<<pin); 
        gpIntEn |= (1<<pin);
        write(IODIRB, ioDirB);
        write(GPIOB, gpioB);
        write(GPINTENB, gpIntEn);
    }
}

void MyMCP23S17::setInterruptOnDefValDevPin(uint8_t pin, mcp_port port, uint8_t pinIntLevel){
    uint8_t gpIntEn = getGpIntEn(port);
    uint8_t intCon = getIntCon(port);
    uint8_t defVal = getDefVal(port);
    if(port==A){
        ioDirA |= (1<<pin); 
        gpIntEn |= (1<<pin);
        intCon |= (1<<pin);
        if(pinIntLevel==HIGH) defVal |= (1<<pin);
        else if(pinIntLevel==LOW) defVal &= ~(1<<pin);
        write(IODIRA, ioDirA);
        write(GPIOA, gpioA);
        write(GPINTENA, gpIntEn);
        write(INTCONA, intCon);
        write(DEFVALA, defVal);
    }
    else if (port==B){
        ioDirB |= (1<<pin); 
        gpIntEn |= (1<<pin);
        intCon |= (1<<pin);
        if(pinIntLevel==HIGH) defVal |= (1<<pin);
        else if(pinIntLevel==LOW) defVal &= ~(1<<pin);
        write(IODIRB, ioDirB);
        write(GPIOB, gpioB);
        write(GPINTENB, gpIntEn);
        write(INTCONB, intCon);
        write(DEFVALB, defVal);
    }
}

void MyMCP23S17::setInterruptOnChangePort(uint8_t intOnChangePins, mcp_port port){
    if(port==A){
        ioDirA |= intOnChangePins;
        write(IODIRA, ioDirA);
        write(GPINTENA, intOnChangePins);
    }
    else if (port==B){
        ioDirB |= intOnChangePins;
        write(IODIRB, ioDirB);
        write(GPINTENB, intOnChangePins);
    }
}

void MyMCP23S17::setInterruptOnDefValDevPort(uint8_t intPins, mcp_port port, uint8_t defVal){
    uint8_t gpIntEn = getGpIntEn(port);
    uint8_t intCon = getIntCon(port);
    intCon |= intPins;
    gpIntEn |= intPins;
    if(port==A){
        ioDirA |= intPins; 
        write(IODIRA, ioDirA);
        write(GPINTENA, gpIntEn);
        write(INTCONA, intCon);
        write(DEFVALA, defVal);
    }
    else if (port==B){
        ioDirB |= intPins; 
        write(IODIRB, ioDirB);
        write(GPINTENB, gpIntEn);
        write(INTCONB, intCon);
        write(DEFVALB, defVal);
    }
}

void MyMCP23S17::deleteAllInterruptsOnPort(mcp_port port){
    if(port==A){
        write(GPINTENA, 0);
    }
    else if (port==B){
        write(GPINTENB, 0);
    }
}

void MyMCP23S17::setPinPullUp(uint8_t pin, mcp_port port, uint8_t pinLevel){
    uint8_t gppu = getPortPullUp(port);
    if(port==A){
        if(pinLevel==HIGH){
            gppu |= (1<<pin);
        }
        else if(pinLevel==LOW){
            gppu &= ~(1<<pin);
        }
        write(GPPUA, gppu);
    }
    else if(port==B){
        if(pinLevel==HIGH){
            gppu |= (1<<pin);
        }
        else if(pinLevel==LOW){
            gppu &= ~(1<<pin);
        }
        write(GPPUB, gppu);
    }
}
        
void MyMCP23S17::setPortPullUp(uint8_t pulledUpPins, mcp_port port){
    if(port==A){
        write(GPPUA, pulledUpPins);
    }
    else if(port==B){
        write(GPPUB, pulledUpPins);
    }
}

uint8_t MyMCP23S17::getPortPullUp(mcp_port port){
    if(port==A){
        return read(GPPUA);
    }
    else{ 
        return read(GPPUB);
    }
}      

void MyMCP23S17::setIntMirror(uint8_t mirrored){
    uint8_t ioConA = getIoCon(A);
    uint8_t ioConB = getIoCon(B);
    if(mirrored){
        ioConA |= (1<<MIRROR);
        ioConB |= (1<<MIRROR);
    }
    else{
        ioConA &= ~(1<<MIRROR);
        ioConB &= ~(1<<MIRROR);
    }
    write(IOCONA, ioConA);
    write(IOCONB, ioConB);
}   

uint8_t MyMCP23S17::getIntFlag(mcp_port port){
    uint8_t value = 0;
    if(port==A){
        value = read(INTFA);
    }
    else if (port==B){ 
        value = read(INTFB);
    }
    return value;
}

bool MyMCP23S17::getPin(uint8_t pin, mcp_port port, bool useTransaction){
    uint8_t result = 0;
    if(port==A){
        result = read(GPIOA, useTransaction);
    }
    else if(port==B){
        result = read(GPIOB, useTransaction);
    }
    result &= (1<<pin);
    if(result) return true;
    else return false;  
}

uint8_t MyMCP23S17::getPort(mcp_port port, bool useTransaction){
    uint8_t value = 0;
    if(port==A){
        value = read(GPIOA, useTransaction);
    }
    else if(port==B){
        value = read(GPIOB, useTransaction);
    }
    return value;
}

uint8_t MyMCP23S17::getIntCap(mcp_port port){
    uint8_t value = 0;
    if(port==A){
        value = read(INTCAPA);
    }
    else if (port==B){
        value = read(INTCAPB);
    }
    return value;
}

void MyMCP23S17::setSPIClockSpeed(unsigned long clock){
    mySPISettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
}

void MyMCP23S17::softReset(){

    setPortMode(0, A);
    setPortMode(0, B);
    uint8_t reg = 0x02;
    
    _spi->beginTransaction(mySPISettings);
    setCsPinLow();
    uint16_t transBytes = ((SPI_Address<<1) << 8 | reg);
    _spi->transfer16(transBytes); 

    for(int8_t i=0; i<18; i++){
        _spi->transfer(0x00);
    }

    setCsPinHigh();
    _spi->endTransaction();
}

void MyMCP23S17::startBatch() {
    _spi->beginTransaction(mySPISettings);
}

void MyMCP23S17::endBatch() {
    _spi->endTransaction();
}

#ifdef DEBUG_MyMCP23S17   // see MyMCP23S17_config.h
void MyMCP23S17::printAllRegisters(){
    uint8_t reg = 0;
    uint8_t regVal = 0;
    char buf[20] = {};
    
    Serial.println(F("Register status:"));
    
    regVal = read(reg); regVal = ~regVal; reg++;
    sprintf(buf, "IODIRA  : 0x%02X | 0b", regVal); 
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); regVal = ~regVal; reg++;
    sprintf(buf, "IODIRB  : 0x%02X | 0b", regVal); 
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "IPOLA   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "IPOLB   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPINTENA: 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPINTENB: 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "DEFVALA : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "DEFVALB : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTCONA : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTCONB : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "IOCONA  : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "IOCONB  : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPPUA   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPPUB   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTFA   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTFB   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTCAPA : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "INTCAPB : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPIOA   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "GPIOB   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "OLATA   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
    
    regVal = read(reg); reg++;
    sprintf(buf, "OLATB   : 0x%02X | 0b", regVal);
    Serial.print(buf); printBin(regVal);
}

void MyMCP23S17::printBin(uint8_t val){
    uint8_t valCopy = val;
    uint8_t proceedingZeros = 0;
    
    while(!(valCopy & 0x80) && proceedingZeros < 7){
        valCopy = valCopy << 1;
        proceedingZeros++;
    }
    
    for(int i=0; i<proceedingZeros; i++){
        Serial.print("0");
    }
    Serial.println(val, BIN);   
}
#endif // DEBUG_MyMCP23S17 

/* Private Functions */

void MyMCP23S17::setIoCon(uint8_t val, mcp_port port){
    if(port==A){
        write(IOCONA, val);
    }
    else if (port==B){
        write(IOCONB, val);
    }
}

uint8_t MyMCP23S17::getIoCon(mcp_port port){
    if(port==A){
        return read(IOCONA);
    }
    else{
        return read(IOCONB);
    }
}


void MyMCP23S17::setGpIntEn(uint8_t val, mcp_port port){
    if(port==A){
        write(GPINTENA, val);
    }
    else if (port==B){
        write(GPINTENB, val);  
    }
}


uint8_t MyMCP23S17::getGpIntEn(mcp_port port){
    if(port==A){
        return read(GPINTENA);
    }
    else{
        return read(GPINTENB);
    }
}


void MyMCP23S17::setIntCon(uint8_t val, mcp_port port){
    if(port==A){
        write(INTCONA, val);
    }
    else if (port==B){
        write(INTCONB, val);
    }
}

uint8_t MyMCP23S17::getIntCon(mcp_port port){
    if(port==A){
        return read(INTCONA);
    }
    else{
        return read(INTCONB);
    }
}

void MyMCP23S17::setDefVal(uint8_t val, mcp_port port){
    if(port==A){
        write(DEFVALA, val);
    }
    else if (port==B){
        write(DEFVALB, val);    
    }
}

uint8_t MyMCP23S17::getDefVal(mcp_port port){
    if(port==A){
        return read(DEFVALA);
    }
    else{
        return read(DEFVALB);  
    }
}

void MyMCP23S17::write(uint8_t reg, uint8_t val, bool useTransaction){
    
    if (useTransaction) {
        _spi->beginTransaction(mySPISettings);
    }
    
    setCsPinLow();

    uint8_t buffer[] = {OPCODE_WRITE, reg, val};
    _spi->transfer(buffer, sizeof(buffer));

    setCsPinHigh();

    if (useTransaction) {
        _spi->endTransaction();
    }
}

void MyMCP23S17::write(uint8_t reg, uint8_t valA, uint8_t valB, bool useTransaction){
    
    if (useTransaction) {
        _spi->beginTransaction(mySPISettings);
    }

    setCsPinLow();

    uint8_t buffer[] = {OPCODE_WRITE, reg, valA, valB};
    _spi->transfer(buffer, sizeof(buffer));

    setCsPinHigh();

    if (useTransaction) {
        _spi->endTransaction();
    }
}

uint8_t MyMCP23S17::read(uint8_t reg, bool useTransaction){

    uint8_t regVal;
    
    if (useTransaction) {
        _spi->beginTransaction(mySPISettings);
    }

    setCsPinLow();

    uint8_t buffer[] = {0b01000001, reg};
    _spi->transfer(buffer, sizeof(buffer));
    // uint16_t transBytes = (((SPI_Address<<1) | SPI_READ) << 8 | reg);
    // _spi->transfer16(transBytes); 
    regVal = _spi->transfer(0x00);

    setCsPinHigh();

    if (useTransaction) {
        _spi->endTransaction();
    }

    return regVal;
}

void MyMCP23S17::setCsPinMode() {

#ifdef USE_HW_CS
return;
#endif

pinMode(csPin, OUTPUT);

}

void MyMCP23S17::setCsPinLow() {

#ifdef MyMCP23S17_USE_HW_CS
return;
#endif

#ifdef MyMCP23S17_USE_ESP32_REG_WRITE
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(csPin));
#else
    digitalWrite(csPin, LOW);
#endif
}

void MyMCP23S17::setCsPinHigh() {

#ifdef MyMCP23S17_USE_HW_CS
return;
#endif

#ifdef MyMCP23S17_USE_ESP32_REG_WRITE
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(csPin));
#else
    digitalWrite(csPin, HIGH);
#endif
}
