#ifndef _TYPES_H_
#define _TYPES_H_ 1

// CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_2.h>

// local
#include "primitives.h"
#include "domain.h"
#include "rt2.h"

#undef min
#undef max

//typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

// Basic types
typedef Kernel::FT         FT;
typedef Kernel::Point_2    Point;
typedef Kernel::Vector_2   Vector;
typedef Kernel::Ray_2      Ray;
typedef Kernel::Line_2     Line;
typedef Kernel::Segment_2  Segment;
typedef Kernel::Triangle_2 Triangle;

// Domain
typedef CDomain<Kernel> Domain;

// Traits
typedef CGAL::Regular_triangulation_filtered_traits_2<Kernel> Traits;
typedef Traits::Weighted_point_2 Weighted_point;
typedef Traits::Weight Weight;

// Vertex
typedef CGAL::Regular_triangulation_vertex_base_2<Traits> RVb;
typedef My_vertex_base<Traits, RVb> MVb;

// Face
typedef CGAL::Regular_triangulation_face_base_2<Traits> RFb;
typedef My_face_base<Traits, RFb> MFb;

// Triangulation
typedef CGAL::Triangulation_data_structure_2<MVb, MFb> TDS;
typedef CGAL::Regular_triangulation_2<Traits, TDS> Regular_triangulation;
typedef CTriangulation<Regular_triangulation> RT;

typedef RT::Vertex                   Vertex;
typedef RT::Vertex_handle            Vertex_handle;
typedef RT::Vertex_iterator          Vertex_iterator;
typedef RT::Vertex_circulator        Vertex_circulator;
typedef RT::Finite_vertices_iterator Finite_vertices_iterator;
typedef RT::Hidden_vertices_iterator Hidden_vertices_iterator;

typedef RT::Edge                  Edge;
typedef RT::Edge_iterator         Edge_iterator;
typedef RT::Edge_circulator       Edge_circulator;
typedef RT::Finite_edges_iterator Finite_edges_iterator;

typedef RT::Face                  Face;
typedef RT::Face_handle           Face_handle;
typedef RT::Face_iterator         Face_iterator;
typedef RT::Face_circulator       Face_circulator;
typedef RT::Finite_faces_iterator Finite_faces_iterator;

#endif
