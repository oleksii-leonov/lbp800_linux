/*
 *  Linux Canon LBP800 CAPT driver
 *  time routines header file
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

#include "machine.h"
#include "timcapt.h"

///////////////////////////////////////////////////////////////
// GESTIONE DI UN TIMER SOFTWARE
// INIZIALIZZA IL TIMER CON UN RITARDO FORNITO
// LA GRANULARITA' NON E' SPECIFICATA, SI SPERA SIA SOTTO IL MILLISECONDO
void StartTimer(TTimeId *ID, long uSec)
{
  // LEGGE IL TEMPO CORRENTE E LO MEMORIZZA
  gettimeofday(&ID->StartTime, NULL);
  
  // SALVA IL RITARDO RICHIESTO
  ID->Delay = uSec;
  
  // SEGNALA CHE IL TIMER E' ATTIVO
  ID->Done = 0;
	
} // END StartTimer()

///////////////////////////////////////////////////////////////
// GESTIONE DI UN TIMER SOFTWARE
// CONTROLLA SE IL TEMPO SPECIFICATO E' PASSATO
// LA GRANULARITA' NON E' SPECIFICATA, SI SPERA SIA SOTTO IL MILLISECONDO
BOOL Timeout(TTimeId *ID)
{
  struct timeval t;
  long diff;
  
  // SE TIMER GIA' SCADUTO, RITORNA TRUE
  if(ID->Done)
    return 1;
    
  // CONTROLLA IL TIMER
  gettimeofday(&t, NULL);
  
  // CALCOLA LA DIFFERENZA IN MICROSECONDI
  diff = t.tv_sec - ID->StartTime.tv_sec;
  diff = diff * 1000000L + t.tv_usec - ID->StartTime.tv_usec;
  
  // SE LA DIFFERENZA E' MAGGIORE DEL TEMPO RICHIESTO,
  // SEGNALA TIMER SCADUTO
  if(diff >= ID->Delay)
  {
  	ID->Done = 1;
  	return 1;
  }
  return 0;
	
} // END Timeout();


