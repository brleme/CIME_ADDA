#ifndef ADDA_H
#define ADDA_H
#include <bcm2835.h>
#include <iostream>
#define  DRDY  RPI_GPIO_P1_11         //P0
#define  RST  RPI_GPIO_P1_12     //P1
#define	SPICS	RPI_GPIO_P1_15	//P3   ads1256  cs
#define	SPICS1	RPI_GPIO_P1_16	//P4   DAC8552 CS

#define CS_1() bcm2835_gpio_write(SPICS,HIGH)
#define CS_0()  bcm2835_gpio_write(SPICS,LOW)

#define CS1_1() bcm2835_gpio_write(SPICS1,HIGH)
#define CS1_0()  bcm2835_gpio_write(SPICS1,LOW)

#define DRDY_IS_LOW()	((bcm2835_gpio_lev(DRDY)==0))

#define RST_1() 	bcm2835_gpio_write(RST,HIGH);
#define RST_0() 	bcm2835_gpio_write(RST,LOW);

//class ADDA{
    enum
    {
        REG_STATUS = 0,	// x1H
        REG_MUX    = 1, // 01H
        REG_ADCON  = 2, // 20H
        REG_DRATE  = 3, // F0H
        REG_IO     = 4, // E0H
        REG_OFC0   = 5, // xxH
        REG_OFC1   = 6, // xxH
        REG_OFC2   = 7, // xxH
        REG_FSC0   = 8, // xxH
        REG_FSC1   = 9, // xxH
        REG_FSC2   = 10, // xxH
    };

    typedef enum
    {
        ADS1256_GAIN_1			= (0),	/* GAIN   1 */
        ADS1256_GAIN_2			= (1),	/*GAIN   2 */
        ADS1256_GAIN_4			= (2),	/*GAIN   4 */
        ADS1256_GAIN_8			= (3),	/*GAIN   8 */
        ADS1256_GAIN_16			= (4),	/* GAIN  16 */
        ADS1256_GAIN_32			= (5),	/*GAIN    32 */
        ADS1256_GAIN_64			= (6),	/*GAIN    64 */
    }ADS1256_GAIN_E;

    typedef enum{
        ADS1256_30000SPS = 0,
        ADS1256_15000SPS,
        ADS1256_7500SPS,
        ADS1256_3750SPS,
        ADS1256_2000SPS,
        ADS1256_1000SPS,
        ADS1256_500SPS,
        ADS1256_100SPS,
        ADS1256_60SPS,
        ADS1256_50SPS,
        ADS1256_30SPS,
        ADS1256_25SPS,
        ADS1256_15SPS,
        ADS1256_10SPS,
        ADS1256_5SPS,
        ADS1256_2d5SPS,
        ADS1256_DRATE_MAX
    }ADS1256_DRATE_E;

    #define ADS1256_DRAE_COUNT = 15;

    typedef struct{
        ADS1256_GAIN_E Gain;		/* GAIN  */
        ADS1256_DRATE_E DataRate;	/* DATA output  speed*/
        int32_t AdcNow[8];			/* ADC  Conversion value */
        uint8_t Channel;			/* The current channel*/
        uint8_t ScanMode;	/*Scanning mode,   0  Single-ended input  8 channel�� 1 Differential input  4 channel*/
    }ADS1256_VAR_T;



    const uint8_t s_tabDataRate[ADS1256_DRATE_MAX] =
    {
        0xF0,		/*reset the default values  */
        0xE0,
        0xD0,
        0xC0,
        0xB0,
        0xA1,
        0x92,
        0x82,
        0x72,
        0x63,
        0x53,
        0x43,
        0x33,
        0x20,
        0x13,
        0x03
    };




    enum
    {
        CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
        CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
        CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
        CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
        CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
        CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
        CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
        CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
        CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
        CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
        CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
        CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
        CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
        CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
    };

class ADDA{


public:
    ADDA();
    ~ADDA();

    int32_t ReadChannel(uint8_t _ch);
    uint16_t Voltage_Convert(float Vref, float voltage);
    uint8_t ADS1256_ReadReg(uint8_t _RegID);//static bb
    uint8_t ADS1256_Recive8Bit(void); //static bb
    uint8_t ADS1256_ReadChipID(void);
    int32_t ADS1256_ReadData(void);//static bb
    int32_t ADS1256_GetAdc(uint8_t _ch);
    uint8_t ADS1256_Scan(void);
    ADS1256_VAR_T g_tADS1256;
    void SetDAC0(uint32_t);
    void SetDAC1(uint32_t);
    void bsp_DelayUS(uint64_t micros);
    void ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate);  //python checked
    void ADS1256_SetChannel(uint8_t _ch);//static bb
    void ADS1256_SetDiffChannel(uint8_t _ch);//static bb
    void ADS1256_ISR(void);
    void ADS1256_WaitDRDY(void);//static bb
    void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue);//static bb
    void ADS1256_WriteCmd(uint8_t _cmd);//static bb
    void ADS1256_DelayDATA(void); //static bb
    void ADS1256_Send8Bit(uint8_t _data);
    void Write_DAC8552(uint8_t channel, uint32_t Data);
    void ADS1256_StartScan(uint8_t _ucScanMode);


};



#endif // ADDA_H
