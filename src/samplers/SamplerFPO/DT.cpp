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

#include "DT.h"

#include <algorithm>
#include <fstream>

#include "util.h"

int FaceInfo::maxid = 0;


DT::DT(const vector<Point_2> &points, bool clip_heuristic)
{
    // Clipping heuristic that excludes replications that probably do not
    // influence the boundary
    if (clip_heuristic) {
        const double e = 4.0 / sqrt(points.size());
        clip[0] = Point_2(-e, -e);
        clip[1] = Point_2(1.0 + e, 1.0 + e);
    } else {
        clip[0] = Point_2(-1, -1);
        clip[1] = Point_2(2, 2);
    }
    
    // Prepare storage
    sites.resize(points.size());

    // Add each point as a vertex to the triangulation
    for (unsigned i = 0; i < points.size(); ++i)
        set_vertex(i, points[i], FH(), true);
    
    // Initialize the sorting tree
    CDT::Face_iterator fc;
    for (fc = dt.faces_begin(); fc != dt.faces_end(); ++fc)
        update_face(fc);
}

DT::~DT()
{
    
}

Circle_2 DT::global_largest_circumcircle(FH &face) const
{
    face = sorted_faces.begin()->face;
    return face->info().circle;
}

Circle_2 DT::global_largest_circumcircle_bruteforce(FH &face) const
{
    Circle_2 largest(Point_2(0, 0), 0);
    CDT::Face_iterator fi;
    for (fi = dt.faces_begin(); fi != dt.faces_end(); ++fi) {
        if (dt.is_infinite(fi)) continue;
        if (!fi->info().main_face) continue;
        
        Circle_2 c = fi->info().circle;
        if (c.squared_radius() > largest.squared_radius()) {
            largest = c;
            face = fi;
        }
    }
    return largest;
}

Circle_2 DT::local_largest_circumcircle(const vector<VH> &neighbors, FH &face) const
{
    Circle_2 largest(Point_2(0, 0), 0);
    for (unsigned i = 0; i < neighbors.size(); ++i) {
        CDT::Face_circulator fc = dt.incident_faces(neighbors[i]), done(fc);
        if (fc == 0) continue;
        
        do {
            if (dt.is_infinite(fc)) continue;
            
            Circle_2 c = fc->info().circle;
            if (c.squared_radius() > largest.squared_radius()) {
                largest = c;
                face = fc;
            }
        } while (++fc != done);
    }
    return largest;
}

void DT::set_vertex(unsigned i, const Point_2 &point, const FH &face,
                    bool setup)
{
    Site site;
    
    // Insert original vertex
    VH v = setup ? dt.insert(point) : insert_vertex(point, face);
    site.vertex = v;
    
    // Insert replications
    for (int u = -1; u <= 1; ++u) {
        for (int v = -1; v <= 1; ++v) {
            if (u != 0 || v != 0) {
                Point_2 p(point.x() + u, point.y() + v);
                if (is_in_rect(p, clip[0], clip[1])) {
                    // Locate face where replicate would be inserted
                    CDT::Locate_type lt;
                    int li;
                    FH loc = dt.locate(p, lt, li);
                    // Skip replicate if it would duplicate an existing vertex
                    if (lt == CDT::VERTEX) continue;
                    // Insert replicate
                    VH r = setup ? dt.insert(p, lt, loc, li)
                                 : insert_vertex(p, FH());
                    site.replications[site.nreplications] = r;
                    site.nreplications++;
                }
            }
        }
    }
    
    sites[i] = site;
}

void DT::clear_vertex(unsigned i)
{
    // Remove replications
    for (unsigned r = 0; r < sites[i].nreplications; ++r)
        remove_vertex(sites[i].replications[r]);
    sites[i].nreplications = 0;
    
    // Remove original vertex
    remove_vertex(sites[i].vertex);
    sites[i] = Site();
}

void DT::incident_vertices(unsigned i, vector<VH> &handles) const
{
    CDT::Vertex_circulator vc = dt.incident_vertices(sites[i].vertex), done(vc);
    do {
        handles.push_back(vc);
    } while (++vc != done);
}

