#include "scene.h"

/* this computes the variance in the distance to each 
 * what are the implications of using squared distance rather than
 * actual distance? would that be better/more sensitive? my impression
 * of the lucarini was they used actual distance, but will double check.
 */

/*
FT Scene::compute_regularity_score(Vertex_handle vertex) const
{
    std::vector<double> adj_vert_distances;
    FT mean_dist = 0.;
    
    Vertex_circulator vcirc = m_rt.incident_vertices(vertex);
    Vertex_circulator vend  = vcirc;
    
    CGAL_For_all(vcirc, vend)
    {
        Vertex_handle vi = vcirc;
        FT this_dist = sqrt(CGAL::squared_distance(vertex->get_position(), vi->get_position()));
        adj_vert_distances.push_back(this_dist);
        mean_dist += this_dist;
    }
    
    mean_dist = mean_dist / (FT) adj_vert_distances.size();
    
    FT temp = 0.;
    
    // sample variance (1/N) Sum (xi - mean)^2
    for(unsigned int i = 0; i < adj_vert_distances.size(); ++i)
    {
        temp = temp + (adj_vert_distances[i] - mean_dist)*(adj_vert_distances[i] - mean_dist);
    }
    
    return temp / ((FT) adj_vert_distances.size());
}

// this is RIDICULOUSLY inefficient; I wasn't really thinking ahead to performance 
// at all when I wrote this. We should definitely precompute all of the regularity
// scores and store them somewhere and then just do a lookup. :-)
//
bool Scene::is_aliased(Vertex_handle vi, double gamma) const
{
    std::vector<double> adj_vert_scores;
    double mean_score = 0.;
    
    double center_score = compute_regularity_score(vi);
    adj_vert_scores.push_back(center_score);
    mean_score += center_score;
    
    Vertex_circulator vcirc = m_rt.incident_vertices(vi);
    Vertex_circulator vend  = vcirc;
    
    CGAL_For_all(vcirc, vend)
    {
        Vertex_handle vj = vcirc;
        double this_score = compute_regularity_score(vj);
        adj_vert_scores.push_back(this_score);
        
        mean_score += this_score;
    }
    
    mean_score /= (double) adj_vert_scores.size();
    
    double temp = 0.;
    
    for(unsigned int i = 0; i < adj_vert_scores.size(); ++i)
    {
        temp += (adj_vert_scores[i] - mean_score) * (adj_vert_scores[i] - mean_score);
    }
    
    double variance = temp / ((double) adj_vert_scores.size());
    
    if(variance < gamma) return true;
    else return false;
}

// sorts and removes repeated vertices and out-of-bounds vertices
void Scene::cleanup_vec(std::vector<Vertex_handle> &vec) 
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    
    std::vector<Vertex_handle> new_vec;
    
    for (unsigned i = 0; i < vec.size(); ++i)
    {
        // did we somehow remove this vertex handle from m_vertices? if so, ignore.
        if(std::find(m_vertices.begin(), m_vertices.end(), vec[i]) == m_vertices.end())
            continue;
        
        // is the vertex out of bounds? if so, we don't need to jitter.
        // FERNANDO - is this right? I may be messing up the toroid by doing this.
        // Perhaps remove this line. (I know there are duplicated points over the boundaries.)
        if(m_domain.is_outside(vec[i]->get_position()))
            continue;
        
        new_vec.push_back(vec[i]);
    }
    vec = new_vec;
}

void Scene::break_regularity(double gamma, int max_to_remove)
{
    std::vector<Vertex_handle> new_vertices;
    std::vector<Vertex_handle> verts_to_jitter;
    std::set<int> indices_for_deletion;

    // collect n aliased sites.
    std::vector<Vertex_handle> aliased_sites;
    collect_aliased_sites(aliased_sites, max_to_remove, gamma);

    // debug
    std::cerr<<"-----------breaking regularity, gamma = "<<gamma<<", remove "<<aliased_sites.size()<<".\n";
    
    for (unsigned i = 0; i < aliased_sites.size(); ++i)
    {
        Vertex_handle vi = aliased_sites[i];
        verts_to_jitter.push_back(vi);			// jitter the center of the 1-ring as well.

        std::vector<Vertex_handle> one_ring_verts;
        one_ring_verts.push_back(vi);		// push back the center of the one-ring

        std::vector<double> rand_weights;	// for using to calculate a new point inside the 1-ring
        double sum_weights = 0.;

        //debug
        int counter = 0;

        Vertex_circulator vcirc = m_rt.incident_vertices(vi);
        Vertex_circulator vend  = vcirc;
        CGAL_For_all(vcirc, vend)
        {
            Vertex_handle vj = vcirc;
            verts_to_jitter.push_back(vj);
            one_ring_verts.push_back(vj);
            double rand = random_double(0., 1.);
            rand_weights.push_back(rand);
            sum_weights += rand;

            counter++;
            if (counter > 40) {std::cerr<<"breaking!!!"<<std::endl; return;}
        }
        
        if (counter == 0)
        {
            // debug: never come here
            if (vi == NULL) { std::cerr<<"vi = null \n"; break; }
            std::cerr<<"vi position = "<<vi->get_position()<<std::endl;
            std::cerr<<"vi index = "<<vi->get_index()<<std::endl;
            std::cerr<<"vi weight = "<<vi->get_weight()<<std::endl;
            return;
        }
        
        double test = random_double(0.,1.);
        // if ((i % 2 ) == 0) // alternating every-other looked better a lot of the time...
        if ( test < 0.5)
        {
            int random_index = random_int(0, one_ring_verts.size() - 1);
            int to_delete = one_ring_verts[random_index]->get_index();

            if (indices_for_deletion.find(to_delete) == indices_for_deletion.end())
                indices_for_deletion.insert(to_delete);
        }
        else // add a new point inside the 1-ring.
        {  // THIS IS WHAT I AM THE MOST SUSPICIOUS OF!!!!!!!! Do I need the CGAL::origin term?
            // remember the symptom is that they become very close to a single point. I believe they
            // get inserted closest to the center - that makes sense given the interpolation I'm doing, right?

            Point new_pos(0., 0.);
            for(unsigned j = 0; j < rand_weights.size(); ++j)
            {
                double u = rand_weights[j];
                if (sum_weights != 0.0) u /= sum_weights;
                new_pos = new_pos + u*(one_ring_verts[j + 1]->get_position() - CGAL::ORIGIN); // index from j+1 to exclude the center vertex
            }																  // this is okay because rand_weights has size = # neighbors

            // make the weight zero.
            // FERNANDO - is insert_vertex the right thing to call here? It seems strange.
            Vertex_handle new_vtx = insert_vertex(m_domain.clamp(new_pos), 0., 0., -1, -1);
            new_vertices.push_back(new_vtx);
        }
    }

    jitter_verts(verts_to_jitter);
    
    // delete points flagged for removal.
    for (std::set<int>::const_reverse_iterator
         rit = indices_for_deletion.rbegin(); rit != indices_for_deletion.rend(); ++rit)
    {
        int index = *rit;
        m_vertices.erase(m_vertices.begin() + index);
    }
    
    // add new points.
    m_vertices.insert(m_vertices.end(), new_vertices.begin(), new_vertices.end());
    
    std::cerr<<"-----------"<<indices_for_deletion.size()<<" points removed; "<<new_vertices.size()<<" points added.\n";
    reset_weights(); // this also calls update_triangulation();
}

void Scene::jitter_verts(std::vector<Vertex_handle> & verts_to_jitter)
{
    double alpha = 2.0; // used for the variance in jittering
    MTGenerator mtg(static_cast<unsigned>(rand()));
    
    std::vector<Point> new_pos;
    std::vector<Vertex_handle> jittered;
    cleanup_vec(verts_to_jitter); // remove repeated & out of bounds verts

    // add noise
    for (unsigned k = 0; k < verts_to_jitter.size(); ++k)
    {
        Vertex_handle vk = verts_to_jitter[k];
        if (vk->is_hidden()) continue;
        
        // debug
        if (std::find(m_vertices.begin(), m_vertices.end(), vk) == m_vertices.end())
        {
            std::cerr<<" vertex not found!! " << vk->get_position() << "\n";
            continue;
        }
        //
        
        jittered.push_back(vk);
        
        // this area could probably be precomputed; it is okay if the distribution is the same for every point.
        // in the paper they used alpha/(# points) for all points, but this wouldn't work for a non-uniform distribution.
        // We figured that since they were using domain with area = 1, this was approx the same as dual area per point.
        // TODO: also try with the area of the 1-ring?

        FT area = m_rt.get_dual_area(vk);
        FT deviation = alpha * std::sqrt(area); // deviation = sqrt(variance), variance = alpha^2 * area
        
        // debug
        if (area <= 0.0)
        {
            std::cerr<<"----------ALERT! zero area!"<<std::endl;
            std::cerr<<"point position = "<<vk->get_position()<<", weight = "<<vk->get_weight()<<std::endl;
            std::cerr<<"area = "<<area<<std::endl;
            area = 0.0;
        }
        if (!(deviation == deviation)) // check for NAN
            std::cerr<<"deviation = "<< deviation <<std::endl;
        //
        
        NormalDist gaussian(0.0, deviation);
        GaussianGenerator gaussian_generator(mtg, gaussian);
        
        Point pi = vk->get_position();
        Point new_point(pi.x() + gaussian_generator(), pi.y() + gaussian_generator());
        new_pos.push_back(m_domain.clamp(new_point));
    }

    // move them all at once, so we don't disturb connectivity.
    for (unsigned k = 0; k < jittered.size(); ++k)
        jittered[k]->set_position(new_pos[k]);
}

void Scene::collect_aliased_sites(std::vector<Vertex_handle>& aliased_sites, int n, double gamma)
{
    aliased_sites.clear();
    std::vector<Vertex_handle> all_aliased;
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        if(m_vertices[i]->is_hidden()) continue;
        
        if(is_aliased(m_vertices[i], gamma))
            all_aliased.push_back(m_vertices[i]);
    }
    
    std::cerr<<all_aliased.size()<<" aliased sites found. ";
    
    if(all_aliased.size() <= n)
        aliased_sites = all_aliased;
    
    else {
        // otherwise, need to randomly select n of them.
        for(unsigned j = 0; j < n; ++j)
        {
            int temp = random_int(0, (int) all_aliased.size());
            aliased_sites.push_back(all_aliased[temp]);
            all_aliased.erase(all_aliased.begin() + temp); // don't pick the same one twice!
        }
    }
    
    std::cerr<<aliased_sites.size()<<" sites selected at random to be removed.\n";
}
*/

