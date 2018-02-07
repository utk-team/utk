/**
 * This file is part of the optimization tool fpo which implements the
 * farthest point optimization method proposed in the paper
 *
 *   T. Schlömer, D. Heck, O. Deussen:
 *   Farthest-Point Optimized Point Sets with Maximized Minimum Distance
 *   HPG '11: High Performance Graphics Proceedings
 * 
 * fpo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>

#include "MersenneTwister.h"

#define SQRT3   1.73205080756887729352

inline double sqr_dist_unit_torus(const Point_2 &p0, const Point_2 &p1) {
    double x = fabs(p0.x() - p1.x());
    double y = fabs(p0.y() - p1.y());
    x = (x > 0.5) ? 1.0 - x : x;
    y = (y > 0.5) ? 1.0 - y : y;
    return x*x + y*y;
}

inline Point_2 wrap_unit_torus(const Point_2 &p) {
    double x = p.x() < 0 ? p.x() + 1.0 : (p.x() >= 1.0 ? p.x() - 1.0 : p.x());
    double y = p.y() < 0 ? p.y() + 1.0 : (p.y() >= 1.0 ? p.y() - 1.0 : p.y());
    return Point_2(x, y);
}

inline bool is_in_unit_torus(const Point_2 &p) {
    return 0.0 <= p.x() && p.x() < 1.0 &&
           0.0 <= p.y() && p.y() < 1.0;
}

inline bool is_in_rect(const Point_2 &p,
                       const Point_2 &clip0, const Point_2 &clip1) {
    return clip0.x() <= p.x() && p.x() <= clip1.x() &&
           clip0.y() <= p.y() && p.y() <= clip1.y();
}

inline void shuffle(unsigned *A, const unsigned N, MTRand &rng)
{
    for (unsigned i = 0; i < N-1; ++i) {
        unsigned r = i + rng.randInt(N-1 - i);
        std::swap(A[i], A[r]);
    }
}

