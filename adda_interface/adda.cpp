#include "adda.h"
ADDA::~ADDA(){
    bcm2835_spi_end();
    bcm2835_close();
}
using namespace std;
ADDA::ADDA(){
    uint8_t id;
    if (!bcm2835_init()){
        cout << "Error initializing BCM2835" << endl;
    }
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST );      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024); // The default
    bcm2835_gpio_fsel(SPICS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SPICS1, BCM2835_GPIO_FSEL_OUTP); //

 //   bcm2835_gpio_write(SPICS, HIGH);
 //   bcm2835_gpio_write(SPICS1, HIGH);

    bcm2835_gpio_fsel(DRDY, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(DRDY, BCM2835_GPIO_PUD_UP);
    id = ADS1256_ReadChipID();
    cout << endl;
    cout << "ID=";
    if (id != 3){
      std::cout << "Error, ASD1256 Chip ID =" << (int)id << endl;
    }
    else{
      std::cout << "Ok, ASD1256 Chip ID =" << (int)id << endl;
    }
    ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
    ADS1256_StartScan(0);
}

void ADDA::bsp_DelayUS(uint64_t micros){
        bcm2835_delayMicroseconds (micros);
}

void ADDA::ADS1256_Send8Bit(uint8_t _data){ //BB:function was static
    bsp_DelayUS(2);
    bcm2835_spi_transfer(_data);
}


void ADDA::ADS1256_StartScan(uint8_t _ucScanMode){
    g_tADS1256.ScanMode = _ucScanMode;
    {
        uint8_t i;

        g_tADS1256.Channel = 0;

        for (i = 0; i < 8; i++)
        {
            g_tADS1256.AdcNow[i] = 0;
        }
    }

}

void ADDA::ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate){
    g_tADS1256.Gain = _gain;
    g_tADS1256.DataRate = _drate;
    ADS1256_WaitDRDY();
    {
        uint8_t buf[4];		/* Storage ads1256 register configuration parameters */
        buf[0] = (0 << 3) | (1 << 2) | (0 << 1);  // The internal buffer is prohibited
        buf[1] = 0x08;
        buf[2] = (0 << 5) | (0 << 3) | (_gain << 0);
        buf[3] = s_tabDataRate[_drate];	// DRATE_10SPS;
        CS_0();
        ADS1256_Send8Bit(CMD_WREG | 0);	/* Write command register, send the register address */
        ADS1256_Send8Bit(0x03);			/* Register number 4,Initialize the number  -1*/
        ADS1256_Send8Bit(buf[0]);	/* Set the status register */
        ADS1256_Send8Bit(buf[1]);	/* Set the input channel parameters */
        ADS1256_Send8Bit(buf[2]);	/* Set the ADCON control register,gain */
        ADS1256_Send8Bit(buf[3]);	/* Set the output rate */
        CS_1();
    }
    bsp_DelayUS(50);
}

void ADDA::ADS1256_DelayDATA(void){  //was static BB
    bsp_DelayUS(10);	/* The minimum time delay 6.5us */
}

uint8_t ADDA::ADS1256_Recive8Bit(void){ //was static BB
    uint8_t read = 0;
    read = bcm2835_spi_transfer(0xff);
    return read;
}

void ADDA::ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue){ //was static BB
    CS_0();	/* SPI  cs  = 0 */
    ADS1256_Send8Bit(CMD_WREG | _RegID);	/*Write command register */
    ADS1256_Send8Bit(0x00);		/*Write the register number */

    ADS1256_Send8Bit(_RegValue);	/*send register value */
    CS_1();	/* SPI   cs = 1 */
}

uint8_t ADDA::ADS1256_ReadReg(uint8_t _RegID){ //was static BB
    uint8_t read;
    CS_0();	/* SPI  cs  = 0 */
    ADS1256_Send8Bit(CMD_RREG | _RegID);	/* Write command register */
    ADS1256_Send8Bit(0x00);	/* Write the register number */

    ADS1256_DelayDATA();	/*delay time */

    read = ADS1256_Recive8Bit();	/* Read the register values */
    CS_1();	/* SPI   cs  = 1 */

    return read;
}

void ADDA::ADS1256_WriteCmd(uint8_t _cmd){ //was static BB
    CS_0();	/* SPI   cs = 0 */
    ADS1256_Send8Bit(_cmd);
    CS_1();	/* SPI  cs  = 1 */
}

uint8_t ADDA::ADS1256_ReadChipID(void){
    uint8_t id;
    ADS1256_WaitDRDY();
    id = ADS1256_ReadReg(REG_STATUS);
    return (id >> 4);
}


void ADDA::ADS1256_SetChannel(uint8_t _ch){  // was static BB
    if (_ch > 7){
        return;
    }
    ADS1256_WriteReg(REG_MUX, (_ch << 4) | (1 << 3));	/* Bit3 = 1, AINN connection AINCOM */
}




