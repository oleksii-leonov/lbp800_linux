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
#ifndef		__TIMCAPT_H
#define		__TIMCAPT_H

#include <sys/time.h>

typedef struct
{
  struct timeval StartTime;
  long Delay;
  BOOL Done;
	
} TTimeId;

///////////////////////////////////////////////////////////////
// GESTIONE DI UN TIMER SOFTWARE
// INIZIALIZZA IL TIMER CON UN RITARDO FORNITO
// LA GRANULARITA' NON E' SPECIFICATA, SI SPERA SIA SOTTO IL MILLISECONDO
void StartTimer(TTimeId *ID, long uSec);

///////////////////////////////////////////////////////////////
// GESTIONE DI UN TIMER SOFTWARE
// CONTROLLA SE IL TEMPO SPECIFICATO E' PASSATO
// LA GRANULARITA' NON E' SPECIFICATA, SI SPERA SIA SOTTO IL MILLISECONDO
BOOL Timeout(TTimeId *ID);

#endif		//__TIMCAPT_H
