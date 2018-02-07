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

#ifndef DT_H
#define DT_H

#include "../../pointsets/Pointset.hpp"
#include <set>
#include <vector>
using std::vector;

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>


// CGAL Types and helper structs
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Triangulation_vertex_base_2<K>                 Tvb;

typedef K::Circle_2  Circle_2;
typedef K::Point_2   Point_2;

struct Node;
typedef std::set<Node> FaceTree;

// Collects information about a Delaunay face
struct FaceInfo {
    Circle_2 circle;            // Circumcircle
    FaceTree::iterator node;    // Reference to node in sorting tree
    int id;                     // A unique ID to break ties when sorting
    bool main_face;             // True if center lies inside unit square
    
    FaceInfo() { circle = Circle_2(Point_2(0, 0), 0); id = ++maxid; }
    static int maxid;
};

typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, K>  Tfb;
typedef CGAL::Triangulation_data_structure_2<Tvb,Tfb>           Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds>                  CDT;
typedef CDT::Vertex_handle  VH;
typedef CDT::Face_handle    FH;

// Tree node that compares faces by the radius of their circumcircle
struct Node {
    FH face;
    bool operator< (const Node &n) const {
        const double &r1 =   face->info().circle.squared_radius();
        const double &r2 = n.face->info().circle.squared_radius();
        return (r1 == r2) ? (face->info().id > n.face->info().id) : (r1 > r2);
    }
};

// Subsumes a Delaunay vertex and its toroidal replications
struct Site {
    Site() : vertex(0), nreplications(0) {};
    VH vertex;
    VH replications[8];  // In 2D there can be a maximum of eight replications
    unsigned nreplications;
};


// This class implements a Delaunay triangulation based on the one provided by
// CGAL. It extends the CGAL implementation to handle periodic boundary
// conditions, and concurrently maintains a binary tree to efficiently
// determine the largest circumcircle of a face in the triangulation.
class DT
{
private:
    CDT dt;                // The CGAL delaunay triangulation
    vector<Site> sites;    // The vertices and their replications
    FaceTree sorted_faces; // Faces sorted by the size of their circumcircle
    
    Point_2 clip[2];          // Optional clipping box for vertex replications
    vector<FH> conflict_faces;// Faces helper array for conflict checks
    vector<CDT::Edge> conflict_edges;// Edges helper array for conflict checks
    
public:
    // Constructor. The DT is immediately created from the given set of
    // points. The clip heuristic clips toroidal vertex replications that are
    // far from the boundary and will not influence it.
    DT(const vector<Point_2> &points, bool clip_heuristic = false);
    ~DT();
    
    // Determines the global largest circumcircle by utilizing the sorted tree
    // of faces. 'face' is filled with the corresponding face handle.
    Circle_2 global_largest_circumcircle(FH &face) const;
    
    // Determines the global largest circumcircle by simply iterating over all
    // faces. 'face' is filled with the corresponding face handle.
    Circle_2 global_largest_circumcircle_bruteforce(FH &face) const;
    
    // Determines the local largest circumcircle by iterating over all faces
    // incident to one of the vertices in 'neighbors'. 'face' is filled with
    // the corresponding face handle.
    Circle_2 local_largest_circumcircle(const vector<VH> &neighbors, FH &face) const;
    
    // Sets vertex 'i' in the triangulation, i.e. inserts a point at the given
    // location, and up to 8 toroidal replications. 'face' is used as a hint
    // on where to insert the main point. 'setup' is for initial construction.
    void set_vertex(unsigned i, const Point_2 &point, const FH &face,
                    bool setup = false);
    
    // Clears vertex 'i', i.e. removes it and its replications from the DT
    void clear_vertex(unsigned i);
    
    // Determines all vertices that are adjacent to the vertex 'i'
    void incident_vertices(unsigned i, vector<VH> &handles) const;
    
    // Some inline functions
    inline VH get_vertex(unsigned i) { return sites[i].vertex; };
    inline unsigned number_of_vertices() const { return sites.size(); };
    
    // Some utility functions
    void get_statistics(double &global_md, double &avg_md) const;
    void save_vertices(utk::Pointset< 2, double, utk::Point<2, double> >& pts) const;
    void save_vertices(const char *fname) const;
    
private:
    // Inserts a point into the DT and updates affected faces accordingly.
    // 'face' may be used as a hint on where to insert the point. The CGAL
    // vertex handle is returned.
    VH insert_vertex(const Point_2 &p, const FH &face);
    
    // Removes a vertex from the DT and updates affected faces accordingly
    void remove_vertex(VH v);
    
    // Removes a face from the tree of sorted faces
    void invalidate_face(const FH &face);
    
    // Updates FaceInfo for a given face
    void update_face(const FH &face);
};

#endif
