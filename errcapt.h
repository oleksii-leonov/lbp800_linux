/*
 *  Linux Canon LBP800 CAPT driver
 *  lbp800 driver error messages header file
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
#ifndef		__ERRCAPT_H
#define		__ERRCAPT_H

// MESSAGGI DI ERRORE E/O INFORMATIVI
enum ErrorCodes
{
   CAPT_NoError
  ,CAPT_CantOpenDevice
  ,CAPT_PrinterNotResponding
  ,CAPT_NoBitmap
  ,CAPT_BadBitmap
  ,CAPT_PrintingPage

}; // END ErrorCodes

extern char *ErrorMessages[];

// MESSAGGI RITORNATI DAL BACKEND AL FILTRO
enum BackendMessages
{
   CAPT_BKMSG_None
  ,CAPT_BKMSG_EndPrint
  ,CAPT_BKMSG_Page
  ,CAPT_BKMSG_NoPaper
  ,CAPT_BKMSG_PaperJam
  ,CAPT_BKMSG_FatalError

}; // END BackendMessages

////////////////////////////////////////////////////////////////
// FUNZIONE PER INSERIRE UN MESSAGGIO IN UN FILE DI LOG
void LBP800Log(char *fmt, ...);

#endif		//__ERRCAPT_H
