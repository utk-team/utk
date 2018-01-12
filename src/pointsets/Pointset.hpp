#ifndef _UTK_POINTSET_HPP_
#define _UTK_POINTSET_HPP_

#include "Point.hpp"
#include "Domain.hpp"
#include "../io/messageStream.hpp"

#include <vector>
#include <typeinfo>
#include <bitset>
#include <array>

namespace utk
{

template <unsigned int D, typename T, typename P>
class Pointset : public std::vector< P >
{
public:
	Pointset() 
	{
		for(uint d=0; d<D; d++)
		{
			domain.pMin.pos()[d] = 0;
			domain.pMax.pos()[d] = 1;
		}
		toricity=-1;
		
		/*m_samples = (P*)malloc(sizeof(P));
		m_size = 0;
		m_capacity = 1;*/
	}
	/*~Pointset()
	{
		//free(m_samples);
	}*/
	
	/*void push_back(P& arg_point)
	{
		m_size++;
		if(m_size > m_capacity)
		{
			m_capacity += 1000;
			m_samples = (P*)realloc(m_samples, sizeof(P)*m_capacity);
		}
		m_samples[m_size-1] = arg_point;
	}
	void push_back(const P& arg_point)
	{
		m_size++;
		if(m_size > m_capacity)
		{
			m_capacity += 1000;
			m_samples = (P*)realloc(m_samples, sizeof(P)*m_capacity);
		}
		m_samples[m_size-1] = arg_point;
	}
	uint size() const
	{
		return m_size;
	}
	void resize(uint new_size)
	{
		if(new_size > m_capacity);
			m_samples = (P*)realloc(m_samples, sizeof(P)*new_size);
		m_size = new_size;
	}
	bool empty() const { return m_size == 0; }
	void clear()
	{
		m_size = 0;
	}
	P& operator[](const uint i)
	{
		assert(i < m_size);
		return m_samples[i];
	}
	P operator[](const uint i) const
	{
		assert(i < m_size);
		return m_samples[i];
	}
	P& at(const uint i)
	{
		assert(i < m_size);
		return m_samples[i];
	}
	P at(const uint i) const
	{
		assert(i < m_size);
		return m_samples[i];
	}
	const P* data() const
	{
		return m_samples;
	}*/
	
	double normalizePositions( Pointset<D, double, Point<D, double> >& pts2 ) const
	{
		if(domain.pMin.pos() > domain.pMax.pos())
		{
			ERROR("Poinset::normalizePositions Domain min point is superior to domain max point");
			return false;
		}

		if(domain.pMin == domain.pMax)
		{
			WARNING("Pointset::normalizePositions no domain is given, cannot normalize, return the original pointset");
			for(uint d=0; d<D; d++)
				pts2.domain.pMax.pos()[d] = 1;
			
			pts2.resize(this->size());
			
			for(uint i=0; i<this->size(); i++)
				for(uint d=0; d<D; d++)
					pts2[i].pos()[d] = this->at(i).pos()[d];
		}
		else
		{	
			for(uint d=0; d<D; d++)
				pts2.domain.pMax.pos()[d] = 1;
			
			pts2.resize(this->size());
			
			for(uint i=0; i<this->size(); i++)
				for(uint d=0; d<D; d++)
					pts2[i].pos()[d] = ((double)this->at(i).pos()[d] - (double)this->domain.pMin.pos()[d]) / ((double)this->domain.pMax.pos()[d] - (double)this->domain.pMin.pos()[d]);
		}
		return true;
	}
	
	Pointset<2, double, Point<2, double> > Projection2D(Vector<D, double> plane) const
	{
		Pointset< 2, double, Point<2, double> > result;
		result.resize(this->size());
		
		uint d=0;
		for(uint dp=0; dp<D; dp++)
		{
			if(plane[dp] == 0)
				continue;
			else
			{
				result.domain.pMin.pos()[d] = this->domain.pMin.pos()[dp];
				result.domain.pMax.pos()[d] = this->domain.pMax.pos()[dp];
				d++;
			}
		}
		
		for(uint i=0; i<this->size(); i++)
		{
			d=0;
			for(uint dp=0; dp<D; dp++)
			{
				if(plane[dp] == 0)
					continue;
				else
				{
					result.domain.pMin.pos()[d] = this->domain.pMin.pos()[dp];
					result.domain.pMax.pos()[d] = this->domain.pMax.pos()[dp];
					result[i].pos()[d] = this->at(i).pos()[dp];
					d++;
				}
			}
		}
		return result;
	}
	
