#include "map_utility.h"
#include <Geex/combinatorics/map_builder.h>

namespace Geex {

	void normalize_map(Map* map) {
		vec3 bmin(1e10, 1e10, 1e10), bmax(-1e10, -1e10, -1e10) ;
		FOR_EACH_VERTEX(Map, map, vit) {
			bmin.x = gx_min(vit->point().x, bmin.x) ;
			bmin.y = gx_min(vit->point().y, bmin.y) ;
			bmin.z = gx_min(vit->point().z, bmin.z) ;
			bmax.x = gx_max(vit->point().x, bmax.x) ;
			bmax.y = gx_max(vit->point().y, bmax.y) ;
			bmax.z = gx_max(vit->point().z, bmax.z) ;
		}
		double lmax = gx_max(gx_max(bmax.x-bmin.x, bmax.y-bmin.y), bmax.z-bmin.z) ;
		
		FOR_EACH_VERTEX(Map, map, vit) {
			vit->point() = (vit->point()-bmin)/lmax ;
		}
	}

	void normalize_map_area(Map* map) {
		double area = 0 ;
		FOR_EACH_FACET(Map, map, fit) {
			area += fit->area() ;
		}

		vec3 bmin(1e10, 1e10, 1e10), bmax(-1e10, -1e10, -1e10) ;
		FOR_EACH_VERTEX(Map, map, vit) {
			bmin.x = gx_min(vit->point().x, bmin.x) ;
			bmin.y = gx_min(vit->point().y, bmin.y) ;
			bmin.z = gx_min(vit->point().z, bmin.z) ;
			bmax.x = gx_max(vit->point().x, bmax.x) ;
			bmax.y = gx_max(vit->point().y, bmax.y) ;
			bmax.z = gx_max(vit->point().z, bmax.z) ;
		}

		double len = sqrt(area) ;
		FOR_EACH_VERTEX(Map, map, vit) {
			vit->point() = bmin+(vit->point()-bmin)/len ;
		}
	}

	void get_bbox(Map* map, double& x_min, double& y_min, double& z_min, double& x_max, double& y_max, double& z_max) {
	    x_min = y_min = z_min =  1e30 ;
        x_max = y_max = z_max = -1e30 ;
        FOR_EACH_VERTEX(Map, map, v) {
            x_min = gx_min(x_min, v->point().x) ;
            y_min = gx_min(y_min, v->point().y) ;
            z_min = gx_min(z_min, v->point().z) ;
            x_max = gx_max(x_max, v->point().x) ;
            y_max = gx_max(y_max, v->point().y) ;
            z_max = gx_max(z_max, v->point().z) ;
        }	
	}

	void copy_map(Map* s, Map* t) {
		MapBuilder builder(t) ;
		MapVertexProperty<int> vindices ;
		int vidx = 0 ;
		t->clear() ;
		vindices.bind(s, "vidx_tmp") ;

		builder.begin_surface() ;
		FOR_EACH_VERTEX(Map, s, v) {
			vindices[v] = vidx++ ;
			builder.add_vertex(v->point()) ;
		}

		FOR_EACH_FACET(Map, s, f) {
			builder.begin_facet() ;
			Map::Halfedge* h = f->halfedge() ;
			do {
				builder.add_vertex_to_facet(vindices[h->vertex()]) ;
				h = h->next() ;
			} while(h!=f->halfedge()) ;
			builder.end_facet() ;
		}
		builder.end_surface() ;

		vindices.unbind() ;
	}

