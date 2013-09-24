/*
 *  Linux Canon LBP800 CAPT driver
 *  low level parport routines
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
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>
#include <linux/ppdev.h>
#include <linux/parport.h>
#include "machine.h"
#include "pport.h"
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>



// APRE IL DEVICE DELLA PORTA
inline int Parport_Open(char *DeviceName)
{
  int Device=-1;
  int bCurrentPortMode=0;
  int bPortMode=IEEE1284_MODE_ECP;
  bPortMode=IEEE1284_MODE_ECPRLE ;

  // APRE IL DEVICE
  Device = open (DeviceName, O_RDWR);
//    Device = open (DeviceName, O_RDWR|O_EXCL);
  if(Device == -1){
    fprintf(stderr, "DEBUG - pport:Parport_Open Can not open %s \n",DeviceName);
    return Device;
  }
//    sleep(2);
 
    
//    fprintf(stderr, "DEBUG - pport:Parport_Open Try to PPNEGOT to %x\n",(int)bPortMode); 
//    if (ioctl (Device, PPNEGOT, &bPortMode)) {
//       fprintf(stderr, "DEBUG - pport:Parport_Open Can not PPNEGOT. err %x\n",(int)errno);
// /      close (Device);
//       return -1;
//    }
    
  // ACQUISISCE IL DEVICE
  
  if (ioctl (Device, PPCLAIM))
  {
   fprintf(stderr, "DEBUG - pport:Parport_Open Can not claim\n");
    close (Device);
    return -1;
  }

   ioctl (Device, PPGETMODE, &bCurrentPortMode);
    fprintf(stderr, "DEBUG - pport:Parport_Open PPGETMODE  %04Xh\n",(int)bCurrentPortMode);
  
  return Device;

} // END Parport_Open()

// CHIUDE IL DEVICE DELLA PORTA
inline void Parport_Close(int Device)
{
  // RILASCIA IL DEVICE
  ioctl(Device, PPRELEASE);
  
  // CHIUDE IL DEVICE
  close(Device);
  
} // END Parport_Close()

// LEGGE UN DATO DALLA PORTA PARALLELA
inline BYTE Parport_ReadData(int Device)
{
  BYTE data;
  
  ioctl(Device, PPRDATA, &data);
  
  return data;
  
} // END Parport_ReadData()

// SCRIVE UN DATO NELLA PORTA PARALLELA
inline void Parport_WriteData(int Device, BYTE data)
{
  ioctl (Device, PPWDATA, &data);
  
} // END Parport_WriteData()

// LEGGE LO STATO DALLA PORTA PARALLELA
inline BYTE Parport_ReadStatus(int Device)
{
  BYTE stat=0;
  
  ioctl (Device, PPRSTATUS, &stat);
  
  return stat;
  
} // END Parport_ReadStatus()

// SCRIVE IL BYTE DI CONTROLLO NELLA PORTA PARALLELA
inline void Parport_WriteControl(int Device, BYTE control)
{
  int direction=0;
  
  // METTE A POSTO LA DIREZIONE DEI DATI
  // NECESSARIO PERCHE' LA CHIAMATA 'PPWCONTROL' GESTISCE SOLO LE
  // 4 LINEE "COMPATIBILI" E NON LA DIREZIONE DATI
  direction = ((control & 0x20) ? 1 : 0);
//  fprintf(stderr,"pport:arport_WriteControl Set direction to %d\n",direction);
  ioctl(Device, PPDATADIR, &direction);
    
  // IMPOSTA I RIMANENTI BYTES
  ioctl (Device, PPWCONTROL, &control);
  
} // END Parport_WriteControl()

BYTE Parport_ReadControl(int Device)
{
  BYTE bControl=0;
  ioctl (Device, PPRCONTROL, &bControl);
  return bControl;
} 


// Print port status. For debug.
void Parport_MonitorStatus(int a_dDevice, int a_dCounter){

	int dIndex=0;
	int bCurrentStatus=0;
	int bPrevStatus=0;
//    fprintf(stderr, "pport:Parport_MonitorStatus 20 \n");	
    for (dIndex=0;dIndex<a_dCounter;dIndex++){
      		bCurrentStatus=Parport_ReadStatus(a_dDevice);
      		if (bPrevStatus!=bCurrentStatus){
      			bPrevStatus=bCurrentStatus;
       			fprintf(stderr, "pport:Parport_MonitorStatus 30 PortStatus %02X %d\n",(int)bPrevStatus,dIndex);
       		}
       	}
    fprintf(stderr, "pport:Parport_MonitorStatus 999 \n \n");	       	
} // END Parport_MonitorStatus()
