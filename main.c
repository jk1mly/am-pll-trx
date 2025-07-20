/**
 * AM transceiver forPIC16F15325
 *
 *      JK1MLY:Hidekazu Inaba
 *
 *  (C)2025 JK1MLY All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F15325
        Driver Version    :  2.00
*/

#include "mcc_generated_files/mcc.h"

// **** I2C LCD and PLL **** **** ****

static i2c1_operations_t wr1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr,1);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_CONTINUE;
}

void I2C1_Write1ByteRegister(i2c1_address_t address, uint8_t reg, uint8_t data)
{
    while(!I2C1_Open(address)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(wr1RegCompleteHandler,&data);
    I2C1_SetBuffer(&reg,1);
    I2C1_SetAddressNackCallback(NULL,NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

void I2C_WriteLCD(uint8_t data)
{
    I2C1_Write1ByteRegister(0x3E, 0x40, data);
}

void I2C_WriteLCD_CMD(uint8_t cmd)
{
    I2C1_Write1ByteRegister(0x3E, 0x00, cmd);
}

void I2C_WritePLL(uint8_t reg, uint8_t data)
{
    I2C1_Write1ByteRegister(0x60, reg, data);
}

// **** DISP ****

void init_LCD()
{
//    I2C_Write2ByteRegister(0x3E, reg, 0x38);
    __delay_ms(100);
    I2C_WriteLCD_CMD(0x38); // FUNCTION SET
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x39); // IS=1
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x14); // INT OSC FREQUENCY
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x70); // CONTRAST SET 0,1,2,3
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x56); // CONTRAST SET 4,5
    __delay_ms(200);
    I2C_WriteLCD_CMD(0x6C); // F0LLOWER CONTROL
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x38); // IS=0
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x0C); // Display ON
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x01); // Clear Display
    __delay_ms(10);
    I2C_WriteLCD_CMD(0x06); // Entry Mode
    __delay_ms(10);
}

void test_DSP()
{
    __delay_ms(100);
    I2C_WriteLCD('J');
    __delay_ms(10);
    I2C_WriteLCD('K');
    __delay_ms(10);
    I2C_WriteLCD('1');
    __delay_ms(10);
    I2C_WriteLCD('M');
    __delay_ms(10);
    I2C_WriteLCD('L');
    __delay_ms(10);
    I2C_WriteLCD('Y');
}

void disp_FREQ(uint16_t freq)
{
    uint8_t temp;
    I2C_WriteLCD_CMD(0xC1); // L2 C2
    __delay_ms(100);
    I2C_WriteLCD('2');
    __delay_ms(10);
    I2C_WriteLCD('8');
    __delay_ms(10);
    I2C_WriteLCD('.');
    __delay_ms(10);
    temp = (uint8_t)(freq / 100);
    I2C_WriteLCD(temp + '0');
    __delay_ms(10);
    freq = freq - (temp * 100);
    temp = (uint8_t)(freq / 10);
    I2C_WriteLCD(temp + '0');
    __delay_ms(10);
    freq = freq - (temp * 10);
    I2C_WriteLCD((uint8_t)freq + '0');
}

void clr_LCD()
{
    I2C_WriteLCD_CMD(0x01); //Clear
}

void disp_TX()
{
    I2C_WriteLCD_CMD(0x80); // L1 C1
    __delay_ms(10);
    I2C_WriteLCD('T');
    __delay_ms(1);
    I2C_WriteLCD('X');
}

void disp_RX()
{
    I2C_WriteLCD_CMD(0x80); // L1 C1
    __delay_ms(10);
    I2C_WriteLCD('R');
    __delay_ms(1);
    I2C_WriteLCD('X');
}

// **** PLL-IC ****

/*
 *  fVCO = fXtal x (a + (b / c) )
 *     a : 15 - 90
 *     b : 0 - 1048575
 *     c : 1 - 1048575
 * MSNA : reg 26-33
 * MSNB : reg 34-41
 *  MSNA_P1[17:0] = 128 x a + Floor(128 x (b / c) - 512)
 *  MSNA_P2[17:0] = 128 x b - c x Floor(128 x (b / c))
 *  MSNA_P3[17:0] = c
 *  fOUT = fVCO x (a + (b / c) )
 *     a + (b / c) = 6 - 1800
 * MS0  : reg 42-49
 * MS1  : reg 50-57
 * MS2  : reg 58-65
 *  MS0_P1[17:0] = 128 x a + Floor(128 x (b / c) - 512)
 *  MS0_P2[17:0] = 128 x b - c x Floor(128 x (b / c))
 *  MS0_P3[17:0] = c
 */    

void MS0_test()
{
    I2C_WritePLL(42,0x10); //MS0_P3[15:8]
    I2C_WritePLL(43,0x00); //MS0_P3[7:0]
    I2C_WritePLL(44,0x10); //MS0_P1[17:16]
    I2C_WritePLL(45,0x0A); //MS0_P1[15:8]
    I2C_WritePLL(46,0x80); //MS0_P1[7:0]
    I2C_WritePLL(47,0x00); //MS0_P3[19:16]MS0_P2[19:16]
    I2C_WritePLL(48,0x00); //MS0_P2[15:8]
    I2C_WritePLL(49,0x00); //MS0_P2[7:0]    
}

