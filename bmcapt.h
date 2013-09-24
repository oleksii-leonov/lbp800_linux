/*  Linux Canon LBP800 CAPT driver
 *  bitmap compression lbp800 CAPT routines header file
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
#ifndef		__BMCAPT_H
#define		__BMCAPT_H

// DIMENSIONE (IN BYTES) DI UNA LINEA
#define BYTES_BY_LINE		592

// NUMERO DI LINEE PER PAGINA
#define LINES_BY_PAGE		6968

// NUMERO DI RIGHE IN UNA 'BAND'
#define LINES_BY_BAND  		104

// STRUTTURA PER UN BUFFER DI 2 LINEE
typedef BYTE TLineBuf[2][BYTES_BY_LINE];

// STRUTTURA PER LA GESTIONE DEL BITMAP CONTENENTE L' IMMAGINE
typedef struct
{
  // IL FILE CONTENENTE IL BITMAP
  FILE *Bitmapf;

  // LARGHEZZA ED ALTEZZA DEL BITMAP
  int Width, Height;

  // RITAGLI SINISTRO E SUPERIORE, SE PRESENTI
  int LeftSkip, TopSkip;

} TSourceBitmap;

////////////////////////////////////////////////////////////////
// SKIP DI UN NUMERO DI BYTES NEL BITMAP
// NECESSARIO PERCHE' POTENDO PROVENIRE DA UNA 'PIPE'
// NON E' POSSIBILE USARE LA FUNZIONE seek
void Bitmap_Skip (FILE *Bitmapf, int Offset);

////////////////////////////////////////////////////////////////
// LETTURA DI UNA LINEA DAL FILE DEL BITMAP
void Bitmap_GetLine(TSourceBitmap *Bitmap, BYTE *LinePtr);

////////////////////////////////////////////////////////////////
// RITORNA LA POSIZIONE DELL' ULTIMO BYTE DIFFERENTE
// TRA LA LINEA CORRENTE E LA LINEA PRECEDENTE (+1)
int getLastDifference(TLineBuf LineBuf, BYTE CurLine);

////////////////////////////////////////////////////////////////
// COMPRESSIONE DEL BITMAP
BOOL CompressBitmap(TSourceBitmap *Bitmap, FILE *CBitmapf);

////////////////////////////////////////////////////////////////
// COMPRESSIONE DI TUTTE LE PAGINE DI STAMPA
FILE *CompressBitmaps(TSourceBitmap *Bitmap);

////////////////////////////////////////////////////////////////
// STAMPA DEL BITMAP SU STANDARD OUTPUT
BOOL PrintBitmaps(FILE *CBitmapf);

#endif		//__BMCAPT_

