/*
 *  Linux Canon LBP800 CAPT driver
 *  low level lbp800 CAPT routines header file
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
#ifndef		__LLCAPT_H
#define		__LLCAPT_H

// RITARDI MASSIMI UTILIZZATI NELLA FUNZIONE WaitForStatus in msec
// #define		CAPT_SHORTDELAY			50
#define		CAPT_SHORTDELAY			10000
//#define		CAPT_SHORTDELAY			10
#define		CAPT_TTIMEOUT			10000000L

// VALORI RITORNATI DALLE VARIE FUNZIONI
#define		CAPT_PRINTER_OK			0x00
#define		CAPT_TIMEOUT			0x01
#define		CAPT_INVALID_STATUS		0x02
#define		CAPT_OUT_OF_PAPER		0x03
#define		CAPT_PAPER_JAM			0x04
#define		CAPT_PRINTER_BAD		0x05

// BUFFER DEI VALORI RITORNATI DALLA STAMPANTE DOPO UN COMANDO
extern BYTE		RETCNT;
extern BYTE		RETBUF[256];

///////////////////////////////////////////////////////////////
// ATTENDE UN CERTO VALORE DI ALCUNI BITS DEL BYTE DI STATO
// INPUT :
//    Device				HANDLE AL DEVICE DELLA PORTA
//    s						I BITS ATTESI
//    mask					LA MASCHERA DI SELEZIONE BITS
//	  usec					MASSIMO TEMPO DI ATTESA IN MICROSECONDI
// OUTPUT :
//	  LO STATO LETTO			SE TUTTO OK
//	  CAPT_TIMEOUT(0x01)		SE TIMEOUT
BYTE WaitForStatus(int Device, const BYTE s, const BYTE mask, const ULONG usec);

///////////////////////////////////////////////////////////////
// ATTENDE UN CERTO VALORE DI ALCUNI BITS DEL BYTE DI STATO
// E CONTROLLA CHE SIA OK
// INPUT :
//    Device				HANDLE AL DEVICE DELLA PORTA
//    s						I BITS ATTESI
//    mask					LA MASCHERA DI SELEZIONE BITS
//	  usec					MASSIMO TEMPO DI ATTESA IN MICROSECONDI
// OUTPUT :
//	  CAPT_PRINTER_OK			SE OK
//	  CAPT_TIMEOUT(0x01)		SE TIMEOUT
BYTE CheckStatus(int Device, const BYTE s, const BYTE mask, const ULONG usec);

////////////////////////////////////////////////////////////////
// INVIO DI UN PACKET DI COMANDO ALLA STAMPANTE
// E LETTURA DEI BYTES RITORNATI DA QUESTA IN RISPOSTA
// INPUT :
//    Port					INDIRIZZO BASE DELLA PORTA PARALLELA
//    buf					IL BUFFER CONTENENTE I BYTES DA INVIARE
//	  cnt					IL NUMERO DI BYTES DA INVIARE
//	  maxread				IL NUMERO MASSIMO DI BYTES DA LEGGERE
// OUTPUT :
//    IL NUMERO DI BYTES LETTI NELLA VARIABILE GLOBALE 'RETCNT'
//	  I BYTES LETTI SONO RITORNATI NELL' ARRAY GLOBALE 'RETBUF'
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE SendPacket(int Device, BYTE const *buf, const BYTE cnt, BYTE MaxRead);

////////////////////////////////////////////////////////////////
// ATTENDE CHE LA STAMPANTE SIA PRONTA PER L' INVIO DATI
// INPUT :
//	  Device				IL DEVICE DI STAMPA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE WaitForReady(int Device);

////////////////////////////////////////////////////////////////
// ATTENDE CHE LA STAMPANTE ABBIA TERMNATO LA STAMPA DI UNA PAGINA
// INPUT :
//	  Device				IL DEVICE DI STAMPA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE WaitForPageReady(int Device);

////////////////////////////////////////////////////////////////
// INIZIO DI INVIO DI UN BUFFER DI DATI DI STAMPA ALLA STAMPANTE
// INPUT :
//	  cnt					IL NUMERO DI BYTES DA INVIARE
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE StartData(int Device, UINT cnt);

////////////////////////////////////////////////////////////////
// INVIO DI UN BUFFER DI DATI DI STAMPA ALLA STAMPANTE
// INPUT :
//    buf					IL BUFFER CONTENENTE I BYTES DA INVIARE
//	  cnt					IL NUMERO DI BYTES DA INVIARE
// OUTPUT :
//	  NULLA
BOOL SendData(int Device, BYTE const *buf, UINT cnt, BYTE Seed, UINT repWrite);

////////////////////////////////////////////////////////////////
// TERMINA L' INVIO DI DATI ALLA STAMPANTE
// INPUT :
//    NULLA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE EndData(int Device);

void PrintBuffer(void);
BYTE CheckForPaperBeforeSendNewPageToPrinter(int a_dDevice);
BYTE WaitCurrentPageIsPrintedBeforToSendNext(int a_dDevice);
void Port_Sleep(void);
#endif		//__LLCAPT_H