void MS1_test()
{
    I2C_WritePLL(50,0x10); //MS1_P3[15:8]
    I2C_WritePLL(51,0x00); //MS1_P3[7:0]
    I2C_WritePLL(52,0x10); //MS1_P1[17:16]
    I2C_WritePLL(53,0x0A); //MS1_P1[15:8]
    I2C_WritePLL(54,0x80); //MS1_P1[7:0]
    I2C_WritePLL(55,0x00); //MS1_P3[19:16]MS1_P2[19:16]
    I2C_WritePLL(56,0x00); //MS1_P2[15:8]
    I2C_WritePLL(57,0x00); //MS1_P2[7:0]    
}

void set_REG(uint16_t freq)
{
    uint8_t  fVCO_a = 28;
    uint16_t fVCO_b;
    uint16_t fVCO_c = 1000;
    uint24_t fVCO_d;
    uint24_t fVCO_P1, fVCO_P2, fVCO_P3;
    uint8_t  REG26, REG27, REG28, REG29, REG30, REG31, REG32, REG33;
    
    fVCO_b  = freq;
    fVCO_d  = fVCO_b * 128 / fVCO_c;
    fVCO_P1 = fVCO_a * 128 + fVCO_d - 512;
    fVCO_P2 = fVCO_b * 128 - fVCO_d;
    fVCO_P3 = fVCO_c;
    
    REG26 = (uint8_t)(fVCO_P3 / 256);
    REG27 = (uint8_t)(fVCO_P3 & 0x00FF);
    REG28 = 0x00;
    REG29 = (uint8_t)(fVCO_P1 / 256);
    REG30 = (uint8_t)(fVCO_P1 & 0x00FF);
    REG31 = 0x00;
    REG32 = (uint8_t)(fVCO_P2 / 256);
    REG33 = (uint8_t)(fVCO_P2 & 0x00FF);
            
    I2C_WritePLL(26, REG26); //MSNA_P3[15:8]
    I2C_WritePLL(27, REG27); //MSNA_P3[7:0]
    I2C_WritePLL(28, REG28); //MSNA_P1[17:16]
    I2C_WritePLL(29, REG29); //MSNA_P1[15:8]
    I2C_WritePLL(30, REG30); //MSNA_P1[7:0]
    I2C_WritePLL(31, REG31); //MSNA_P3[19:16]MSNA_P2[19:16]
    I2C_WritePLL(32, REG32); //MSNA_P2[15:8]
    I2C_WritePLL(33, REG33); //MSNA_P2[7:0]        
}

void init_PLL()
{
    I2C_WritePLL(3,0xFF); //Disable Output
    I2C_WritePLL(16,0x80); //CLOCK0 Power down
    set_REG(300);
    MS0_test();
    MS1_test();
    I2C_WritePLL(177,0xA0);//Reset PLLA and PLLB
    I2C_WritePLL(16,0x4F); //CLOCK0 Power up 8mA
    I2C_WritePLL(17,0x4F); //CLOCK1 Power up 8mA
    I2C_WritePLL( 3,0xFC); //Enable CLOCK0,1
}

// **** VOLUME ****

void mute_VOL()
{
    uint8_t bits;
    uint16_t data = 0xFF04;

    for (bits = 0; bits < 11; bits++) {
        __delay_ms(2);
        VDAT_LAT = 0;
        __delay_ms(2);
        VCLK_LAT = 0;
        __delay_ms(2);
        VDAT_LAT = (data >> bits) & 0x01;
        __delay_ms(2);
        VCLK_LAT = 1;
    }
    __delay_ms(2);
    VDAT_LAT  = 1;
    __delay_ms(2);
    VCLK_LAT  = 0;
}

void set_VOL(uint16_t vol)
{
    uint8_t bits;
    uint16_t data;
    data = vol & 0x01FC;
    data = data | ((vol & 0x0003 ) << 7);
    data = data | 0xFE00;

    for (bits = 0; bits < 11; bits++) {
        __delay_ms(2);
        VDAT_LAT = 0;
        __delay_ms(2);
        VCLK_LAT = 0;
        __delay_ms(2);
        VDAT_LAT = (data >> bits) & 0x01;
        __delay_ms(2);
        VCLK_LAT = 1;
    }
    __delay_ms(2);
    VDAT_LAT  = 1;
    __delay_ms(2);
    VCLK_LAT  = 0;
}

// **** MAIN ****

void main(void)
{
    uint16_t freq = 315;

    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    init_LCD();
    test_DSP();
    init_PLL();
    __delay_ms(1000);
    clr_LCD();
    
    while (1)
    {
        uint16_t ad_vol, ad_frq;
        uint8_t  in_ptt, old_ptt;

        ad_frq = ADC_GetConversion(0);
        ad_frq = ad_frq / 256;
        freq = ad_frq + 250;        // 250 - 450(420)
        if (freq > 450){
            freq = 450;
        }
        
        disp_FREQ(freq);
        
        in_ptt = xPTT_PORT;
        if(in_ptt != old_ptt){
            if(in_ptt){
                disp_RX();
                LED_LAT  = 0;
                TXEN_LAT = 0;            
                set_REG(freq + 460);
                I2C_WritePLL( 3,0xFE); //Enable CLK0 Disable CLK1
//                ad_vol = ADC_GetConversion(1);
//                set_VOL(ad_vol >> 9);
            } else {
                disp_TX();
                mute_VOL();
                set_REG(freq);
                I2C_WritePLL( 3,0xFD); //Enable CLK1 Disable CLK0
                LED_LAT  = 1;
                TXEN_LAT = 1;
            }
            old_ptt = in_ptt;
            __delay_ms(100);
        } else {
            if(in_ptt){
                set_REG(freq + 460);
                ad_vol = ADC_GetConversion(1);
                set_VOL(ad_vol >> 9);
            } else {
                __delay_ms(10);
            }
        }
    }
}