	Pointset<3, double, Point<3, double> > Projection3D(Vector<D, double> plane) const
	{
		Pointset< 3, double, Point<3, double> > result;
		result.resize(this->size());
		
		uint d=0;
		for(uint dp=0; dp<D; dp++)
		{
			if(plane[dp] == 0)
				continue;
			else
			{
				result.domain.pMin.pos()[d] = this->domain.pMin.pos()[dp];
				result.domain.pMax.pos()[d] = this->domain.pMax.pos()[dp];
				d++;
			}
		}
		
		for(uint i=0; i<this->size(); i++)
		{
			d=0;
			for(uint dp=0; dp<D; dp++)
			{
				if(plane[dp] == 0)
					continue;
				else
				{
					result.domain.pMin.pos()[d] = this->domain.pMin.pos()[dp];
					result.domain.pMax.pos()[d] = this->domain.pMax.pos()[dp];
					result[i].pos()[d] = this->at(i).pos()[dp];
					d++;
				}
			}
		}
		return result;
	}

	//template<unsigned int D, typename T2>
	bool cranley_patterson(const Vector<D, T>& arg_vect)
	{
	  if(!checkpointset_cranley_patterson(arg_vect))
		  return false;
	  
	  for(uint i=0; i<this->size(); i++)
	  {
	    this->at(i).pos() += arg_vect;
	    for(uint d=0; d<D; d++)
	      if (this->at(i).pos()[d] >= domain.pMax.pos()[d])
				this->at(i).pos()[d] -= (domain.pMax.pos()[d]-domain.pMin.pos()[d]);
	  }
	  
	  return true;
	}
	//template<unsigned int D, typename T2>
	bool cranley_patterson(const Vector<D, T>& arg_vect, Pointset<D, T, P>& arg_pts) const
	{
	  if(!checkpointset_cranley_patterson(arg_vect))
		  return false;
	  
	  for(int i=0; i<this->size(); i++)
	  {
	    arg_pts.push_back(this->at(i));
	    arg_pts.at(i).pos() += arg_vect;
	    for(int d=0; d<D; d++)
	      if (arg_pts.at(i).pos()[d] >= domain.pMax.pos()[d])
		arg_pts.at(i).pos()[d] -= (domain.pMax.pos()[d]-domain.pMin.pos()[d]);
	  }
	  arg_pts.domain.pMin = domain.pMin;
	  arg_pts.domain.pMax = domain.pMax;
	  arg_pts.toricity = toricity;
	  return true;
	}
	
