

/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: 
 *  (1) "AS IS" WITH NO WARRANTY; 
 *  (2) TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, HopeRF SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) HopeRF
 * website: www.HopeRF.com
 *          www.HopeRF.cn
 *
 */

/*! 
 * file       rfm69_tx.ino
 * hardware   HopeDuino
 * software   send message via rfm69
 * note       can talk to HopeRF's EVB or DK demo           
 *
 * version    1.0
 * date       Jun 10 2014
 * author     QY Ruan
 */


 /*
 
 /** @file
    LaCrosse/StarMétéo/Conrad TX35 protocol.
*/
/**
Generic decoder for LaCrosse "IT+" (instant transmission) protocol.
Param device29or35 contain "29" or "35" depending of the device.
LaCrosse/StarMétéo/Conrad TX35DTH-IT, TFA Dostmann 30.3155     Temperature/Humidity Sensors.
LaCrosse/StarMétéo/Conrad TX29-IT                              Temperature Sensors.
Tune to 868240000Hz
Protocol
========
Example data : https://github.com/merbanan/rtl_433_tests/tree/master/tests/lacrosse/06/gfile-tx29.cu8
       a    a    2    d    d    4    9    2    8    4    4    8    6    a    e    c
    Bits :
    1010 1010 0010 1101 1101 0100 1001 0010 1000 0100 0100 1000 0110 1010 1110 1100
    Bytes num :
    ----1---- ----2---- ----3---- ----4---- ----5---- ----6---- ----7---- ----8----
    ~~~~~~~~~ 1st byte
    preamble, sequence 10B repeated 4 times (see below)
              ~~~~~~~~~~~~~~~~~~~ bytes 2 and 3
    brand identifier, always 0x2dd4
                                  ~~~~ 1st nibble of bytes 4
    datalength (always 9) in nibble, including this field and crc
                                       ~~~~ ~~ 2nd nibble of bytes 4 and 1st and 2nd bits of byte 5
    Random device id (6 bits)
                                              ~ 3rd bits of byte 5
    new battery indicator
                                               ~ 4th bits of byte 5
    unknown, unused
                                                 ~~~~ ~~~~ ~~~~ 2nd nibble of byte 5 and byte 6
    temperature, in bcd *10 +40
                                                                ~ 1st bit of byte 7
    weak battery
                                                                 ~~~ ~~~~ 2-8 bits of byte 7
    humidity, in%. If == 0x6a : no humidity sensor
                                                                          ~~~~ ~~~~ byte 8
    crc8 of bytes
Developer's comments
====================
I have noticed that depending of the device, the message received has different length.
It seems some sensor send a long preamble (33 bits, 0 / 1 alternated), and some send only
six bits as the preamble. I own 3 sensors TX29, and two of them send a long preamble.
So this decoder synchronize on the following sequence:
    1010 1000 1011 0111 0101 0010 01--
       A    8    B    7    5    2    4
-  0 -  5 : short preabmle [101010B]
-  6 - 14 : brand identifier [2DD4h]
- 15 - 19 : datalength [9]
Short preamble example (sampling rate - 1Mhz):
https://github.com/merbanan/rtl_433_tests/tree/master/tests/lacrosse/06/gfile-tx29-short-preamble.cu8.
TX29 and TX35 share the same protocol, but pulse are different length, thus this decoder
handle the two signal and we use two r_device struct (only differing by the pulse width).
*/
 


#include <HopeDuino_RFM69.h> 

rf69Class radio;
//1010 1010 1010 1010 1010 1010 1010 1010 1 = preamble 33 bis
//0010 1101 1101 0100 =0x2dd4 brand identifier
//1001 = 9 Data len - no. of nibbles (4 bits) to follow - total 40 bits. hence 9x4 to follow after data len
//
//1001 0010 1000 0100 0100 1000 0110 1010 1110 1100
byte str[5] = {146,132,72,106,236};

void setup()
{
 radio.Modulation     = OOK;
 radio.COB            = RFM69H;
 radio.Frequency      = 914865;
 radio.OutputPower    = 20+11;          //10dBm OutputPower //range: 0-31 [-11dBm~+20dBm] for RFM69H/RFM69HC
 radio.PreambleLength = 4;             //4 Byte preamble (need to make it 33 bits 1010 1010 1010 1010 1010 1010 1010 1010 1) - will keep 32 bits here and add the last 1 to next
 radio.FixedPktLength = true;           //packet length didn't in message which need to be send
 radio.PayloadLength  = 5;
 radio.CrcDisable     = true;
 radio.AesOn          = false;

 radio.SymbolTime     = 58004;         //17.24 Kbps
 radio.Devation       = 0;             //devation only need for FSK
 radio.BandWidth      = 100;            //100KHz for bandwidth
 radio.SyncLength     = 3;              //
 radio.SyncWord[0]    = 0xAA;
 radio.SyncWord[1]    = 0x2D;
 radio.SyncWord[2]    = 0xD4;
 radio.vInitialize();
 radio.vGoStandby();

}

void loop()
{
 radio.bSendMessage(str, 5);

 delay(4000);

}
 
 
