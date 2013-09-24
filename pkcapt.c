/*
 *  Linux Canon LBP800 CAPT driver
 *  CAPT command packages routines header file
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

#include "machine.h"
#include "llcapt.h"

#include "pkcapt.h"

///////////////////////////////////////////////////////////////
// PACCHETTI DI COMANDO STANDARD
BYTE Packet_a0_a0[4]		= {0xa0, 0xa0, 0x04, 0x00};
BYTE StartDataPacket[]		= {0xa0, 0xc0, 0x00, 0x00};
BYTE InterBandPacket[]		= {0xa0, 0xe0, 0x04, 0x00};
BYTE Packet_a0_a2[4]		= {0xa0, 0xa2, 0x04, 0x00};
BYTE PaperPacket[4]			= {0xa1, 0xa0, 0x04, 0x00};
BYTE Packet_a1_a1[4] 		= {0xa1, 0xa1, 0x04, 0x00};
BYTE Packet_a1_d0[4] 		= {0xa1, 0xd0, 0x04, 0x00};
BYTE Packet_a4_e0[4] 		= {0xa4, 0xe0, 0x04, 0x00};
BYTE Packet_a0_d0[38]		=
{                         //0x00 ???
  	0xa0, 0xd0, 0x26, 0x00, 0x16, 0x00, 0xa4, 0x01,
  	0x02, 0x01, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f,
    0x00, 0x11, 0x03, 0x01, 0x01, 0x01, 0x02, 0x00,
    0x00, 0x00, 0x70, 0x00, 0x78, 0x00, 0x50, 0x02,
    0x7a, 0x1a, 0x60, 0x13, 0x67, 0x1b
};
BYTE Packet_a5_e0[12]		=
{
	0xa5, 0xe0, 0x0c, 0x00,
	0xee, 0xdb, 0xea, 0xad, 0x00, 0x00, 0x00, 0x00
};

inline BYTE Send_StartDataPacket(int Device, UINT DataSize)
{
  DataSize += sizeof(StartDataPacket); // LUNGHEZZA DELLO HEADER !

  StartDataPacket[2] = DataSize & 0xFF;
  StartDataPacket[3] = (DataSize & 0xFF00) >> 8;

  return SendPacket(Device, StartDataPacket, sizeof(StartDataPacket), 0);
  
} // END Send_StartDataPacket()

inline BYTE Send_InterBandPacket(int Device)
{
  return SendPacket(Device, InterBandPacket, sizeof(InterBandPacket), 255); //llcapt
  
} // END Send_InterBandPacket()

inline BYTE Send_Packet_a0_a0(int Device)
{
  return SendPacket(Device, Packet_a0_a0, sizeof(Packet_a0_a0), 255); //llcapt
	
} // END Send_Packet_a0_a0()

inline BYTE Send_Packet_a0_a2(int Device)
{
  return SendPacket(Device, Packet_a0_a2, sizeof(Packet_a0_a2), 255);
  
} // END Send_Packet_a0_a2()

inline BYTE Send_PaperPacket(int Device)
{
  return SendPacket(Device, PaperPacket, sizeof(PaperPacket), 255);
  
} // END Send_PaperPacket()

inline BYTE Send_Packet_a1_a1(int Device)
{
  return SendPacket(Device, Packet_a1_a1, sizeof(Packet_a1_a1), 255);
  
} // END Send_Packet_a1_a1()

inline BYTE Send_Packet_a1_d0(int Device)
{
  return SendPacket(Device, Packet_a1_d0, sizeof(Packet_a1_d0), 0);
  
} // END Send_Packet_a1_d0()

inline BYTE Send_Packet_a4_e0(int Device)
{
  return SendPacket(Device, Packet_a4_e0, sizeof(Packet_a4_e0), 255);
  
} // END Send_Packet_a4_e0

inline BYTE Send_Packet_a0_d0(int Device)
{
  return SendPacket(Device, Packet_a0_d0, sizeof(Packet_a0_d0), 0);
  
} // END Send_Packet_a0_d0

inline BYTE Send_Packet_a5_e0(int Device)
{
  return SendPacket(Device, Packet_a5_e0, sizeof(Packet_a5_e0), 255);
  
} // END Send_Packet_e5_a0
