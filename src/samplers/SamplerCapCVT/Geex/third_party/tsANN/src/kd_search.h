//----------------------------------------------------------------------
// File:			kd_search.h
// Programmer:		Sunil Arya and David Mount
// Description:		Standard kd-tree search
// Last modified:	01/04/05 (Version 1.0)
//----------------------------------------------------------------------
// Copyright (c) 1997-2005 University of Maryland and Sunil Arya and
// David Mount.  All Rights Reserved.
// 
// This software and related documentation is part of the Approximate
// Nearest Neighbor Library (ANN).  This software is provided under
// the provisions of the Lesser GNU Public License (LGPL).  See the
// file ../ReadMe.txt for further information.
// 
// The University of Maryland (U.M.) and the authors make no
// representations about the suitability or fitness of this software for
// any purpose.  It is provided "as is" without express or implied
// warranty.
//----------------------------------------------------------------------
// History:
//	Revision 0.1  03/04/98
//		Initial release
//  Revision 1.1.ts 16/03/09
//    Sylvain Lefebvre - thread safe version (removed globals used for recursion)
//----------------------------------------------------------------------

#ifndef ANN_kd_search_H
#define ANN_kd_search_H

#include "kd_tree.h"					// kd-tree declarations
#include "kd_util.h"					// kd-tree utilities
#include "pr_queue_k.h"					// k-element priority queue

#include <ANN/ANNperf.h>				// performance evaluation

//----------------------------------------------------------------------
// SL: thread safe version (removed globals used for recursion), embbed search parameters in struct
//----------------------------------------------------------------------

struct s_kSearchParams {
  int			       ANNkdDim;				// dimension of space
  ANNpoint	     ANNkdQ;					// query point
  double		     ANNkdMaxErr;			// max tolerable squared error
  ANNpointArray	 ANNkdPts;				// the points
  ANNmin_k		  *ANNkdPointMK;		// set of k closest points
  int	           ANNptsVisited;		// number of pts visited in search
};

//----------------------------------------------------------------------

#endif