	/*bool hybrid_xor_shift(const Vector<D, T>& arg_vect)
	{
		if(!checkpointset_hybrid_xor_shift(arg_vect))
			return false;
		
		int sqr_n = sqrt(this->size());
		int xor_shift[D] = {0};
		int tor_shift[D] = {0};
		
		for(int d=0; d<D; d++)
			tor_shift[d] = arg_vect[d]/sqr_n;

		for(int d=0; d<D; d++)
			xor_shift[d] = arg_vect[d]%sqr_n;

		for(int d=0; d<D; d++)
		{
			if(xor_shift[d] > sqr_n)
			{
				tor_shift[d]++;
				xor_shift[d]=this->size() - sqr_n + xor_shift[d];
			}
		}
		
		for(int i=0; i<this->size(); i++)
		{
			for(int d=0; d<D; d++)
				this->at(i).pos()[d] = (this->at(i).pos()[d] + tor_shift[d]*sqr_n)%this->size();
		}
		
		for(int i=0; i<this->size(); i++)
		{
			for(int d=0; d<D; d++)
				this->at(i).pos()[d] = this->at(i).pos()[d] ^ xor_shift[d];
		}
	}
	
	bool hybrid_xor_shift(const Vector<D, T>& arg_vect, Pointset<D, T, P>& arg_pts) const
	{
		if(!checkpointset_hybrid_xor_shift(arg_vect))
			return false;
		
		arg_pts.resize(this->size());
		
		int sqr_n = sqrt(this->size());
		int xor_shift[D] = {0};
		int tor_shift[D] = {0};
		
		for(int d=0; d<D; d++)
			tor_shift[d] = arg_vect[d]/sqr_n;

		for(int d=0; d<D; d++)
			xor_shift[d] = arg_vect[d]%sqr_n;

		for(int d=0; d<D; d++)
		{
			if(xor_shift[d] > sqr_n)
			{
				tor_shift[d]++;
				xor_shift[d]=this->size() - sqr_n + xor_shift[d];
			}
		}
		
		for(int i=0; i<this->size(); i++)
		{
			for(int d=0; d<D; d++)
				arg_pts[i].pos()[d] = (this->at(i).pos()[d] + tor_shift[d]*sqr_n)%this->size();
		}
		
		for(int i=0; i<this->size(); i++)
		{
			for(int d=0; d<D; d++)
				arg_pts[i].pos()[d] = arg_pts[i].pos()[d] ^ xor_shift[d];
		}
	}*/
	

	bool digit_xor(const Vector<D, T>& arg_vect)
	{	
	  if(!checkpointset_digit_xor(arg_vect))
		return false;

	  for(uint i=0; i<this->size(); i++)
	  {
	    for(uint d=0; d<D; d++)
		{
	      unsigned int tmp = this->at(i).pos()[d];
	      tmp ^= (unsigned int)(arg_vect[d]);
		  this->at(i).pos()[d] = tmp;
		}
		
	  }
	  return true;
	}

	bool digit_xor(const Vector<D, T>& arg_vect, Pointset<D, T, P>& arg_pts) const
	{		
	  if(!checkpointset_digit_xor(arg_vect))
		return false;
	  
	  for(int i=0; i<this->size(); i++)
	  {
	    arg_pts.push_back(this->at(i));
	    for(int d=0; d<D; d++)
		{
		  unsigned int tmp = arg_pts.at(i).pos()[d];
	      tmp ^= (unsigned int)(arg_vect[d]);
		  arg_pts.at(i).pos()[d] = tmp;
		}
	  }
	  arg_pts.domain.pMin = domain.pMin;
	  arg_pts.domain.pMax = domain.pMax;
	  arg_pts.toricity = toricity;
	  return true;
	}
	
	/*bool owen_scrambling(const Vector<D, bool*>& arg_vect, Pointset<D, T, P>& arg_pts, int nbBits=-1) const
	{
	  if(!checkpointset_owen_scrambling(arg_vect))
		return false;
	  
	  int ind;
	  if(nbBits==-1)
	    nbBits = ceil(log2(this->size()));
	  //uint nbFlags = pow(2, nbBits)-1;
	  
	  arg_pts.resize(this->size());
	  
	  bool* pt_digits = (bool*)malloc(sizeof(bool)*nbBits);
	  bool* res_digits = (bool*)malloc(sizeof(bool)*nbBits);
	  
	  for(uint d=0; d<D; d++)
	  {
		bool* permut = arg_vect[d];
		
		for(uint ipt=0; ipt<this->size(); ipt++) 
		{
			T pt = this->at(ipt).pos()[d];
			//std::cout << "pt " << pt << std::endl;
			
			for(int i=0; i<nbBits; i++)
				pt_digits[i] = (pt >> i) & 1;

			for(int ilevel=0; ilevel<nbBits; ilevel++) {
				int begin_level = pow(2, ilevel)-1;
				ind = (pt >> (nbBits-ilevel)) + begin_level;// + (pow(2,ilevel)-1);
				//std::cout << "Digit " << nbBits-ilevel-1 << " = " << permut[ind] << '(' << begin_level <<  "+" << (pt >> (nbBits-ilevel)) << ") ^" << pt_digits[nbBits-ilevel-1] << std::endl;
				res_digits[ilevel] = permut[ind] ^ pt_digits[nbBits-ilevel-1];
			}
			
			T tmp = 0;
			for(int i=0; i<nbBits; i++) {
				tmp <<= 1;
				tmp |= res_digits[i];
			}
			
			arg_pts[ipt].pos()[d] = tmp;
		}
	  }
	  
	  arg_pts.domain.pMin = domain.pMin;
	  arg_pts.domain.pMax = domain.pMax;
	  arg_pts.toricity = toricity;
	  
	  free(pt_digits);
	  free(res_digits);
    
      return true;
	}*/
	
