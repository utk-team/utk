#ifndef __UTILITY_H__ 	
#define __UTILITY_H__

#include <Geex/combinatorics/map.h>

namespace Geex {
	void GEEX_API normalize_map(Map* map) ;
	void GEEX_API normalize_map_area(Map* map) ;
	void GEEX_API get_bbox(Map* map, double& xmin, double& ymin, double& zmin, double& xmax, double& ymax, double& zmax) ;
	void GEEX_API copy_map(Map* s, Map* t) ;
	void GEEX_API measure_quality(Map* map) ;
	
	void GEEX_API get_bbox(Map::Facet* f, vec3& bmin, vec3& bmax) ;
	vec3 GEEX_API center(Map::Facet* f) ;
	double GEEX_API facet_area(Map::Facet* f) ;
	vec3   GEEX_API facet_normal(Map::Facet* f) ;
	double GEEX_API facet_min_angle(Map::Facet* f) ;
}

#endif