void ADDA::ADS1256_SetDiffChannel(uint8_t _ch){ //BB: was static
    if (_ch == 0){
        ADS1256_WriteReg(REG_MUX, (0 << 4) | 1);	/* DiffChannal  AIN0�� AIN1 */
    }
    else if (_ch == 1){
        ADS1256_WriteReg(REG_MUX, (2 << 4) | 3);	/*DiffChannal   AIN2�� AIN3 */
    }
    else if (_ch == 2){
        ADS1256_WriteReg(REG_MUX, (4 << 4) | 5);	/*DiffChannal    AIN4�� AIN5 */
    }
    else if (_ch == 3){
        ADS1256_WriteReg(REG_MUX, (6 << 4) | 7);	/*DiffChannal   AIN6�� AIN7 */
    }
}



void ADDA::ADS1256_WaitDRDY(void){ //was static BB
    uint32_t i;
    for (i = 0; i < 400000; i++){
        if (DRDY_IS_LOW()){
            break;
        }
    }
    if (i >= 400000){
        printf("ADS1256_WaitDRDY() Time Out ...\r\n");
    }
}


int32_t ADDA::ADS1256_ReadData(void){ //was static BB
    uint32_t read = 0;
    uint8_t buf[3];
    CS_0();	/* SPI   cs = 0 */
    ADS1256_Send8Bit(CMD_RDATA);	/* read ADC command  */
    ADS1256_DelayDATA();	/*delay time  */
    buf[0] = ADS1256_Recive8Bit();
    buf[1] = ADS1256_Recive8Bit();
    buf[2] = ADS1256_Recive8Bit();
    read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
    read |= ((uint32_t)buf[1] << 8);
    read |= buf[2];
    CS_1();
    if (read & 0x800000){
        read |= 0xFF000000;
    }
    return (int32_t)read;
}

int32_t ADDA::ReadChannel(uint8_t _ch){
    int32_t iTemp;
    iTemp = ADS1256_GetAdc(_ch);
    return (iTemp / (int32_t)1677);
}
int32_t ADDA::ADS1256_GetAdc(uint8_t _ch){
    while((ADS1256_Scan() == 0));
    int32_t iTemp;
    if (_ch > 7){
        return 0;
    }
    iTemp = g_tADS1256.AdcNow[_ch];
    return iTemp;
}

void ADDA::ADS1256_ISR(void){
    if (g_tADS1256.ScanMode == 0)	/*  0  Single-ended input  8 channel�� 1 Differential input  4 channe */
    {
        ADS1256_SetChannel(g_tADS1256.Channel);	/*Switch channel mode */
        bsp_DelayUS(5);

        ADS1256_WriteCmd(CMD_SYNC);
        bsp_DelayUS(5);

        ADS1256_WriteCmd(CMD_WAKEUP);
        bsp_DelayUS(25);

        if (g_tADS1256.Channel == 0)
        {
            g_tADS1256.AdcNow[7] = ADS1256_ReadData();
        }
        else
        {
            g_tADS1256.AdcNow[g_tADS1256.Channel-1] = ADS1256_ReadData();
        }

        if (++g_tADS1256.Channel >= 8)
        {
            g_tADS1256.Channel = 0;
        }
    }
    else{  	/*DiffChannal*/
        ADS1256_SetDiffChannel(g_tADS1256.Channel);	/* change DiffChannal */
        bsp_DelayUS(5);

        ADS1256_WriteCmd(CMD_SYNC);
        bsp_DelayUS(5);

        ADS1256_WriteCmd(CMD_WAKEUP);
        bsp_DelayUS(25);

        if (g_tADS1256.Channel == 0)
        {
            g_tADS1256.AdcNow[3] = ADS1256_ReadData();
        }
        else
        {
            g_tADS1256.AdcNow[g_tADS1256.Channel-1] = ADS1256_ReadData();
        }

        if (++g_tADS1256.Channel >= 4)
        {
            g_tADS1256.Channel = 0;
        }
    }
}


uint8_t ADDA::ADS1256_Scan(void){
    if (DRDY_IS_LOW()){
        ADS1256_ISR();
        return 1;
    }
    return 0;
}

uint32_t ADDA::get_Status_DA0(){
    return status_DA0;
}
uint32_t ADDA::get_Status_DA1(){
    return status_DA1;
}
void ADDA::Write_DAC8552(uint8_t channel, uint32_t Data){
    CS1_1() ;
    CS1_0() ;
    bcm2835_spi_transfer(channel);
    bcm2835_spi_transfer((Data>>8));
    bcm2835_spi_transfer((Data&0xff));
    CS1_1() ;
}


uint16_t ADDA::Voltage_Convert(float Vref, float voltage){
    uint16_t _D_;
    _D_ = (uint16_t)(65536 * voltage / Vref);
    return _D_;
}

void ADDA::SetDAC0(uint32_t millivolts){
    status_DA0=millivolts;
    Write_DAC8552(0x30,(millivolts * 13));
}

void ADDA::SetDAC1(uint32_t millivolts){
    status_DA1=millivolts;
    Write_DAC8552(0x34, (millivolts * 13));
}