	void measure_quality(Map* map) {
		double amin = 180, amax=0, amin_ave=0 ;
		double qmin = 1, qave=0 ;
		double emin = 1e10, emax = 0 ;
		int    nle30 = 0 ;

		FOR_EACH_FACET(Map, map, f) {
			Map::Halfedge* h=f->halfedge() ;
			vec3 v1 = h->vertex()->point() ;
			vec3 v2 = h->next()->vertex()->point() ;
			vec3 v3 = h->next()->next()->vertex()->point() ;
			double l12 = distance(v1, v2) ;
			double l23 = distance(v2, v3) ;
			double l31 = distance(v3, v1) ;

			emin = gx_min(emin, gx_min(l12, gx_min(l23, l31))) ;
			emax = gx_max(emax, gx_max(l12, gx_max(l23, l31))) ;

			double amini=M_PI, amaxi=0 ;
			double a1 = acos(dot(normalize(v2-v1), normalize(v3-v1)))*180/M_PI ;
			double a2 = acos(dot(normalize(v3-v2), normalize(v1-v2)))*180/M_PI ;
			double a3 = acos(dot(normalize(v1-v3), normalize(v2-v3)))*180/M_PI ;
			amini = gx_min(a1, gx_min(a2, a3)) ;
			amaxi = gx_max(a1, gx_max(a2, a3)) ;

			amin_ave += amini ;
			amin = gx_min(amin, amini) ;
			amax = gx_max(amax, amaxi) ;
			if(amini<30) nle30 ++ ;

			real area = cross(v2-v1, v3-v1).length() ; //tri_area(v1, v2, v3) ; //F.vertex[0], F.vertex[1], F.vertex[2]) ;
			real Qi = 4*sqrt(3.0)*area/((l12+l23+l31)*emax) ;

			qmin = gx_min(qmin, Qi) ;
			qave += Qi ;
		}

		amin_ave /= map->nb_facets() ;
		qave /= map->nb_facets() ;

		std::cout << "Mesh geometry quality: " << std::endl ;
		std::cout << "  AngleMin = " << amin << std::endl ;
		std::cout << "  AngleMax = " << amax << std::endl ;
		std::cout << "  AverageMinAngle = " << amin_ave << std::endl ;
		std::cout << "  Qmin = " << qmin << std::endl ;
		std::cout << "  Qave = " << qave << std::endl ;
		std::cout << "  P30 = " << (double)nle30 / (map->nb_facets()*3.) << std::endl ;
		std::cout << "  EegeMin = " << emin  << std::endl ;
		std::cout << "  EdgeMax = " << emax  << std::endl ;
	}

	double facet_area(Map::Facet* f) {
		Map::Halfedge* h = f->halfedge() ;
		return 0.5*cross(h->next()->vertex()->point()-h->vertex()->point(), h->prev()->vertex()->point()-h->vertex()->point()).length() ;
	}

	vec3 facet_normal(Map::Facet* f) {
		Map::Halfedge* h = f->halfedge() ;
		return normalize(cross(h->next()->vertex()->point()-h->vertex()->point(), h->prev()->vertex()->point()-h->vertex()->point())) ;
	}

	double facet_min_angle(Map::Facet* f) {
		Map::Halfedge* h = f->halfedge() ;
		double angle_min = M_PI ;
		do {
			double angle = acos(dot(normalize(h->next()->vertex()->point()-h->vertex()->point()), normalize(h->prev()->vertex()->point()-h->vertex()->point()))) ;
			angle_min = gx_min(angle_min, angle) ;
			h = h->next() ;
		} while(h!=f->halfedge()) ;

		return angle_min*180/M_PI ;
	}

	vec3 center(Map::Facet* f) {
		vec3 g(0, 0, 0) ;
		Map::Halfedge* h=f->halfedge() ;
		do {
			g += h->vertex()->point() ;
			h = h->next() ;
		} while(h!=f->halfedge()) ;
		return g/f->degree() ;
	}

	void get_bbox(Map::Facet* f, vec3& bmin, vec3& bmax) {
		Map::Halfedge* h = f->halfedge() ;

	    bmin.x = bmin.y = bmin.z =  1e30 ;
        bmax.x = bmax.y = bmax.z =  -1e30 ;
		do {
            bmin.x = gx_min(bmin.x, h->vertex()->point().x) ;
            bmin.y = gx_min(bmin.y, h->vertex()->point().y) ;
            bmin.z = gx_min(bmin.z, h->vertex()->point().z) ;
            bmax.x = gx_max(bmax.x, h->vertex()->point().x) ;
            bmax.y = gx_max(bmax.y, h->vertex()->point().y) ;
            bmax.z = gx_max(bmax.z, h->vertex()->point().z) ;
			h = h->next() ;
        } while(h!=f->halfedge()) ;
	}
}

