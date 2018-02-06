// Copyright (c) 2007  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.4-branch/Spatial_sorting/include/CGAL/spatial_sort.h $
// $Id: spatial_sort.h 36875 2007-03-07 11:37:05Z spion $
//
// Author(s)     : Christophe Delage

//Modified by liuyang

#ifndef CGAL_CVT_SPATIAL_SORT_H
#define CGAL_CVT_SPATIAL_SORT_H

#include <CGAL/basic.h>

#include "CVT_Hilbert_sort_2.h"
#include "CVT_Hilbert_sort_3.h"

#include <CGAL/Multiscale_sort.h>

CGAL_BEGIN_NAMESPACE

//namespace CGALi {

	template <class RandomAccessIterator, class Kernel>
	void CVT_spatial_sort_2 (RandomAccessIterator begin, RandomAccessIterator end,
		const Kernel &k)
	{
		typedef std::iterator_traits<RandomAccessIterator> ITraits;
		typedef typename ITraits::value_type               value_type;

		typedef CVT_Hilbert_sort_2<value_type, Kernel> Sort;
		(Multiscale_sort<Sort> (Sort (k, 4), 16, 0.25)) (begin, end);
	}

	template <class RandomAccessIterator, class Kernel>
	void CVT_spatial_sort_3 (RandomAccessIterator begin, RandomAccessIterator end,
		const Kernel &k)
	{
		typedef std::iterator_traits<RandomAccessIterator> ITraits;
		typedef typename ITraits::value_type               value_type;

		typedef CVT_Hilbert_sort_3<value_type, Kernel> Sort;
		(Multiscale_sort<Sort> (Sort (k, 8), 64, 0.125)) (begin, end);
	}
//}

//template <class RandomAccessIterator, class Kernel>
//void CVT_spatial_sort (RandomAccessIterator begin, RandomAccessIterator end,
//				   const Kernel &k)
//{
//	typedef std::iterator_traits<RandomAccessIterator> ITraits;
//	typedef typename ITraits::value_type               value_type;
//
//	CGALi::CVT_spatial_sort (begin, end, k, static_cast<value_type *> (0));
//}

//template <class RandomAccessIterator>
//void CVT_spatial_sort (RandomAccessIterator begin, RandomAccessIterator end)
//{
//	typedef std::iterator_traits<RandomAccessIterator> ITraits;
//	typedef typename ITraits::value_type               value_type;
//	typedef CGAL::Kernel_traits<value_type>            KTraits;
//	typedef typename KTraits::Kernel                   Kernel;
//
//	CVT_spatial_sort (begin, end, Kernel());
//}

CGAL_END_NAMESPACE

#endif//CGAL_SPATIAL_SORT_H
