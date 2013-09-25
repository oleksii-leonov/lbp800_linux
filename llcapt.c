/*
 *  Linux Canon LBP800 CAPT driver
 *  low level lbp800 CAPT routines
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
#include <unistd.h>

#include <string.h>

#include "machine.h"
#include "pport.h"

#include "timcapt.h"
#include "llcapt.h"
#include "pkcapt.h"
#include "hlcapt.h"
#include "lbp800.h"
#include <unistd.h>
#define DEBUG_WaitForStatus
// BUFFER DEI VALORI RITORNATI DALLA STAMPANTE DOPO UN COMANDO
BYTE		RETCNT;
BYTE		RETBUF[256];

///////////////////////////////////////////////////////////////
// ATTENDE UN CERTO VALORE DI ALCUNI BITS DEL BYTE DI STATO
// INPUT :
//    Device				HANDLE AL DEVICE DELLA PORTA
//    s						I BITS ATTESI
//    mask					LA MASCHERA DI SELEZIONE BITS
//	  msec					MASSIMO TEMPO DI ATTESA IN MICROSECONDI
// OUTPUT :
//	  LO STATO LETTO			SE TUTTO OK
//	  CAPT_TIMEOUT(0x01)		SE TIMEOUT
BYTE WaitForStatus(int Device, const BYTE s, const BYTE mask, const ULONG usec)
{
  BYTE stat=0;
  TTimeId Timer;

#ifdef DEBUG_WaitForStatus
  fprintf(stderr, "DEBUG - llcapt:WaitForStatus(0x%02x, 0x%02x...) - ", s, mask);
#endif

  StartTimer(&Timer, usec);
  do
  {
  	stat = Parport_ReadStatus(Device) & 0xf8;
  	if( (stat & mask) == s)
  	{
#ifdef DEBUG_WaitForStatus
      fprintf(stderr, "OK %02x\n", stat);
#endif
  	  return stat;
  	}
	usleep(50);
  }
  while(!Timeout(&Timer));
#ifdef DEBUG_WaitForStatus
  fprintf(stderr, "TIMEOUT - llcapt STATUS IS 0x%02x\n", stat);
#endif
  return CAPT_TIMEOUT;

} // END WaitForStatus()

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
BYTE CheckStatus(int Device, const BYTE s, const BYTE mask, const ULONG usec)
{
  if(WaitForStatus(Device, s, mask, usec) == CAPT_TIMEOUT){
    return CAPT_PRINTER_BAD;
	}
  return CAPT_PRINTER_OK;

} // END CheckStatus()

void PrintBuffer(void)
{
  int i;

  fprintf(stderr, "RETBUF = [");
//  for(i = 0 ; i < RETCNT ; i++)
  for(i = 0 ; i < 20 ; i++)
  {
    if(i > 0)
      fprintf(stderr, ", ");
    fprintf(stderr, "%02x", RETBUF[i]);
  }
  fprintf(stderr, "]\n");

}

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
BYTE SendPacket(int Device, BYTE const *buf, const BYTE cnt, BYTE MaxRead)
{
  BYTE i=0, stat=0;

  // INIZIALIZZA IL BUFFER
  RETCNT = 0;

  // SE 0 BYTES, RITORNA
  if(cnt == 0)
    return 0;

  // INVIA IL PACKET
  Parport_WriteControl(Device, 0x06);
  for(i = 0 ; i < cnt ; i++)
  {
    Parport_WriteData(Device, buf[i]);
    usleep(CAPT_SHORTDELAY);
    Parport_WriteControl(Device, 0x04);
    usleep(CAPT_SHORTDELAY);
    Parport_WriteControl(Device, 0x05);
    if(WaitForStatus(Device, 0x78, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT)
      return CAPT_PRINTER_BAD;
    Parport_WriteControl(Device, 0x04);
    if(WaitForStatus(Device, 0xf0, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT)
      return CAPT_PRINTER_BAD;
  }

  // SE NESSUN BYTE DA LEGGERE, RITORNA
  if(MaxRead == 0)
    return CAPT_PRINTER_OK;

  // LEGGE I DATI RESTITUITI DALLA STAMPANTE
  Parport_WriteControl(Device, 0x26);
  usleep(CAPT_SHORTDELAY);
  for(i = 0 ; i < MaxRead ; i++)
  {
    Parport_WriteControl(Device, 0x22);
    if((stat = WaitForStatus(Device, 0x10, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT))
      return CAPT_PRINTER_BAD;
    RETBUF[RETCNT++] = Parport_ReadData(Device);

    // RITOCCA IL VALORE DI MAXREAD DAI BYTES CHE LEGGE DAL PACCHETTO
    if(RETCNT == 4)
      MaxRead = RETBUF[2];

    Parport_WriteControl(Device, 0x20);
    stat = WaitForStatus(Device, 0x50, 0xf7, CAPT_TTIMEOUT);
    if(stat == CAPT_TIMEOUT)
     return CAPT_PRINTER_BAD;
    if(stat == 0x58)
      break;
  } // for i

  Parport_WriteControl(Device, 0x22);
  if(WaitForStatus(Device, 0x58, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT)
    return CAPT_PRINTER_BAD;

  Parport_WriteControl(Device, 0x26);

  if(WaitForStatus(Device, 0xf8, 0xff, CAPT_TTIMEOUT) == CAPT_TIMEOUT)
    return CAPT_PRINTER_BAD;

  Parport_WriteControl(Device, 0x06);

///////////////
//PrintBuffer();
///////////////
  return CAPT_PRINTER_OK;

} // END SendPacket()


// RETBUF = [a1, a0, 16, 00, 00, 10, c0, 00, 00, 00, b0, 00, b3, 00, 7d, 00, fd, 00, 00, 00]  before printing paper OK!
// RETBUF = [a1, a0, 16, 00, 00, 10, 80, 00, 00, 00, b0, 00, b3, 00, 7d, 00, 00, 00, 00, 00]  before printing No paper 
BYTE CheckForPaperBeforeSendNewPageToPrinter(int a_dDevice){
  BYTE dPaperStatus=CAPT_PRINTER_BAD;
  BYTE dPortStatus=CAPT_PRINTER_OK;	
  int dTimeIndex=0;
  fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter Can we print next page?\n");
// RETBUF = [a1, a0, 16, 00, 00, 10, c0, 00, 00, 00, b0, 00, b3, 00, 7d, 00, fd, 00, 00, 00]
  for (dTimeIndex=0; dTimeIndex< 120 ;dTimeIndex++){ // no more 120 seconds to wait
//      fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter looking for status %d\n",dTimeIndex);
//      dPortStatus=Send_InterBandPacket(a_dDevice);
      PrintBuffer();
      if (dPortStatus!= CAPT_PRINTER_OK){
	dPaperStatus=CAPT_PRINTER_BAD;
        fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter Check printer on/off \n");
	break; // some HW error
      }
// we have so info from printer. See state
// first page
      if ((RETBUF[0]==0xA1)&&(RETBUF[1]==0xA0)&& (RETBUF[16]==0xFD)) {// There is a balnk page... Go to print!
	 dPaperStatus=CAPT_PRINTER_OK;
         fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter First page is ready. p50 \n");
	 break;
      }

// next pages

//      if ((RETBUF[0]==0xA0)&&(RETBUF[1]==0xA0)&& (RETBUF[5]==0x10)) {// There is a balnk page... Go to print!
//	 dPaperStatus=CAPT_PRINTER_OK;
//         fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter First page is ready.  p 60 \n");
//	 break;
//      }

//     dPortStatus= Send_Packet_a0_a0(a_dDevice);
      dPortStatus=Send_PaperPacket(a_dDevice);
      sleep(1); // delay ...
  }
  fprintf(stderr,"llcapt:CheckForPaperBeforeSendNewPageToPrinter page printed. p 90 \n");
   PrintBuffer();
//  for (dTimeIndex=0;dTimeIndex<20;dTimeIndex++){
//      Send_Packet_a0_a0(a_dDevice);
//      PrintBuffer();
//      Send_PaperPacket(a_dDevice);
//      PrintBuffer();
//  }

  return dPaperStatus; 
}

//RETBUF = [a0, e0, 06, 00, 00, 0c, 04, 00, 80, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00]   get paper 
//RETBUF = [a0, e0, 06, 00, 00, 0d, 04, 00, 80, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00]   printing in progress
//RETBUF = [a0, e0, 06, 00, 00, 0f, 04, 00, 80, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00]   page printed. Wait for paper info
//RETBUF = [a0, e0, 06, 00, 12, 0f, 04, 00, 80, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00]   page printed. No paper
//RETBUF = [a0, e0, 06, 00, 10, 0f, 04, 00, 80, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00]   page printed. Paper Ok!

//RETBUF = [a1, a0, 16, 00, 00, 10, 80, 00, 00, 02, b0, 09, b3, 0d, 7d, 00, 00, 00, 00, 00]
// Printer is printing now
// Wait till printing ends
BYTE WaitCurrentPageIsPrintedBeforToSendNext(int a_dDevice){
  BYTE dPaperStatus=CAPT_PRINTER_OK; // page is printed out. Send next... I you have paper for that ;-)
  BYTE dPortStatus=CAPT_PRINTER_OK;
  int dTimeIndex=0;
  fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext Waiting for print end .............. \n");
  for (dTimeIndex=0;dTimeIndex<50;dTimeIndex++){ // no more ... seconds per page
//      fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext looking for status %d\n",dTimeIndex);
      //PrintBuffer();
//      dPortStatus=Send_InterBandPacket(a_dDevice);
      dPortStatus=Send_Packet_a0_a0(a_dDevice); //pkcapt
      PrintBuffer();
      if (dPortStatus!= CAPT_PRINTER_OK){
	dPaperStatus=CAPT_PRINTER_BAD;
        fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext Check printer on/off \n");
	break; // sone HW error. Can njt send data to printer
      }
      if (RETBUF[0]==0xA0 &&  RETBUF[1]==0xA0 &&  RETBUF[12] && RETBUF[14]&& RETBUF[16]) {
	 dPaperStatus=CAPT_PRINTER_OK;
         fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext Send next page. p50 \n");
	 break;
      }
//      if ((RETBUF[5]==0x0F) && (RETBUF[4]) ){// This is next page
//	 dPaperStatus=CAPT_PRINTER_OK;
//       fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext Next page is ready. \n");
//	 break;
//      }
//      fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext page is printing. Wait. \n");	
//      dPortStatus=Send_Packet_a0_a0(a_dDevice); //pkcapt
//	dPortStatus=Send_InterBandPacket(a_dDevice);
      sleep(1);
  }
  fprintf(stderr,"llcapt:WaitCurrentPageIsPrintedBeforToSendNext Page is printed. \n");
//  for (dTimeIndex=0;dTimeIndex<50;dTimeIndex++){
//       Send_Packet_a0_a0(a_dDevice); //pkcapt
//       PrintBuffer();
//      Send_InterBandPacket(a_dDevice);
//       PrintBuffer();
//      sleep(1);
//  }
// usleep(500);
 return dPaperStatus;
}


////////////////////////////////////////////////////////////////
// ATTENDE CHE LA STAMPANTE SIA PRONTA PER L' INVIO DATI
// INPUT :
//	  Device				IL DEVICE DI STAMPA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE WaitForReady(int Device)
{
  BYTE Status=0;

  while(1)
  {
    // INVIA IL PACCHETTO DI INTERBANDA
    Status = Send_InterBandPacket(Device);
    if(Status != CAPT_PRINTER_OK)
      return CAPT_PRINTER_BAD;

    // CONTROLLA SE RICHIESTO UN PACCHETTO a0_a0
    if((RETBUF[5] & 0x01) != 0)
    {
      if(Send_Packet_a0_a0(Device) != CAPT_PRINTER_OK)
        return CAPT_PRINTER_BAD;
    }
    // SE LA RISPOSTA CONTIENE UNO '0x00' NEL QUARTO BYTE, OK
    else if(RETBUF[4] == 0x00)
      return CAPT_PRINTER_OK;

  }

} // END WaitForReady()



////////////////////////////////////////////////////////////////
// ATTENDE CHE LA STAMPANTE ABBIA TERMNATO LA STAMPA DI UNA PAGINA
// INPUT :
//	  Device				IL DEVICE DI STAMPA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE WaitForPageReady(int Device)
{
  BYTE Status=0;
  fprintf(stderr, "llcapt:WaitForPageReady 10 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
  while(1)
  {
    // INVIA IL PACCHETTO DI INTERBANDA
    Status = Send_InterBandPacket(Device);
    if(Status != CAPT_PRINTER_OK)
      return CAPT_PRINTER_BAD;

    // CONTROLLA SE RICHIESTO UN PACCHETTO a0_a0
    if((RETBUF[5] & 0x01) != 0)
    {
      if(Send_Packet_a0_a0(Device) != CAPT_PRINTER_OK)
        return CAPT_PRINTER_BAD;

      // SE E' PRONTO PER UN NUOVO INVIO PAGINA, RITORNA OK
      else if(RETBUF[4] == 0x00)
        return CAPT_PRINTER_OK;

    }
  }

} // END WaitForPageReady()


////////////////////////////////////////////////////////////////
// INIZIO DI INVIO DI UN BUFFER DI DATI DI STAMPA ALLA STAMPANTE
// INPUT :
//	  cnt					IL NUMERO DI BYTES DA INVIARE
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE StartData(int Device, UINT cnt)
{
  // INTERROGA LA STAMPANTE SE E' PRONTA A RICEVERE ALTRI DATI
  if(WaitForReady(Device) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;

  // INVIA LO HEADER
  if(Send_StartDataPacket(Device, cnt) != CAPT_PRINTER_OK)
    return CAPT_PRINTER_BAD;

  Parport_WriteData(Device, 0x43);
  Parport_WriteControl(Device, 0x05);

  return CheckStatus(Device, 0x78, 0xff, CAPT_TTIMEOUT);

} // END StartData()

////////////////////////////////////////////////////////////////
// INVIO DI UN BUFFER DI DATI DI STAMPA ALLA STAMPANTE
// INPUT :
//    buf					IL BUFFER CONTENENTE I BYTES DA INVIARE
//	  cnt					IL NUMERO DI BYTES DA INVIARE
// OUTPUT :
//	  NULLA
BOOL SendData(int Device, BYTE const *buf, UINT cnt, BYTE Seed, UINT repWrite)
{
  BYTE LastVal;
  UINT i;
//  ssize_t dTmoDummy=0;

  if(repWrite == 0)
    repWrite = 1;

  if(cnt == 0)
    return -1;

  LastVal = Seed;
  while(cnt > 0)
  {
  	LastVal ^= Seed;
  	LastVal ^= *buf++;

  	for(i = 0 ; i < repWrite ; i++){
      	Parport_WriteData(Device, LastVal);
//		dTmoDummy=write(Device,&LastVal,sizeof(LastVal));
	}

    cnt--;
  }
  return CAPT_PRINTER_OK;

} // END SendData()




////////////////////////////////////////////////////////////////
// TERMINA L' INVIO DI DATI ALLA STAMPANTE
// INPUT :
//    NULLA
// OUTPUT :
//	  RITORNA CAPT_OK SE TUTTO BENE, CAPT_PRINTER_BAD SE ERRORE
BYTE EndData(int Device)
{
  // TERMINA IL PACCHETTO
//  fprintf(stderr, "llcapt:EndData 10 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
  Parport_WriteControl(Device, 0x04);
//  fprintf(stderr, "llcapt:EndData 20 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
  if(CheckStatus(Device, 0xf0, 0xff, CAPT_TTIMEOUT) != CAPT_PRINTER_OK){
      fprintf(stderr, "llcapt:EndData 50 PortStatus %04X\n",(int)Parport_ReadStatus(Device));
    return CAPT_PRINTER_BAD;
    }

  return CAPT_PRINTER_OK;

} 

void Port_Sleep(void){
     usleep(CAPT_SHORTDELAY);
}// END EndData()


