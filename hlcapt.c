/*
 *  Linux Canon LBP800 CAPT driver
 *  high level lbp800 CAPT routines
 *  Copyright (c) 2007 Massimo Del Fedele <max@veneto.com>
 * 
 *  Adapted from a printer driver for Samsung ML-85G laser printer
 *  (C) Copyleft, 2000 Rildo Pragana <rpragana@acm.org>
 *  and from Linux Canon CAPT LBP810 driver
 *  Copyright (C) 2004 Nicolas Boichat <nicolas@boichat.ch>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <sys/ioctl.h>

#include "machine.h"
#include "pport.h"
#include "llcapt.h"
#include "pkcapt.h"
#include <unistd.h>
#include <errno.h>
#include "hlcapt.h"


////////////////////////////////////////////////////////////////
// INIZIALIZZAZIONE DELLA STAMPANTE
BYTE InitPrinter(int Device)
{
  BYTE i=0;
//  ssize_t dCount=0;
  int dPortMode=0;  
//  BYTE bPortState =0;
//  BYTE bCurrentPortState =0;
//  int dIndex=0;
//  ClosePrinter( Device);
  
 
  //for (dIndex=0;dIndex<256; dIndex++){
  //     Parport_WriteControl(Device, dIndex); // 00001100b PARPORT_CONTROL_INIT|PARPORT_CONTROL_SELECT
  //     fprintf(stderr, "hlcapt:InitPrinter  20 (%02d) ctrl %02X 0 PortStatus %04X\n",dIndex,(int)Parport_ReadControl(Device),(int)Parport_ReadStatus(Device)); 
  //     Parport_MonitorStatus(Device,10000);
  //     }
 
//      return CAPT_PRINTER_BAD;
  
 //  Parport_WriteControl(Device, PARPORT_CONTROL_INIT|PARPORT_CONTROL_SELECT /*0x0c*/);// 00001100b PARPORT_CONTROL_INIT|PARPORT_CONTROL_SELECT
  

//  BYTE bTmp=0xaa;
  
  Parport_WriteControl(Device, PARPORT_CONTROL_AUTOFD |PARPORT_CONTROL_INIT ); // 0x06);// PARPORT_CONTROL_AUTOFD |PARPORT_CONTROL_INIT   
//  fprintf(stderr, "hlcapt:InitPrinter  40 PortControl %04X\n",(int)Parport_ReadControl(Device)); 
    
