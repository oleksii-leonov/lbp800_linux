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
#ifndef		__PKCAPT_H
#define		__PKCAPT_H

inline BYTE Send_StartDataPacket(int Device, UINT DataSize);
inline BYTE Send_InterBandPacket(int Device);
inline BYTE Send_Packet_a0_a0(int Device);
inline BYTE Send_Packet_a0_a2(int Device);
inline BYTE Send_PaperPacket(int Device);
inline BYTE Send_Packet_a1_a1(int Device);
inline BYTE Send_Packet_a1_d0(int Device);
inline BYTE Send_Packet_a4_e0(int Device);
inline BYTE Send_Packet_a0_d0(int Device);
inline BYTE Send_Packet_a5_e0(int Device);

#endif		//__PKCAPT_H

