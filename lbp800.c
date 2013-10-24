/*
 *  Linux Canon LBP800 CAPT driver
 *  main file
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

#include "machine.h"
#include "errcapt.h"
#include "bmcapt.h"

#include "lbp800.h"

////////////////////////////////////////////////////////////////
// PROGRAMMA PRINCIPALE
int main(int argc, char** argv)
{
  int c;
  fprintf(stderr,"lbp800:main is here\n");
  // BITMAP SORGENTE
  TSourceBitmap Bitmap;

  // IL FILE TEMPORANEO CONTENENTE IL BITMAP COMPRESSO
  FILE *CBitmapf;

  // INIZIALIZZA I VALORI DI DEFAULT
  Bitmap.Bitmapf = stdin;
  Bitmap.TopSkip = 0;
  Bitmap.LeftSkip = 0;

  // LEGGE GLI ARGOMENTI ED IMPOSTA I PARAMETRI RICHIESTI
  while ((c = getopt(argc,argv,"t:l:f:d:")) != -1)
  {
    switch (c)
    {
      // TOP SKIP
      case 't' :
        sscanf(optarg, "%d", &Bitmap.TopSkip);
        break;

      // LEFT SKIP
      case 'l' :
        sscanf(optarg, "%d", &Bitmap.LeftSkip);
        break;

      // FILE BITMAP
      case 'f' :
        Bitmap.Bitmapf = fopen(optarg, "r");
        fprintf(stderr,"lbp800:main Open file %s\n", optarg);
        if (!Bitmap.Bitmapf)
        {
          fprintf(stderr,ErrorMessages[CAPT_NoBitmap], optarg);
          exit(4);
        }
        break;

      default:
        fprintf(stderr, "Usage: %s [-t topskip] [-l leftskip] [-f file]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    } // END switch()
  } // END while


  // COMPRIME IL BITMAP
  if( (CBitmapf = CompressBitmaps(&Bitmap)) != NULL)
  {
    fseek(CBitmapf, 0L, SEEK_SET);

  	// STAMPA IL BITMAP
  	PrintBitmaps(CBitmapf);

  	// CHIUDE IL BITMAP COMPRESSO
  	fclose(CBitmapf);

  } // if CompressBitmap

  // CHIUDE IL BITMAP SORGENTE
  if(Bitmap.Bitmapf != stdin)
    fclose(Bitmap.Bitmapf);

  // RITORNA TUTTO OK
  return 0;

} // END Main()