void DT::get_statistics(double &global_md, double &avg_md) const
{
    global_md  = DBL_MAX;
    double sum = 0.0;
    
    // Determines local mindist for each site and updates global and sum of
    // mindists accordingly
    for (unsigned i = 0; i < sites.size(); ++i) {
        double local_md = DBL_MAX;
        CDT::Vertex_circulator vc = dt.incident_vertices(sites[i].vertex),
                                    done(vc);
        if (vc != 0) {
            do {
                double d = CGAL::squared_distance(sites[i].vertex->point(),
                                                  vc->point());
                local_md = std::min(local_md, d);
            } while (++vc != done);
        }
        global_md = std::min(local_md, global_md);
        sum += sqrt(local_md);
    }
    
    global_md = sqrt(global_md);
    avg_md    = sum / sites.size();
}

void DT::save_vertices(utk::Pointset< 2, double, utk::Point<2, double> >& pts) const
{
	pts.setUnitToroidalDomain();
	for (unsigned i = 0; i < sites.size(); ++i)
    {
		utk::Point< 2, double > p;
		p.pos()[0] = sites[i].vertex->point().x();
		p.pos()[1] = sites[i].vertex->point().y();
		pts.push_back(p);
	}
}

void DT::save_vertices(const char *fname) const
{
    FILE *fp;
    if (!(fp = fopen(fname, "wb"))) {
        fprintf(stderr, "Cannot write point set to %s!\n", fname);
        return;
    }
    
    for (unsigned i = 0; i < sites.size(); ++i)
        fprintf(fp, "%.8f %.8f\n", sites[i].vertex->point().x(), sites[i].vertex->point().y());

    fclose(fp);
}

VH DT::insert_vertex(const Point_2 &p, const FH &face)
{
    VH v;
    
    if (dt.number_of_vertices() > 2) {
        // Determine faces that will be affected by the insert operation
        conflict_faces.clear();
        conflict_edges.clear();
        dt.get_conflicts_and_boundary(p, std::back_inserter(conflict_faces), 
                                      std::back_inserter(conflict_edges),
                                      face);
        
        // Invalidate these faces
        vector<FH>::iterator it;
        for (it = conflict_faces.begin(); it != conflict_faces.end(); ++it)
            invalidate_face(*it);
        
        // Insert point as new vertex using the information we already have
        v = dt.star_hole(p, conflict_edges.begin(), conflict_edges.end(),
                         conflict_faces.begin(), conflict_faces.end());
    } else {
        v = dt.insert(p, face);
    }
    
    // Update FaceInfo of all adjacent faces
    CDT::Face_circulator f = dt.incident_faces(v), fend = f;
    if (f != 0) {
        do {
            update_face(f);
        } while (++f != fend);
    }
    
    return v;
}

void DT::remove_vertex(VH v)
{
    // Invalidate adjacent faces
    Point_2 p = v->point();
    CDT::Face_circulator f = dt.incident_faces(v), fend = f;
    if (f != 0) {
        do {
            invalidate_face(f);
        } while (++f != fend);
    }
    
    // Remember one neighbor for faster conflict check below
    VH neighbor = dt.incident_vertices(v);
    
    // Remove the vertex
    dt.remove(v);
    
    // Determine faces that were created or affected by the remove operation
    conflict_faces.clear();
    conflict_edges.clear();
    FH face = dt.incident_faces(neighbor);
    dt.get_conflicts_and_boundary(p, std::back_inserter(conflict_faces), 
                                  std::back_inserter(conflict_edges), face);
    
    // Update these faces
    vector<FH>::iterator it;
    for (it = conflict_faces.begin(); it != conflict_faces.end(); ++it)
        update_face(*it);
}

void DT::invalidate_face(const FH &face)
{
    // Clears FaceInfo and removes a main face from the sorting tree
    FaceInfo &info = face->info();
    if (info.node != FaceTree::iterator())
        sorted_faces.erase(info.node);
    info.circle = Circle_2(Point_2(0, 0), 0);
    info.node = FaceTree::iterator();
}

void DT::update_face(const FH &face)
{
    if (dt.is_infinite(face)) return;
    
    // Update FaceInfo for the given face
    Point_2 c = dt.circumcenter(face);
    double r = CGAL::squared_distance(c, face->vertex(0)->point());
    
    FaceInfo &info = face->info();
    info.circle = Circle_2(c, r);
    info.main_face = is_in_unit_torus(c);
    
    // Only main faces are linked to the sorting tree
    if (info.main_face) {
        Node n;
        n.face = face;
        std::pair<FaceTree::iterator,bool> x = sorted_faces.insert(n);
        if (x.second)
            info.node = x.first;
    }
}
