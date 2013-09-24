/*
 *  Linux Canon LBP800 CAPT driver
 *  bitmap compression lbp800 CAPT routines
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

#include "machine.h"
#include "errcapt.h"
#include "llcapt.h"
#include "hlcapt.h"

#include "bmcapt.h"

////////////////////////////////////////////////////////////////
// SKIP DI UN NUMERO DI BYTES NEL BITMAP
// NECESSARIO PERCHE' POTENDO PROVENIRE DA UNA 'PIPE'
// NON E' POSSIBILE USARE LA FUNZIONE seek
void Bitmap_Skip (FILE *Bitmapf, int Offset)
{
  BYTE Dummy[256]={0};
  int dTmp=0;
  if(!Offset)
    return ;

  while (Offset > sizeof(Dummy))
  {
    dTmp=fread(Dummy, 1, sizeof(Dummy), Bitmapf);
    Offset -= sizeof(Dummy);
  }
  dTmp=fread(Dummy, 1, Offset, Bitmapf);

} // END Bitmap_Skip()

////////////////////////////////////////////////////////////////
// LETTURA DI UNA LINEA DAL FILE DEL BITMAP
void Bitmap_GetLine(TSourceBitmap *Bitmap, BYTE *LinePtr)
{
  int BytesToRead=0, BytesLeft=0;
  int i=0;
  int dTmp=0;
  // AZZERA IL BUFFER
  memset(LinePtr, 0x00, BYTES_BY_LINE);

  // SALTA L' EVENTUALE LeftSkip
  Bitmap_Skip(Bitmap->Bitmapf, Bitmap->LeftSkip);

  // DETERMINA IL NUMERO DI BYTES DA LEGGERE
  BytesToRead = Bitmap->Width - Bitmap->LeftSkip;
  if(BytesToRead < 0)
    BytesToRead = 0;
  BytesLeft = 0;
  if(BytesToRead > BYTES_BY_LINE)
  {
  	BytesLeft = BytesToRead - BYTES_BY_LINE;
  	BytesToRead = BYTES_BY_LINE;
  }

  // LEGGE LA LINEA
  dTmp=fread(LinePtr, BytesToRead, 1, Bitmap->Bitmapf);

  // @@@@ HACK -- UN CODICE PIXEL A 43 NON E' AMMESSO
  // @@@@ PERCHE' MANDA IN PALLA IL SISTEMA DI CODIFICA XOR
  // @@@@ QUINDI.... I 43 LI TRASFORMO IN 63 (SIGH)
  for(i = 0 ; i < BYTES_BY_LINE ; i++)
    if(LinePtr[i] == 0x43)
      LinePtr[i] = 0x63;

  // SALTA EVENTUALI BYTES IN PIU'
  if(BytesLeft)
    Bitmap_Skip(Bitmap->Bitmapf, BytesLeft);

} // END Bitmap_GetLine()


////////////////////////////////////////////////////////////////
// RITORNA LA POSIZIONE DELL' ULTIMO BYTE DIFFERENTE
// TRA LA LINEA CORRENTE E LA LINEA PRECEDENTE (+1)
int getLastDifference(TLineBuf LineBuf, BYTE CurLine)
{
  int iPos;

  BYTE *CurrPtr = LineBuf[CurLine] + BYTES_BY_LINE;
  BYTE *PrevPtr = LineBuf[CurLine ? 0 : 1] + BYTES_BY_LINE;

  // CERCA A RITROSO L' ULTIMA DIFFERENZA
  iPos = BYTES_BY_LINE;
  while( (*(--CurrPtr) == *(--PrevPtr)) && iPos)
    iPos--;

  return iPos;

} // END getLastDifference()


////////////////////////////////////////////////////////////////
// COMPRESSIONE DEL BITMAP
BOOL CompressBitmap(TSourceBitmap *Bitmap, FILE *CBitmapf)
{
  // POSIZIONE DEL BITMAP CORRENTE NEL FILE DEI BITMAPS COMPRESSI
  long BitmapPos=0;

  // IL NUMERO DI BANDE NEL FILE E IL NUMERO DI BANDA CORRENTE
  int nBands=0;

  // IL NUMERO DI BYTES NELLA BANDA CORRENTE
  int nBandBytes=0;

  // PUNTATORE NEL FILE ALL' INIZIO DELLA BANDA CORRENTE
  long BandStart=0;

  // LINEE DA LEGGERE NEL BITMAP
  int LinesToRead=0;

  // LINEA CORRENTE NEL BITMAP
  int iLine=0;

  // LINEA CORRENTE NELLA BANDA CORRENTE
  int iBandLine=0;

  // ULTIMA DIFFERENZA CON LA LINEA PRECEDENTE
  int LastLineDiff=0;

  // BUFFER PER 2 LINEE DEL BITMAP
  BYTE LineBuf[2][BYTES_BY_LINE];
  BYTE CurLinePtr=0;

  // PUNTATORI AI BYTES CORRENTE E SUCCESSIVO NELLA LINEA IN ESAME
  BYTE *LinePtr=0;

  // BYTE CORRENTE NELLA LINEA IN ESAME
  UINT iByte=0;

  // NUMERO DI BYTES CONSECUTIVI RIPETUTI
  UINT rep=0;

  // CONTROLLA IL FORMATO DEL FILE BITMAP
  if (fgets((char *)(LineBuf[0]), BYTES_BY_LINE-1, Bitmap->Bitmapf) <= 0){
	fprintf(stderr,"bmcapt:CompressBitmap Point 100\n");
    return FALSE;
	}
  if (strncmp((char *)(LineBuf[0]), "P4", 2))
  {
    fprintf(stderr,ErrorMessages[CAPT_BadBitmap],0);
    return FALSE;
  }
//	fprintf(stderr,"bmcapt:CompressBitmap Point 110\n");
  // SALTA LA LINEA DI COMMENTO
  do
  {
    char * pTmp=fgets((char *)LineBuf[0], BYTES_BY_LINE-1, Bitmap->Bitmapf);
	pTmp=0;
  }
  while (LineBuf[0][0] == '#');

  // LEGGE LE DIMENSIONI DEL BITMAP
  if (sscanf((char *)(LineBuf[0]),"%d %d", &Bitmap->Width, &Bitmap->Height) < 2)
  {
    fprintf(stderr,ErrorMessages[CAPT_BadBitmap],0);
    return 0;
  }
	fprintf(stderr,"bmcapt:CompressBitmap Bitmap has Width=%d,Height=%d\n",Bitmap->Width,Bitmap->Height);
  // DIMENSIONE ORIZZONTALE E SKIP IN BYTES
  Bitmap->Width = (Bitmap->Width+7)/8 ;
  Bitmap->LeftSkip = (Bitmap->LeftSkip+7)/8 ;

  // SE GLI SKIPS SONO MAGGIORI DELLE DIMENSIONI DEL BITMAP, ERRORE
  if(Bitmap->TopSkip >= Bitmap->Height || Bitmap->LeftSkip >= Bitmap->Width){
	fprintf(stderr,"bmcapt:CompressBitmap  Error Point 130\n");
    return 0;
 }

  // SALTA SE RICHIESTO LE PRIME LINEE DEL BITMAP
  if (Bitmap->TopSkip)
    Bitmap_Skip(Bitmap->Bitmapf, Bitmap->Width * Bitmap->TopSkip);

  // DETERMINA LE LINEE DEL BITMAP DA LEGGERE
  LinesToRead = Bitmap->Height - Bitmap->TopSkip;
  if(LinesToRead > LINES_BY_PAGE)
    LinesToRead = LINES_BY_PAGE;

  // MEMORIZZA LA POSIZIONE CORRENTE NEL FILE DEI BITMAP COMPRESSI
  BitmapPos = ftell(CBitmapf);

  // SPAZIO PER IL NUMERO DI BAND CONTENUTE
  nBands = 0;
  fwrite(&nBands, sizeof(nBands), 1, CBitmapf);

  // CICLO PER TUTTE LE LINEE DEL BITMAP
  iLine = 0;
  do
  {
    // INIZIALIZZA IL DOPPIO BUFFER DELLE LINEE DEL BITMAP
    LineBuf[0][0] = LineBuf[1][0] = 0;
    CurLinePtr = 0;

    // INIZIALIZZA IL NUMERO DI BYTES PER LA BAND CORRENTE
    nBandBytes = 0;
    BandStart = ftell(CBitmapf);
    fwrite(&nBandBytes, sizeof(nBandBytes), 1, CBitmapf);

    // CICLO PER LA BAND CORRENTE
    iBandLine = 0;
  	do
  	{
      // LEGGE LA LINEA DAL BITMAP
      Bitmap_GetLine(Bitmap, LineBuf[CurLinePtr]);
      LinePtr = LineBuf[CurLinePtr];

      // DETERMINA L' ULTIMA DIFFERENZA CON LA LINEA PRECEDENTE
      if(iBandLine)
        LastLineDiff = getLastDifference(LineBuf, CurLinePtr) ;
      else
        LastLineDiff = BYTES_BY_LINE;

      // AGGIORNA IL PUNTATORE PER LA PROSSIMA LINEA
      CurLinePtr = (CurLinePtr ? 0 : 1);

      // SCRIVE IL BAND START NEL FILE COMPRESSO
      fputc(0x40, CBitmapf);

      // COMPRIME LA LINEA CORRENTE
      iByte = 0;
      while(iByte < LastLineDiff)
      {
      	// BYTES CONSECUTIVI UGUALI ?
      	if(iByte < LastLineDiff-1 && *LinePtr == *(LinePtr+1))
      	{
          // DETERMINA IL NUMERO DI BYTES RIPETUTI
      	  rep = 2;
      	  while(iByte + rep < LastLineDiff && *LinePtr == *(LinePtr+rep) && rep < 255)
      	    rep++;
      	  iByte += rep;

      	  // ORA, SE NON HA RAGGIUNTO LA FINE LINEA, CERCA DI ACCORPARE
      	  // UNO O 2 BYTES NON RIPETUTI AL PACCHETTO DEI BYTES RIPETUTI
      	  if(
      	    rep >= 8 &&
      	    iByte < LastLineDiff-2 &&
      	    *(LinePtr+rep) != *(LinePtr+rep+1) &&
      	    *(LinePtr+rep+1) != *(LinePtr+rep+2)
      	  )
      	  {
      	  	// OK, ACCORPA 2 BYTES DIVERSI AL PACCHETTO
      	  	fputc(0xa0 | (rep >> 3), CBitmapf);
      	  	fputc(0x02 | ((rep & 0x07) << 3), CBitmapf);
      	  	fputc(*LinePtr, CBitmapf);
      	  	fputc(*(LinePtr+rep), CBitmapf);
      	  	fputc(*(LinePtr+rep+1), CBitmapf);
      	  	iByte += 2;
      	  	LinePtr = LinePtr+rep+2;
      	  } // END accorpamento 2 bytes
      	  else if(
       	    iByte < LastLineDiff-1 &&
      	    *(LinePtr+rep) != *(LinePtr+rep+1)
      	  )
      	  {
      	  	// OK, ACCORPA 1 BYTE DIVERSO AL PACCHETTO
      	  	if(rep < 8)
      	  	{
       	  	  fputc(0xc1 | (rep << 3), CBitmapf);
      	  	  fputc(*LinePtr, CBitmapf);
      	  	  fputc(*(LinePtr+rep), CBitmapf);
      	  	}
      	  	else
      	  	{
       	  	  fputc(0xa0 | (rep >> 3), CBitmapf);
      	  	  fputc(0x01 | ((rep & 0x07) << 3), CBitmapf);
      	  	  fputc(*LinePtr, CBitmapf);
      	  	  fputc(*(LinePtr+rep), CBitmapf);
      	  	}
      	  	iByte += 1;
      	  	LinePtr = LinePtr+rep+1;
      	  } // END accorpamento 1 byte
      	  else
      	  {
      	  	// NO, NON ACCORPA BYTES AL PACCHETTO
      	  	if(rep < 8)
      	  	{
       	  	  fputc(0x40 | (rep << 3), CBitmapf);
      	  	  fputc(*LinePtr, CBitmapf);
      	  	}
      	  	else
      	  	{
       	  	  fputc(0xa0 | (rep >> 3), CBitmapf);
      	  	  fputc(0x80 | ((rep & 0x07) << 3), CBitmapf);
      	  	  fputc(*LinePtr, CBitmapf);
      	  	}
      	  	LinePtr = LinePtr+rep;
      	  } // END no accorpamento

      	} // END if bytes consecutivi uguali

      	else if(iByte < LastLineDiff-1 && *LinePtr != *(LinePtr+1))
      	{
          // DETERMINA IL NUMERO DI BYTES DIVERSI RIPETUTI
      	  rep = 2;
      	  while(iByte + rep < LastLineDiff && *(LinePtr+rep-1) != *(LinePtr+rep) && rep < 255)
      	    rep++;
      	  iByte += rep;

          // EMETTE I BYTES
          if (rep < 8)
          {
   	  	    fputc(rep << 3, CBitmapf);
          }
          else
          {
       	  	fputc(0xa0 | (rep >> 3), CBitmapf);
      	  	fputc(0xc0 | ((rep & 0x07) << 3), CBitmapf);
          }
          fwrite(LinePtr, 1, rep, CBitmapf);

          LinePtr += rep;

      	} // END if bytes consecutivi differenti

      	// CASO SPECIALE PER LA FINE DELLA LINEA
      	else if(iByte == LastLineDiff-1)
      	{
      	  // INVIA UN SOLO BYTE "DIFFERENTE"
      	  rep = 1;
  	      fputc(rep << 3, CBitmapf);
  	      fputc(*LinePtr, CBitmapf);
  	      iByte++;
  	      LinePtr++;
      	}
      } // END while iByte < LastLineDiff

      // SE CI SON BYTES UGUALI ALLA LINEA PRECEDENTE, LO SEGNALA
      if (LastLineDiff < BYTES_BY_LINE)
   	  	fputc(0x41, CBitmapf);

  	  iBandLine++;
  	  iLine++;
  	}
  	while(iBandLine < LINES_BY_BAND && iLine < LinesToRead);

  	// PACCHETTO DI CHIUSURA BANDA
  	fputc(0x40, CBitmapf);

  	// SE ULTIMA BANDA, AGGIUNGE IL TERMINATORE DI PAGINA
  	if(iLine >= LinesToRead)
    	fputc(0x42, CBitmapf);

  	// BANDA TERMINATA, DEVE AGGIORNARE I BYTES DELLA BANDA CORRENTE
  	// E I BYTES TOTALI
  	nBandBytes = ftell(CBitmapf) - BandStart - sizeof(nBandBytes);
  	fseek(CBitmapf, BandStart, SEEK_SET);
  	fwrite(&nBandBytes, sizeof(nBandBytes), 1, CBitmapf);
  	fseek(CBitmapf, 0L, SEEK_END);

  	// INCREMENTA IL NUMERO DI BANDE
  	nBands++;

  }
  while(iLine < LinesToRead);

  // SKIP DELL' EVENTUALE PARTE FINALE DEL BITMAP
  for(iLine = LinesToRead + Bitmap->TopSkip ; iLine < Bitmap->Height ; iLine++)
    Bitmap_GetLine(Bitmap, LineBuf[0]);

  // AGGIORNA IL NUMERO DELLE BANDE NEL FILE
  fseek(CBitmapf, BitmapPos, SEEK_SET);
  fwrite(&nBands, sizeof(nBands), 1, CBitmapf);
  fseek(CBitmapf, 0L, SEEK_END);

  return TRUE;

} // END CompressBitmap()


////////////////////////////////////////////////////////////////
// COMPRESSIONE DI TUTTE LE PAGINE DI STAMPA
FILE *CompressBitmaps(TSourceBitmap *Bitmap)
{
  // IL FILE DEL BITMAP COMPRESSO
  FILE *CBitmapf;

  // IL NUMERO DI PAGINE
  UINT nPages;

  // CREA IL FILE TEMPORANEO PER LA COMPRESSIONE
  CBitmapf = tmpfile();
  if(!CBitmapf)
    return 0;

  // INIZIALIZZA IL NUMERO DI PAGINE NEL FILE
  nPages = 0;
  fwrite(&nPages, sizeof(nPages), 1, CBitmapf);
  // LEGGE E COMPRIME TUTTI I BITMAPS
  while(!feof(Bitmap->Bitmapf) && CompressBitmap(Bitmap, CBitmapf)){
	nPages++;
  	fprintf(stderr,"bmcapt:CompressBitmaps %u page compressed\n", nPages);
}

  // SCRIVE IL NUMERO DI PAGINE NEL FILE
  fseek(CBitmapf, 0L, SEEK_SET);
  fwrite(&nPages, sizeof(nPages), 1, CBitmapf);
  fprintf(stderr,"bmcapt:CompressBitmaps %u pages found\n", nPages);
  fseek(CBitmapf, 0L, SEEK_SET);

  return CBitmapf;

} // END CompressBitmaps()



////////////////////////////////////////////////////////////////
// STAMPA DEL BITMAP IN FORMATO RAW CANON COMPRESSO SU STANDART OUTPUT
BOOL PrintBitmaps(FILE *CBitmapf)
{
  char buf[1024];
  int nRead;

  // RITORNA ALL' INIZIO DEL FILE TEMPORANEO
  fseek(CBitmapf, 0L, SEEK_SET);

  // COPIA IL FILE SU STDOUT
  while(!feof(CBitmapf))
  {
    nRead = fread(buf, 1, 1024, CBitmapf);
    fwrite(buf, 1, nRead, stdout);
  }

  return CAPT_PRINTER_OK;

} // END PrintBitmaps()
