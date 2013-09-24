/*
 *  Linux Canon LBP800 CAPT driver
 *  lbp800 driver error messages
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
#include <stdarg.h>
#include <stdio.h>

#include "errcapt.h"

////////////////////////////////////////////////////////////////
// MESSAGGI DI ERRORE
char *ErrorMessages[] =
{
  "Nessun errore"
  ,"Impossibile aprire il device '%s'\n"
  ,"La stampante non risponde\n"
  ,"File '%s' non trovato o illeggibile.\n"
  ,"Formato del bitmap errato.\n"
  ,"Stampa pagina %d in corso.\n"

}; // END ErrorMessagesp[]

////////////////////////////////////////////////////////////////
// FUNZIONE PER INSERIRE UN MESSAGGIO IN UN FILE DI LOG
void LBP800Log(char *fmt, ...)
{
  FILE *LogFile;

  LogFile = fopen("/tmp/LBP800.log", "a");
  va_list args;
  va_start( args, fmt );
  vfprintf(LogFile, fmt, args );
  va_end( args );
  fclose(LogFile);

} // END LBP800Log()