/* gamma_min is the smallest gamma for which aliased sites will be found 
 * on a given dataset. Typical values for results of our method, converged to epsilon E-5:
 * 100 pts : E-8 or E-9
 * 500	   : E -10 - E11 (variable)
 * 1024	   : E -10
 * 5000	   : E -12 or -13
 */

/*
FT Scene::compute_gamma_min() {

    FT gamma = 1.0;
    std::vector<Vertex_handle> aliased_sites;
    collect_aliased_sites(aliased_sites, num_points(), gamma);

    if(aliased_sites.empty()) {
        std::cerr<<"ERROR: gamma > 1. Returning gamma = 1. \n";
        return gamma;
    }

    collect_aliased_sites( aliased_sites, num_points(), gamma / 10. );

    while(! aliased_sites.empty()){
        gamma = gamma / 10.;
        collect_aliased_sites( aliased_sites, num_points(), gamma / 10. );
    }

    collect_aliased_sites( aliased_sites, num_points(), gamma);
    std::cerr<<"min gamma = "<<gamma<<"; at this level there are "<<aliased_sites.size()<<" aliased sites.\n";

    return gamma;
}
*/

/* DEPRECATED
// now we use var (dist to neighboring verts)
 FT Scene::compute_min_dist_to_segment(Vertex_handle vertex) const
{
 std::vector<Vertex_handle> adj_verts;
 Vertex_circulator vcirc = m_rt.incident_vertices(vertex);
 Vertex_circulator vend  = vcirc;
 CGAL_For_all(vcirc, vend)
 {
  Vertex_handle vi = vcirc;
  adj_verts.push_back(vi);
 }

 // there are k(k-1)/2 edges to check.
 FT min_dist = 1000000.;
 for(unsigned int i = 0; i < adj_verts.size(); ++i)
 {
  for(unsigned int j = i+1; j < adj_verts.size(); ++j)
  {
   Line line(adj_verts[i]->get_position(), adj_verts[j]->get_position());
   FT dist = CGAL::squared_distance(line, vertex->get_position());
   min_dist = std::min(dist, min_dist);
  }
 }
 return min_dist;
}


void Scene::jitter_vertex(Vertex_handle &vi, double dx, double dy)
{
 Point pi = vi->get_position();
 Point new_pos(pi.x() + dx, pi.y() + dy);
 vi->set_position(m_domain.clamp(new_pos));
}

void Scene::jitter_one_ring(Vertex_handle vi, double deviation)
{
 std::cout<<"jittering the one ring.\n";

 MTGenerator mtg(static_cast<unsigned> (rand()));
 NormalDist gaussian(0.0, deviation);
 GaussianGenerator gaussian_generator(mtg, gaussian);

 double rx = gaussian_generator();
 double ry = gaussian_generator();

 Point pi = vi->get_position();

 Point new_pos(pi.x() + rx, pi.y() + ry);
 vi->set_position(m_domain.clamp(new_pos));

 //std::cout<<"moving "<<pi<< " by "<<rx<<","<<ry<<" to "<<vi->get_position()<<std::endl;
 Vertex_circulator vcirc = m_rt.incident_vertices(vi);
 Vertex_circulator vend  = vcirc;

 CGAL_For_all(vcirc, vend)
 {
  Vertex_handle vj = vcirc;
  Point pj = vj->get_position();
  rx = gaussian_generator();
  ry = gaussian_generator();
  Point new_pos(pj.x() + rx, pj.y() + ry);
  vj->set_position(m_domain.clamp(new_pos));

  //std::cout<<"moving "<<pj<< " by "<<rx<<","<<ry<< " to "<<vj->get_position()<<std::endl;
 }
}
*/

/*
void Scene::compute_regularity(FT &min_score, FT &max_score)
{
    max_score = 0.0;
    min_score = 10000000.;
    m_regularity.clear();
    
    for (unsigned i = 0; i < m_vertices.size(); ++i)
    {
        Vertex_handle vi = m_vertices[i];
        if (vi->is_hidden()) continue;
        
        FT score = compute_regularity_score(vi);
        m_regularity.push_back(score);
        
        max_score = std::max(score, max_score);
        min_score = std::min(score, min_score);
    }
}
*/