// fprintf(stderr, "hlcapt:InitPrinter  10.1 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
//  WaitForStatus(Device, 0x78, 0xff, CAPT_TTIMEOUT);
//  dCount=write(Device,&bTmp, sizeof(bTmp));
//  fprintf(stderr, "DEBUG - hlcapt:InitPrinter not to write. err %x\n",(int)errno);
 
  
  fprintf(stderr, "hlcapt:InitPrinter  91 PortStatus %04X  \n",(int)Parport_ReadStatus(Device));    
  Parport_WriteData(Device, 0xaa);
  fprintf(stderr, "hlcapt:InitPrinter out 0xaa 95 PortStatus %04X\n",(int)Parport_ReadStatus(Device));    
  Parport_MonitorStatus(Device,1000);
  Parport_WriteControl(Device, 0x08); // PARPORT_CONTROL_SELECT

  fprintf(stderr, "hlcapt:InitPrinter  97  Ctrl 0x08 PortStatus %04X\n",(int)Parport_ReadStatus(Device));    
  Parport_MonitorStatus(Device,1000);
    
  Parport_WriteControl(Device, 0x0c); // 00001100b PARPORT_CONTROL_INIT|PARPORT_CONTROL_SELECT
    fprintf(stderr, "hlcapt:InitPrinter  100 Ctrl 0x0c  PortStatus %04X\n",(int)Parport_ReadStatus(Device));    
  Parport_MonitorStatus(Device,1000);
  if(WaitForStatus(Device, 0x58, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT){ // llcapt  PARPORT_STATUS_ERROR |PARPORT_STATUS_SELECT |PARPORT_STATUS_ACK 
    return CAPT_PRINTER_BAD;
  }
  fprintf(stderr, "hlcapt:InitPrinter  140 PortStatus %04X\n",(int)Parport_ReadStatus(Device));

  for(i = 0; i < 2 ; i++)
  {
    Parport_WriteControl(Device, 0x04); // PARPORT_CONTROL_INIT 
  
    fprintf(stderr, "hlcapt:InitPrinter  150  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  
  
    Parport_WriteControl(Device, 0x04); // // PARPORT_CONTROL_INIT 
    fprintf(stderr, "hlcapt:InitPrinter  151  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  
  
    

    Parport_WriteData(Device, 0xaa);    // pport

    fprintf(stderr, "hlcapt:InitPrinter  155  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  
    
    Parport_WriteData(Device, 0x55);
    fprintf(stderr, "hlcapt:InitPrinter  156  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  
    
    Parport_WriteData(Device, 0x00);
    fprintf(stderr, "hlcapt:InitPrinter  157  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  
    
//   fprintf(stderr, "hlcapt:InitPrinter  18 PortStatus %04X\n",(int)Parport_ReadStatus(Device)); 
 ////   
    Parport_WriteData(Device, 0xff);
	fprintf(stderr, "hlcapt:InitPrinter  200 PortStatus %04X\n",(int)Parport_ReadStatus(Device));

    fprintf(stderr, "hlcapt:InitPrinter  220  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
    Parport_MonitorStatus(Device,1000);  

//	if(WaitForStatus(Device, 0x58, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT){ // llcapt  PARPORT_STATUS_ERROR |PARPORT_STATUS_SELECT |PARPORT_STATUS_ACK 
//	      return CAPT_PRINTER_BAD;
//	}
    Parport_WriteControl(Device, 0x0c); // PARPORT_CONTROL_SELECT|PARPORT_CONTROL_INIT
  }
 //   ioctl (Device, PPGETMODE, &dPortMode);
//  fprintf(stderr, "hlcapt:InitPrinter  25 PortMode %04X\n",(int)dPortMode); 
//  fprintf(stderr, "hlcapt:InitPrinter  28 PortStatus %04X\n",(int)Parport_ReadStatus(Device)); 
//  Parport_WriteControl(Device, 0x0c);  	// PARPORT_CONTROL_SELECT | PARPORT_CONTROL_INIT
//  Port_Sleep();
  i=0x28;
//  write(Device,&i,1);
//     Port_Sleep();  
//    //fprintf(stderr, "hlcapt:InitPrinter  25 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
 //  write(Device,&i,1);
	  fprintf(stderr, "hlcapt:InitPrinter  250 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
	  Parport_WriteData(Device, 0x28);  // some magic figure...
  	  fprintf(stderr, "hlcapt:InitPrinter 400  0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
	  Parport_MonitorStatus(Device,1000);
 
	  fprintf(stderr, "hlcapt:InitPrinter 420  out 0x28 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
	  Parport_MonitorStatus(Device,1000);	
  
	  Port_Sleep();
	  Parport_WriteControl(Device, 0x06); 	// PARPORT_CONTROL_INIT | PARPORT_CONTROL_AUTOFD 
 
	  fprintf(stderr, "hlcapt:InitPrinter 450  ctrl 0x06 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
	  Parport_MonitorStatus(Device,1000);

	  Port_Sleep();

  
	  ioctl (Device, PPGETMODE, &dPortMode);
	  fprintf(stderr, "hlcapt:InitPrinter  600 PortMode %04X\n",(int)dPortMode);  
	  fprintf(stderr, "hlcapt:InitPrinter  630 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
	  if(WaitForStatus(Device, 0x38, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT){ // PARPORT_STATUS_PAPEROUT | PARPORT_STATUS_SELECT |PARPORT_STATUS_ERROR  llcapt
		return CAPT_PRINTER_BAD;
	  }

  Parport_WriteControl(Device, 0x07);// PARPORT_CONTROL_INIT |PARPORT_CONTROL_AUTOFD|PARPORT_CONTROL_STROBE 

  fprintf(stderr, "hlcapt:InitPrinter 650  ctrl 0x07 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
  Parport_MonitorStatus(Device,1000);  

  Parport_WriteControl(Device, 0x04); // PARPORT_CONTROL_INIT  
  
  fprintf(stderr, "hlcapt:InitPrinter 670  ctrl 0x04 PortStatus %04X\n",(int)Parport_ReadStatus(Device));     
  Parport_MonitorStatus(Device,1000);  
  
  if(WaitForStatus(Device, 0xd8, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT){// PARPORT_STATUS_ACK |PARPORT_STATUS_PAPEROUT | PARPORT_STATUS_ERROR   llcapt
      return CAPT_PRINTER_BAD;
  }
  fprintf(stderr, "hlcapt:InitPrinter  700 PortStatus %04X\n",(int)Parport_ReadStatus(Device));

  ioctl (Device, PPGETMODE, &dPortMode);
  fprintf(stderr, "hlcapt:InitPrinter  900 PortMode %04X\n",(int)dPortMode); 
//    fprintf(stderr, "hlcapt:InitPrinter Point 40\n");
//     PrintBuffer();
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK){ 
    return CAPT_PRINTER_BAD;
  }
   fprintf(stderr, "hlcapt:InitPrinter Point 1500\n");
  PrintBuffer();  
//   StartPrint(Device); //debug
  return CAPT_PRINTER_OK;

} // END InitPrinter()

////////////////////////////////////////////////////////////////
// CONTROLLO SE CARTA CARICATA NELLA STAMPANTE
BYTE CheckPaper(int Device)
{
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK)
    return 0;
  return(RETBUF[16] == 0xfd);
	
} // END CheckPaper()

////////////////////////////////////////////////////////////////
// INIZIO FASE DI STAMPA
BYTE StartPrint(int Device)
{
  // INVIA LA SEQUENZA DI INIT
/*
  if(Send_Packet_a0_a2(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a4_e0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a5_e0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_a0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
*/
  if(Send_Packet_a1_a1(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_a2(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a4_e0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a5_e0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_a0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_PaperPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_d0(Device) != CAPT_PRINTER_OK) // , Packet_a0_d0_26_00_ECC, 38, 0)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a1_d0(Device) != CAPT_PRINTER_OK) // , Packet_a1_d0_04_00, 4, 0)
    return CAPT_PRINTER_BAD;
  if(Send_InterBandPacket(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_a0(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  
    
  return CAPT_PRINTER_OK;

/*
  if(Send_Packet_a0_a2(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a0_d0(Device) != CAPT_PRINTER_OK) // , Packet_a0_d0_26_00_ECC, 38, 0)
    return CAPT_PRINTER_BAD;
  if(Send_Packet_a1_d0(Device) != CAPT_PRINTER_OK) // , Packet_a1_d0_04_00, 4, 0)
    return CAPT_PRINTER_BAD;
  return CAPT_PRINTER_OK;
*/
    

} // END StartPrint()
void ClosePrinter(int a_Device){
  Parport_WriteData(a_Device, 0x34);
  usleep(5);
  Parport_WriteControl(a_Device, 0x08);
  usleep(5);  
  Parport_WriteControl(a_Device, 0x08);
  usleep(5);  
  Parport_WriteControl(a_Device, 0x08); 
  usleep(5);  
  Parport_WriteControl(a_Device, 0x08);
  usleep(5);  
  Parport_WriteControl(a_Device, 0x0C);  
  usleep(5);  
  fprintf(stderr, "hlcapt:ClosePrinter 650  ctrl PortStatus %02X Device data %02X \n",(int)Parport_ReadStatus(a_Device),(int)Parport_ReadData(a_Device));     
  Parport_MonitorStatus(a_Device,1000);    
}