	/*bool owen_scrambling(const Vector<D, bool*>& arg_vect, int nbBits=-1)
	{
	  if(!checkpointset_owen_scrambling(arg_vect))
		return false;
	  
	  int ind;
	  if(nbBits==-1)
		  nbBits = ceil(log2(this->size()));
	  //uint nbFlags = pow(2, nbBits)-1;
	  
	  bool* pt_digits = (bool*)malloc(sizeof(bool)*nbBits);
	  bool* res_digits = (bool*)malloc(sizeof(bool)*nbBits);
	  
	  for(uint d=0; d<D; d++)
	  {
		bool* permut = arg_vect[d];
		
		for(uint ipt=0; ipt<this->size(); ipt++) 
		{
			T pt = this->at(ipt).pos()[d];
			//std::cout << "pt " << pt << std::endl;
			
			for(int i=0; i<nbBits; i++)
				pt_digits[i] = (pt >> i) & 1;

			for(int ilevel=0; ilevel<nbBits; ilevel++) {
				int begin_level = pow(2, ilevel)-1;
				ind = (pt >> (nbBits-ilevel)) + begin_level;// + (pow(2,ilevel)-1);
				//std::cout << "Digit " << nbBits-ilevel-1 << " = " << permut[ind] << '(' << begin_level <<  "+" << (pt >> (nbBits-ilevel)) << ") ^" << pt_digits[nbBits-ilevel-1] << std::endl;
				res_digits[ilevel] = permut[ind] ^ pt_digits[nbBits-ilevel-1];
			}
			
			T tmp = 0;
			for(int i=0; i<nbBits; i++) {
				tmp <<= 1;
				tmp |= res_digits[i];
			}
			
			this->at(ipt).pos()[d] = tmp;
		}
	  }
	  
	  free(pt_digits);
	  free(res_digits);
			
      return true;
	}*/
	
	
	//TODO Efficient implementation
	/*bool isTK2NetInBaseB(int t, int k, int b)
	{
		if(pow(b, k) != this->size())
		{
			ERROR("Pointset::isTK2NetInBaseB the pointset must have b^k points");
			return false;
		}
		if(D != 2)
		{
			ERROR("Pointset::isTK2NetInBaseB can only compute equidistribution for 2D pointsets");
			return false;
		}
		
		int nbpts = this->size();
	
		for(uint qi=0; qi<=k; qi++)
		for(uint qj=0; qj<=k; qj++)
		{
			uint q = qi+qj;
			if(q > (k-t))
				continue;
			
			
			unsigned long int bqi = pow(b, qi);
			unsigned long int bqj = pow(b, qj);
			unsigned long pts_par_box = pow(b, k-q);
			
			double box_size[2];
			box_size[0] = 1.0/(double)bqi;
			box_size[1] = 1.0/(double)bqj;
			
			for(uint ri=0; ri<bqi; ri++)
			for(uint rj=0; rj<bqj; rj++)
			{
				double box_corner[2];
				box_corner[0] = (double)ri/(double)bqi;
				box_corner[1] = (double)rj/(double)bqj;
				
				int nbpts_in_box = 0;
				for(int pi=0; pi<nbpts; pi++)
				{
					double pt[2];
					pt[0] = (double) this->at(pi).pos()[0] /(double) nbpts;
					pt[1] = (double) this->at(pi).pos()[1] /(double) nbpts;
					
					if(is2DPtInsideBox(pt, box_corner, box_size))
						nbpts_in_box++;
				}
				
				if(nbpts_in_box != pts_par_box)
					return false;
			}
		}
		return true;
	}
	bool isTK3NetInBaseB(int t, int k, int b)
	{
		if(pow(b, k) != this->size())
		{
			ERROR("Pointset::isTK3NetInBaseB the pointset must have b^k points");
			return false;
		}
		if(D != 3)
		{
			ERROR("Pointset::isTK3NetInBaseB can only compute equidistribution for 3D pointsets");
			return false;
		}
		
		int nbpts = this->size();
	
		for(uint qi=0; qi<=k; qi++)
		for(uint qj=0; qj<=k; qj++)
		for(uint qk=0; qk<=k; qk++)
		{
			uint q = qi+qj+qk;
			if(q > (k-t))
				continue;
			
			
			unsigned long int bqi = pow(b, qi);
			unsigned long int bqj = pow(b, qj);
			unsigned long int bqk = pow(b, qk);
			unsigned long pts_par_box = pow(b, k-q);
			
			double box_size[3];
			box_size[0] = 1.0/(double)bqi;
			box_size[1] = 1.0/(double)bqj;
			box_size[2] = 1.0/(double)bqk;
			
			for(uint ri=0; ri<bqi; ri++)
			for(uint rj=0; rj<bqj; rj++)
			for(uint rk=0; rj<bqk; rk++)
			{
				double box_corner[3];
				box_corner[0] = (double)ri/(double)bqi;
				box_corner[1] = (double)rj/(double)bqj;
				box_corner[2] = (double)rk/(double)bqk;
				
				int nbpts_in_box = 0;
				for(int pi=0; pi<nbpts; pi++)
				{
					double pt[3];
					pt[0] = (double) this->at(pi).pos()[0] /(double) nbpts;
					pt[1] = (double) this->at(pi).pos()[1] /(double) nbpts;
					pt[2] = (double) this->at(pi).pos()[2] /(double) nbpts;
					
					if(is3DPtInsideBox(pt, box_corner, box_size))
						nbpts_in_box++;
				}
				
				if(nbpts_in_box != pts_par_box)
					return false;
			}
		}
		return true;
	}*/
	
/*#define PERMUTS_NBITS 15
#define NPTS 16
#define NBITS 4
	//template<unsigned int D, typename T2>
	//TODO virer les templates de bitset pour avoir un truc dynamique en fction de la taille du pointset (ou d'une profondeur d'arbre parametre)
	bool digitScrambling_Owen(const Vector<D, T>& arg_vect, Pointset<D, T, P>& arg_pts) const
	{		
	  if(!checkpointset_digitScrambling_Owen(arg_vect))
		return false;
	
	  int ind;
	  
	  for(int d=0; d<D; d++)
	  {
		std::bitset<PERMUTS_NBITS> permut = arg_vect[d];
		for(int ipt=0; ipt<NPTS; ipt++) {
			std::bitset<NBITS> xresdigits;
			T sobolPt = arg_pts[ipt].pos()[d];
			std::bitset<NBITS> sobolPt_Bits = sobolPt;

			for(int ilevel=0; ilevel<NBITS; ilevel++) {
				ind = (sobolPt >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				xresdigits.set(NBITS-ilevel-1, (permut[PERMUTS_NBITS-ind-1] ^ sobolPt_Bits[NBITS-ilevel-1]) );
			};
			arg_pts[ipt].pos()[d] = xresdigits.to_ulong();
		}
	  }
	}
	//template<unsigned int D, typename T>
	bool digitScrambling_Owen(const Vector<D, T>& arg_vect)
	{		
	  if(!checkpointset_digitScrambling_Owen(arg_vect))
		return false;
	
	  int ind;
	  
	  for(int d=0; d<D; d++)
	  {
		std::bitset<PERMUTS_NBITS> permut = arg_vect[d];
		for(int ipt=0; ipt<NPTS; ipt++) {
			std::bitset<NBITS> xresdigits;
			T sobolPt = this->at(ipt).pos()[d];
			std::bitset<NBITS> sobolPt_Bits = sobolPt;

			for(int ilevel=0; ilevel<NBITS; ilevel++) {
				ind = (sobolPt >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				xresdigits.set(NBITS-ilevel-1, (permut[PERMUTS_NBITS-ind-1] ^ sobolPt_Bits[NBITS-ilevel-1]) );
			};
			this->at(ipt).pos()[d] = xresdigits.to_ulong();
		}
	  }
	}
	
	//template<unsigned int D, typename T2>
	//TODO virer les templates de bitset pour avoir un truc dynamique en fction de la taille du pointset (ou d'une profondeur d'arbre parametre)
	bool digitScrambling_InverseOwen(const Vector<D, T>& arg_vect, Pointset<D, T, P>& arg_pts) const
	{		
	  if(!checkpointset_digitScrambling_Owen(arg_vect))
		return false;
	
	  int ind;
	  
	  for(int d=0; d<D; d++)
	  {
		std::bitset<PERMUTS_NBITS> permut = arg_vect[d];
		for(int ipt=0; ipt<NPTS; ipt++) {
			std::bitset<NBITS> listflags;
			T sobolPt = arg_pts[ipt].pos()[d];

			for(int ilevel=0; ilevel<NBITS; ilevel++) {
				ind = (sobolPt >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				listflags.set(NBITS-ilevel-1, permut[PERMUTS_NBITS-ind-1]);
			};
			T flags = listflags.to_ulong();
			
			
			arg_pts[ipt].pos()[d] = (flags ^ sobolPt);
		}
	  }
	}
	//template<unsigned int D, typename T2>
	bool digitScrambling_InverseOwen(const Vector<D, T>& arg_vect)
	{		
	  if(!checkpointset_digitScrambling_Owen(arg_vect))
		return false;
	
	  int ind;
	  
	  for(int d=0; d<D; d++)
	  {
		std::bitset<PERMUTS_NBITS> permut = arg_vect[d];
		for(int ipt=0; ipt<NPTS; ipt++) {
			std::bitset<NBITS> xresdigits;
			T sobolPt = this->at(ipt).pos()[d];
			std::bitset<NBITS> sobolPt_Bits = sobolPt;

			for(int ilevel=0; ilevel<NBITS; ilevel++) {
				ind = (sobolPt >> (NBITS-ilevel)) + (pow(2,ilevel)-1);
				xresdigits.set(NBITS-ilevel-1, (permut[PERMUTS_NBITS-ind-1] ^ sobolPt_Bits[NBITS-ilevel-1]) );
			};
			this->at(ipt).pos()[d] = xresdigits.to_ulong();
		}
	  }
	}*/
	
