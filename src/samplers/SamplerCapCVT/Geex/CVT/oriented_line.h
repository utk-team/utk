
#ifndef __ORIENTED_LINE__
#define __ORIENTED_LINE__

/* Further reading:
 * http://realtimecollisiondetection.net/blog/?p=13
 * (why we should do that differently)
 * Kensler06 - fast ray-triangle intersection.
 */

#include <Geex/CVT/geometry.h>

namespace Geex {

//_________________________________________________________

/**
 * OrientedLine implements plucker coordinates, which enables
 *  oriented lines to be compared. The comparison, implemented by the
 *  side() function, is a predicate similar to the right hand rule.
 * For instance, this class is used for the fast line-triangle intersection test.
 * (see D.M.Y. Sommerville, Analytical Geometry of Three Dimensions.
 *  Cambridge University Press, 1959).
 */
    template <class T> class OrientedLine {
    public:
        OrientedLine(const vec3g<T>& p, const vec3g<T>& q) ;
        OrientedLine() ;

        T pi_[6] ;
    } ;

//_________________________________________________________

    template <class T>
    inline OrientedLine<T>::OrientedLine(
        const vec3g<T>& p, const vec3g<T>& q
    ) {
        // There are several conventions for defining plucker coordinates,
        // this one is introduced in : Marco Pellegrini, Stabbing and 
        // ray-shooting in 3-dimensional space. In Proc. 6th ACM Symposium
        // on Computational Geometry, pages 177-186, 1990.
        // I think that it is possible to have a more symmetric formulation
        // of plucker coordinates, leading to a more symmetric side() 
        // predicate, but I have no time to investigate this.
        pi_[0] = p.x*q.y - p.y*q.x ;
        pi_[1] = p.x*q.z - p.z*q.x ;
        pi_[2] = p.x - q.x ;
        pi_[3] = p.y*q.z - p.z*q.y ;
        pi_[4] = p.z - q.z ;
        pi_[5] = q.y - p.y ;
    }

    template <class T>
    inline OrientedLine<T>::OrientedLine() {
        pi_[0] = T(0) ;
        pi_[1] = T(0) ;
        pi_[2] = T(0) ;
        pi_[3] = T(0) ;
        pi_[4] = T(0) ;
        pi_[5] = T(0) ;
    }


    /**
     * "right hand rule" like predicate.
     */
    template <class T1, class T2>
    inline T2 side(
        const OrientedLine<T1>& a, 
        const OrientedLine<T2>& b
    ) {
        // Note: the order might seem strange, but product between
        //  lines in plucker coordinates is a permuted cross product.
        return 
            a.pi_[0] * b.pi_[4] +
            a.pi_[1] * b.pi_[5] +
            a.pi_[2] * b.pi_[3] +
            a.pi_[4] * b.pi_[0] +
            a.pi_[5] * b.pi_[1] +
            a.pi_[3] * b.pi_[2] ;
    }

//_________________________________________________________

}

#endif
