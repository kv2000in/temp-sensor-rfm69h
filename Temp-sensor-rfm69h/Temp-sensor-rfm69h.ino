

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
From rtl example file gfile-tx29.cu8 (decoded successfully by rtl_433 version Elantra2012TPMS-1-g7541af5 branch WorkingBranchForElantra2012TPMS at 201909291032 inputs file rtl_tcp RTL-SDR SoapySDR)using inspectrum
1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 1 1 0 1 0 1 0 1 1 1 0 1 1 0 0
=1010 1010 0010 1101 1101 0100 1001 0010 1000 0100 0100 1000 0110 1010 1110 1100
= AA2DD49284486AEC  rate 275.269Hz period 3.63281 ms symbol rate 17.6172 kBd symbol period 56.7627 us

Readme from trl_tests file says data should be aa 2d d4 92 84 48 6a ec
which is 
1010 1010 0010 1101 1101 0100 1001 0010 1000 0100 0100 1000 0110 1010 1110 1100

After sending and reading via inspectrum
1 1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 1 1 0 1 0 1 0 1 1 1 0 1 1 0 0  = extra 1 at the start

removing extra 1 gives
1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 1 1 0 1 0 1 0 1 1 1 0 1 1 0 0
= AA2DD49284486AEC

so - let's send without the extra 1

0101 0100 0101 1011 1010 1001 0010 0101 0000 1000 1001 0000 1101 0101 1101 100 0 (add an extra 0 at the end) = 54 5B A9 25 08 90 D5 D8

capture and decode via inspectrum
1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 1 1 0 1 0 1 0 1 1 1 0 1 1 0 0

1010 1010 0010 1101 1101 0100 1001 0010 1000 0100 0100 1000 0110 1010 1110 1100  = AA2DD49284486AEC


from gfile-tx29-short-preamble.cu8   (Not decoded by rtl_433 version Elantra2012TPMS-1-g7541af5 branch WorkingBranchForElantra2012TPMS at 201909291032 inputs file rtl_tcp RTL-SDR SoapySDR)
1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 1 1 1 0 0 0 1 0 1 1 0 0 0 0 1 0 0 0 1 1 0 1 0 1 0 1 1 0 1 1 1 0 1
=1010 1010 0010 1101 1101 0100 1001 0011 1100 0101 1000 0100 0110 1010 1101 1101  = AA 2D D4 93 C5 84 6A DD
Read me file says this data should be = aa 2d d4 96 a6 41 22 50

 = 1010 1010 0010 1101 1101 0100 1001 0110 1010 0110 0100 0001 0010 0010 0101 0000



Transmitter is actually Tx37u-it
80 bit data
1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 0 0 1 0 1 1 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 0 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 1 0 0 0 1 1 0 1 0 1 0 1 0 0 0 1 0 0 0 
==AAAAAA2DD49206046A88
AA AA AA 2D D4 92 06 04 6A 88
 = {"time" : "@0.043352s", "brand" : "LaCrosse", "model" : "TX29-IT", "id" : 8, "battery" : "OK", "newbattery" : 0, "temperature_C" : 20.400, "mic" : "CRC"}
*/
 


#include <HopeDuino_RFM69.h> 

rf69Class radio;
//want to transmit a total of this - 8 bytes. 
//byte str[4] = {132,72,106,236 }; //AA 2D D4 92 84 48 6A EC
//byte str[4] = {8,144,213,216 }; //54 5B A9 25 08 90 D5 D8
byte str[4] = {6,4,106,136 }; //06 04 6A 88
void setup()
{
 radio.Modulation     = FSK;
 radio.COB            = RFM69H;
 radio.Frequency      = 914865;
 radio.OutputPower    = 12;          //10dBm OutputPower //range: 0-31 [-11dBm~+20dBm] for RFM69H/RFM69HC
 radio.PreambleLength = 2;             //4 Byte preamble (need to make it 33 bits 1010 1010 1010 1010 1010 1010 1010 1010 1) - will keep 32 bits here and add the last 1 to next
 radio.FixedPktLength = true;           //packet length didn't in message which need to be send
 radio.PayloadLength  = 4;
 radio.CrcDisable     = true;
 radio.AesOn          = false;

 radio.SymbolTime     = 58000;         //
 radio.Devation       = 35;             //devation only need for FSK
 radio.BandWidth      = 100;            //100KHz for bandwidth
 radio.SyncLength     = 4;//3;              //
 radio.SyncWord[0]    = 0xAA;//0xA8;//0xAA; //1010 1000
 radio.SyncWord[1]    = 0x2D; //0xB7;//0x2D;
 radio.SyncWord[2]    =0xD4; //0x52;//0xD4;
 radio.SyncWord[3]    = 0x92;//Added;
 radio.vInitialize();
 radio.vGoStandby();

}

void loop()
{
 radio.bSendMessage(str, 4);

 delay(4000);

}
 
 