	Domain<P> domain;
	int toricity;
	
protected:
	
	/*int m_capacity = 0;
	int m_size = 0;
	P* m_samples;*/
	
	/*bool is2DPtInsideBox(double pt[2], double box_corner[2], double box_size[2])
	{
		if(pt[0] < box_corner[0])
			return false;
		if(pt[0] >= box_corner[0]+box_size[0])
			return false;
		if(pt[1] < box_corner[1])
			return false;
		if(pt[1] >= box_corner[1]+box_size[1])
			return false;
		return true;
	}
	bool is3DPtInsideBox(double pt[3], double box_corner[3], double box_size[3])
	{
		if(pt[0] < box_corner[0])
			return false;
		if(pt[0] >= box_corner[0]+box_size[0])
			return false;
		if(pt[1] < box_corner[1])
			return false;
		if(pt[1] >= box_corner[1]+box_size[1])
			return false;
		if(pt[2] < box_corner[2])
			return false;
		if(pt[2] >= box_corner[2]+box_size[2])
			return false;
		return true;
	}*/
	
	//template<unsigned int D, typename T2>
	bool checkpointset_cranley_patterson(const Vector<D, T>& arg_vect) const
	{
	  if(toricity==-1)
	    WARNING("Pointset::cranley_patterson Cranley Patterson shift is being applied on a pointset of unknown toricity");
	  
	  if(toricity==0)
	  {
	    ERROR("Pointset::cranley_patterson Cranley Patterson shift is being applied on a non toric pointset");
	    return false;
	  }
	  if(domain.pMin == domain.pMax)
	  {
	    ERROR("Poinset::cranley_patterson Domain undefined, cannot apply a toric shift over the domain");
	    return false;
	  }
	  if(domain.pMin.pos() > domain.pMax.pos())
	  {
	    ERROR("Poinset::cranley_patterson Domain min point is superior to domain max point");
	    return false;
	  }
	  return true;
	}
	
	
	//template<unsigned int D, typename T2>
	bool checkpointset_digit_xor(const Vector<D, T>& arg_vect) const
	{
	  if(domain.pMin.pos() > domain.pMax.pos())
	  {
		ERROR("Poinset::digit_xor Domain min point is superior to domain max point");
		return false;
	  }
	
	  if(domain.pMin == domain.pMax)
	    WARNING("Pointset::digit_xor no domain is given, cannot ensure that the permuted points will still be in the domain");
	  else
	  {
		for(uint i=0; i<D;i++)
			if(domain.pMin.pos()[i] != 0){
				ERROR("Pointset::digit_xor The min corner of the domain is not [0]^d");
				return false;
			}
			
		for(uint i=0; i<D;i++)
			if((int)log2(domain.pMax.pos()[i]) != log2(domain.pMax.pos()[i])) {
				ERROR("Pointset::digit_xor domain not a power of 2");
				return false;
			}
			
		for(uint i=0; i<D;i++)
			if(domain.pMax.pos()[i] < arg_vect[i]){
				ERROR("Pointset::digit_xor xoring outside the domain");
				return false;
			}
	  }
		
	  if(typeid(T) != typeid(unsigned int))
	    WARNING("Pointset::digit_xor digit scrambling on a pointset with non uint positions may give undefined results");
	  
	  return true;
	}
	
