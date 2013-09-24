/*  Linux Canon LBP800 CAPT driver
 *  parallel capt backend
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cups/backend.h>

#include "machine.h"
#include "errcapt.h"
#include "pport.h"
#include "llcapt.h"
#include "hlcapt.h"
#include "pkcapt.h"
#include "parcapt.h"



////////////////////////////////////////////////////////////////
// CONTROLLO PRESENZA STAMPANTE NEL PORT SPECIFICATO
BOOL HasLBP800(char *DeviceName)
{
  BYTE Status=0;
  int Device=0;

  // TENTA L' APERTURA DEL DEVICE
  Device = Parport_Open(DeviceName); // pport
  if (Device == -1){
     fprintf(stderr,"parcapt:HasLBP No printer found at %s\n",DeviceName);
    return FALSE;
	}

  // TENTA L' INIZIALIZZAZIONE DELLA STAMPANTE
  Status =  InitPrinter(Device) ; //hlcapt

  // CHIUDE IL DEVICE
  Parport_Close(Device);
  fprintf(stderr,"parcapt:HasLBP Printer found at %s\n",DeviceName);
  // RITORNA 'TRUE' SE HA TROVATO LA STAMPANTE
  return (Status == CAPT_PRINTER_OK);

} // END HasLBP800()


////////////////////////////////////////////////////////////////
// STAMPA DELLA PAGINA CORRENTE
BYTE PrintPage(int Device, FILE *CBitmapf)
{
  int nBands=0, iBand=0;
  int nBandBytes=0;
  BYTE **Bands=0;
  BYTE *BandPtr=0;
  BYTE Status=0;
  int dTmp=0;
  // LEGGE IL NUMERO DI BANDE
  dTmp=fread(&nBands, sizeof(nBands), 1, CBitmapf);
  if (dTmp<1){
    fprintf(stderr,"parcapt:PrintPage is here. Bad input data\n");
    return CAPT_PRINTER_BAD;
  }
  // ALLOCA IL DESCRITTORE IN MEMORIA
  Bands = malloc(nBands * sizeof(BYTE *));
  fprintf(stderr,"parcapt:PrintPage is here for %d bands\n",nBands);
  // LEGGE LE BANDE DAL DISCO ALLOCANDO LO SPAZIO NECESSARIO
  // QUESTO PER POTER SCRIVERE VELOCEMENTE SULLA STAMPANTE SENZA ATTESE
  // DI LETTURA DA FILE
  for(iBand = 0 ; iBand < nBands; iBand++)
  {
    int dTmpDummy=fread(&nBandBytes, sizeof(nBandBytes), 1, CBitmapf);
    if (dTmpDummy<1){
		fprintf(stderr,"parcapt:PrintPage is here. Bad input data. P 100\n");
		return CAPT_PRINTER_BAD;
  	}
    Bands[iBand] = malloc(nBandBytes * sizeof(BYTE *) + sizeof(nBandBytes));
    BandPtr = Bands[iBand];
    memmove(BandPtr, &nBandBytes, sizeof(nBandBytes));
    BandPtr += sizeof(nBandBytes);
    dTmpDummy=fread(BandPtr, nBandBytes, 1, CBitmapf);
    if (dTmpDummy<1){
		fprintf(stderr,"parcapt:PrintPage is here. Bad input data. P 200\n");
		return CAPT_PRINTER_BAD;
  	}    
  }
//
  // INIZIO FASE DI STAMPA
  if( (Status = StartPrint(Device)) == CAPT_PRINTER_OK)
  {
     // STAMPA TUTTE LE BANDS
    for(iBand = 0 ; iBand < nBands; iBand++)
    {
  	  BandPtr = Bands[iBand];
      	  memmove(&nBandBytes, BandPtr, sizeof(nBandBytes));
//	  fprintf(stderr,"      parcapt:PrintPage  StartData for band %d\n",iBand);
  	  if((Status = StartData(Device, nBandBytes)) != CAPT_PRINTER_OK){
	        fprintf(stderr,"parcapt:PrintPage Error Can not StartData for band %d\n",iBand);
	        break;
	  }
//   	  fprintf(stderr,"      parcapt:PrintPage SendData for band %d\n",iBand);
  	  if((Status = SendData(Device, BandPtr+sizeof(nBandBytes), nBandBytes, 0x43, 1)) != CAPT_PRINTER_OK){
		fprintf(stderr,"parcapt:PrintPage Error Can not SendData for band %d\n",iBand);
          	break;
	  }
   	  fprintf(stderr,"      parcapt:PrintPage  EndData for band %d\n",iBand);
  	  if((Status = EndData(Device)) != CAPT_PRINTER_OK){
		fprintf(stderr,"parcapt:PrintPage Error Can not EndData for band %d\n",iBand);
        	break;
	  }
    } // for iBand

  }

  // LIBERA LA MEMORIA
  for(iBand = 0 ; iBand < nBands; iBand++){
    free(Bands[iBand]);
  }
  free(Bands);

  // ATTENDE CHE LA STAMPA SIA COMPLETA
  fprintf(stderr,"parcapt:PrintPage Page printed. WaitForPageReady \n");
//  WaitForPageReady(Device);
  Status=WaitCurrentPageIsPrintedBeforToSendNext(Device);

  return Status;

} // END PrintPage()


////////////////////////////////////////////////////////////////
// STAMPA DI TUTTE LE PAGINE
BYTE PrintPages(int Device, FILE *CBitmapf)
{
  int nPages=0 , iPage=0;
//  int  dIndex=0;
  int Status=CAPT_PRINTER_OK;
  int dPaperStatus=CAPT_PRINTER_OK;


//	for (dIndex=0;dIndex<10;dIndex++){
//	    fprintf(stderr,"parcapt:PrintPages looking for status %d\n",dIndex);
//		Send_InterBandPacket(Device);
//		Send_PaperPacket(Device);
//		PrintBuffer();
//		sleep(1);
//	}
// return Status;

//  int dTmpDummy=0;
  // LEGGE IL NUMERO DI PAGINE DAL BITMAP
  int dTmpDummy=fread(&nPages, sizeof(nPages), 1, CBitmapf);
	dTmpDummy=dTmpDummy;
  // CICLO DI STAMPA PER TUTTE LE PAGINE
//  Status = CAPT_PRINTER_OK;
  fprintf(stderr,"parcapt:PrintPages We have to print %d pages \n",nPages+1);
//  nPages=1;
  for( iPage = 0; iPage < nPages; iPage++)
  {
//    fprintf(stderr,"parcapt:PrintPages page %d. Is paper Ok?\n",iPage);
    dPaperStatus= CheckForPaperBeforeSendNewPageToPrinter(Device);
    if (dPaperStatus!=CAPT_PRINTER_OK){
      Status=dPaperStatus;
//      fprintf(stderr, "NOTICE: No paper to print page %d \n", iPage+1); 
//      fprintf(stderr, "NOTICE: No paper to print page %d \n", iPage+1);      
//      fprintf(stderr,"parcapt:PrintPages No paper to print page %d\n",iPage);
      break;
    }
    fprintf(stderr, "NOTICE: Printing page %d of %d \n", iPage+1,nPages);    
    // STAMPA LA PAGINA CORRENTE
    fprintf(stderr,"parcapt:PrintPages Printing page %d\n",iPage);
    if((Status = PrintPage(Device, CBitmapf)) != CAPT_PRINTER_OK){
	    fprintf(stderr,"parcapt:PrintPages Error to printing page %d\n",iPage);
      break;
      }
//    fprintf(stderr, "INFO: Page %d printed\n", iPage+1);  
//    fprintf(stderr, "NOTICE: total %d-pages \n", iPage+1);  
//NOTICE: message
//PAGE: total #-pages        
//    fprintf(stderr,"parcapt:PrintPages Sleep till next page %d\n",iPage+1);
//	sleep(19);
  }
//	for (dIndex=0;dIndex<60;dIndex++){
//	    fprintf(stderr,"parcapt:PrintPages looking for status %d\n",dIndex);
//		Send_InterBandPacket(Device);
//		PrintBuffer();
//		sleep(1);
//	}

  return Status;

} // END PrintBitmaps()


////////////////////////////////////////////////////////////////
// PROGRAMMA PRINCIPALE
int main(int argc, char** argv)
{
  int Status=CUPS_BACKEND_FAILED;
  int dTimeIndex=0;
  int dPaperStatus=CAPT_PRINTER_BAD;
  int dByteCiunterForstdin=0;
  FILE * fdTmpInputFile=tmpfile(); // input buffer file
  
  
//  int dPortStatus=CAPT_PRINTER_OK;	

  // HANDLE AL DEVICE parport(n)
  int Device=0;

  // NOME DEL FILE DEL DEVICE
  char DeviceName[255] = "/dev/parport0";

  // SE CHIAMATO SENZA PARAMETRI, STAMPA SU stdout L' ELENCO DELLE STAMPANTI TROVATE
  if(argc == 1)
  {
    char i;
    for(i = 0; i < 4; i++)
    {
//      DeviceName[strlen(DeviceName)-1] = i + '0';
      sprintf (DeviceName,"/dev/parport%1d",i);
      if(HasLBP800(DeviceName)) // parcapt
        printf("direct parcapt:/dev/parport%1d \"CANON-LBP800\" \"CANON-LBP800 on parport%1d\"\n", i, i);
    }
    return CUPS_BACKEND_OK;
  }

  // LEGGE IL NOME DEL DEVICE DA argv[0], SE PRESENTE LA STRINGA "parcapt:",
  // ALTRIMENTI SI TRATTA DI UNA CHIAMATA DI TEST QUINDI LEGGE IL DEVICE DA argv[1]
  if(!strncmp(argv[0], "parcapt:", strlen("parcapt:"))){
    strncpy(DeviceName, argv[0]+strlen("parcapt:"), 254);
    }
  else{
    strncpy(DeviceName, argv[1], 254);
    }

  // APRE IL DEVICE
  Device = Parport_Open(DeviceName);
  if (Device == -1)
  {
    fprintf(stderr, "ERROR: ");
    fprintf(stderr, ErrorMessages[CAPT_CantOpenDevice], DeviceName);
    exit(CUPS_BACKEND_FAILED);
  }

  // INIZIALIZZA LA STAMPANTE
  if((InitPrinter(Device) != CAPT_PRINTER_OK)&& InitPrinter(Device) != CAPT_PRINTER_OK)
  {
    fprintf(stderr, "ERROR: ");
	char * pcErrorMessage=ErrorMessages[CAPT_PrinterNotResponding];
  	fprintf(stderr, pcErrorMessage,0);
  	Parport_Close(Device);
  	exit(CUPS_BACKEND_FAILED);
  }

// debug
//	WaitCurrentPageIsPrintedBeforToSendNext(Device);
//        exit(CUPS_BACKEND_FAILED);
// wait for page before first page
  for (dTimeIndex=0;dTimeIndex<120;dTimeIndex++){

//       PrintBuffer();
//       Send_Packet_a0_a0(Device);
	dPaperStatus=CheckForPaperBeforeSendNewPageToPrinter(Device);
	if(dPaperStatus==CAPT_PRINTER_OK){
	   break;	
	}
         Send_PaperPacket(Device);
	sleep(1);
  }
//    exit(CUPS_BACKEND_FAILED);
// Copy input file to tmp buffer
  while(!feof(stdin)){
  	BYTE bTmpByte=0;
  	int dBytesRead=fread(&bTmpByte,sizeof(bTmpByte),1,stdin);
  	dBytesRead=fwrite(&bTmpByte,sizeof(bTmpByte),1,fdTmpInputFile);
  	dByteCiunterForstdin+=dBytesRead;
  }
    fprintf(stderr, "parcapt:main I had got %d bytes from stdin \n",dByteCiunterForstdin);  
//    exit(CUPS_BACKEND_FAILED);    
  // STAMPA LE PAGINE
  if(dPaperStatus==CAPT_PRINTER_OK){
    if (fdTmpInputFile!=stdin){
 	  	rewind(fdTmpInputFile);
    }
  	Status = PrintPages(Device, fdTmpInputFile);
  }
  // CHIUDE IL DEVICE
  Parport_Close(Device);

  // METTE LO STATUS IN FORMATO CUPS
  // E SEGNALA AL FILTRO CHE LA STAMPA E' FINITA
  if(Status == CAPT_PRINTER_OK)
  {
    Status = CUPS_BACKEND_OK;
    fprintf(stderr, "NOTICE: Printed.\n");      
  }
  else
  {
    Status = CUPS_BACKEND_FAILED;
  }

  // RITORNA LO STATO
  return Status;
}
