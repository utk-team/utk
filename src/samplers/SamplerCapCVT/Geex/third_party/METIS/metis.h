/*
 * Copyright 1997, Regents of the University of Minnesota
 *
 * metis.h
 *
 * This file includes all necessary header files
 *
 * Started 8/27/94
 * George
 *
 * $Id: metis.h,v 1.3 2005/09/19 14:16:43 levy Exp $
 */


#include <stdio.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#ifndef WIN32
#include <strings.h>
#endif

#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#include <METIS/defs.h>
#include <METIS/struct.h>
#include <METIS/macros.h>
#include <METIS/rename.h>
#include <METIS/proto.h>