	//template<unsigned int D, typename T2>
	/*bool checkpointset_hybrid_xor_shift(const Vector<D, T>& arg_vect) const
	{
	  if(domain.pMin.pos() > domain.pMax.pos())
	  {
		ERROR("Poinset::hybrid_xor_shift Domain min point is superior to domain max point");
		return false;
	  }
	
	  if(domain.pMin == domain.pMax)
	    WARNING("Pointset::hybrid_xor_shift no domain is given, cannot ensure that the permuted points will still be in the domain");
	  else
	  {
		for(uint i=0; i<D; i++)
			if(domain.pMin.pos()[i] != 0){
				ERROR("Pointset::hybrid_xor_shift The min corner of the domain is not [0]^d");
				return false;
			}
			
		for(uint i=0; i<D; i++)
			if((int)log2(domain.pMax.pos()[i]) != log2(domain.pMax.pos()[i])) {
				ERROR("Pointset::hybrid_xor_shift domain not a power of 2");
				return false;
			}
			
		for(uint i=0; i<D; i++)
			if(domain.pMax.pos()[i] < arg_vect[i]){
				ERROR("Pointset::hybrid_xor_shift xoring outside the domain");
				return false;
			}
	  }
		
	  if(typeid(T) != typeid(unsigned int))
	    WARNING("Pointset::hybrid_xor_shift digit scrambling on a pointset with non uint positions may give undefined results");
	  
	  return true;
	}*/
	
	
	//template<unsigned int D, typename T2>
	/*bool checkpointset_owen_scrambling(const Vector<D, bool*>& arg_vect) const
	{
	  if(domain.pMin.pos() > domain.pMax.pos())
	  {
		ERROR("Poinset::owen_scrambling Domain min point is superior to domain max point");
		return false;
	  }
		
	  if(typeid(T) != typeid(unsigned int))
	    WARNING("Pointset::owen_scrambling digit scrambling on a pointset with non uint positions may give undefined results");
	  
	  return true;
	}*/
};

} //end namespace

#endif
