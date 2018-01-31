#include <iostream>
#include <cmath>
#include "../../samplers/SamplerDigitalSequence.hpp"

#define D 2

typedef std::vector< std::pair<double, double> > PCF;

double period = 4.0;
double step = 0.005;
double sigma = 0.2;
std::vector<double> toricEuclideanDistances(const utk::Point<D, double> &a,
                                            const utk::Point<D, double> &b)
{
  std::vector<double> d(9, 0);
  
  int di = 0;
  for(int ix=-1; ix<2; ix++)
  for(int iy=-1; iy<2; iy++)
  {
    double x = ((a.pos()[0]) - (b.pos()[0]+ix));
    double y = ((a.pos()[1]) - (b.pos()[1]+iy));
    double dist = (x*x + y*y);
    d[di] = sqrt(dist);
    di++;
  }
  
  return d;
}
//Gaussian Kernel 1D
double gaussianKernel(const double sigma, const double x)
{
  return ( 1.0/(sqrt(M_PI)*sigma) ) * exp( -(x*x)/(sigma*sigma) );
}
PCF computePCF(const utk::Pointset<D, double, utk::Point<D, double> > &pts,
               const double mmd)
{
  double rmax = period*mmd;
  double step_mmd = rmax*step;
  double n = pts.size();
  //compute PCF
  PCF PCF_pointset;
  for(double r=10*step_mmd; r<rmax+step_mmd; r+=step_mmd)
  {
	//std::cout << r << " " << r+step_mmd << " " << rmax << std::endl;
    PCF_pointset.push_back(std::pair<double, double>(r/mmd, 0.0 ));
  }
  
  size_t pcf_size = PCF_pointset.size();
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<n; j++)
    {
      if(i==j)
      continue;
      std::vector<double> d = toricEuclideanDistances(pts[i], pts[j]);
      for(int vd=0; vd<d.size(); vd++)
      {
        double norm_dist = d[vd]/mmd;
        for(int p=0; p<pcf_size; p++)
        {
          PCF_pointset[p].second += gaussianKernel(sigma, PCF_pointset[p].first-norm_dist);
        }
      }
    }
  }
  
  double cdf = 0;
  //normalize & weight
  for(int i=0; i<pcf_size; i++)
  {
    double norm = 1.0/(2.0*M_PI*(PCF_pointset[i].first)*n*n);
    PCF_pointset[i].second *= norm;
    cdf += PCF_pointset[i].second;
  }
  for(int i=0; i<pcf_size; i++)
  PCF_pointset[i].second /= cdf;
  
  
  return PCF_pointset;
}
double lmax(const PCF &pa, const PCF &pb)
{
  //std::cout << pa.size() << " " << pb.size() << std::endl;
  //std::cout << pa[189].first << " " << pb[189].first << std::endl;
  assert( pa.size() == pb.size() );

  double res=0.0;
  for(unsigned int i = 0; i < pa.size(); ++i)
    if (std::abs(pa[i].second - pb[i].second) > res )
      res = std::abs(pa[i].second - pb[i].second);
  
  return res;
}
PCF readPCF(const char *filename)
{
  PCF p;
  double d,density;
  std::ifstream filePS;
  filePS.open(filename, std::ifstream::in);
  unsigned int cpt=0;
  while (filePS.good())
  {
    filePS>>d;
    filePS>>density;
    if (filePS.good())
    {
      p.push_back(std::make_pair(d,density));
      cpt++;
    }
  }
  filePS.close();
  std::cout << "Read "<<cpt<< " bins"<<std::endl;
  return p;
}

int nb_patterns_written = 0;
void writeMatrices(Matrix<double, Dynamic, Dynamic> matx, Matrix<double, Dynamic, Dynamic> maty, const char *filename)
{
  std::ofstream filePCF;
  filePCF.open(filename, std::ios::out | std::ios::app);

  filePCF << "\n{";
  
  for(int i=0; i<matx.rows(); i++)
  {
	filePCF << "{ ";
	for(int j=0; j<matx.cols(); j++)
	{
		filePCF << matx(i,j); 
		if(j < matx.cols()-1)
			filePCF << ",";
	}
	filePCF << " }";
	filePCF << ",";
  }
  filePCF << std::endl;
  for(int i=0; i<maty.rows(); i++)
  {
	filePCF << "{ ";
	for(int j=0; j<maty.cols(); j++)
	{
		filePCF << maty(i,j); 
		if(j < maty.cols()-1)
			filePCF << ",";
	}
	filePCF << " }";
	if(i < matx.rows()-1)
			filePCF << ",";
  }
  
  filePCF << " },\n";
  
  filePCF.close();
}
void writePointSet(utk::Pointset<D, double, utk::Point<D, double> > pts, const char *filename)
{
  std::ofstream filePCF;
  filePCF.open(filename, std::ios::out);

  for(int i=0; i<pts.size(); i++)
	filePCF << pts[i].pos() << std::endl;
  
  filePCF.close();
}

int main(int argc, char **argv)
{
  srand(time(NULL));
  
  uint base = 3;
  uint nbpts = pow(base, 3);
  
  utk::SamplerDigitalSequence sampler;
  PCF inputPCF = readPCF(argv[1]);
  
  double best = 0;
  while(1)
  {
	sampler.resetMatrices();
	sampler.setBasis(base);
  
	utk::Pointset< D, double, utk::Point<D, double> > pts;
	sampler.generateSamples<D, double, utk::Point<D, double> >(pts, nbpts);
	
	double minDist = -1;
		for(int i=0; i<pts.size(); i++)
		for(int j=0; j<pts.size(); j++)
		{
			if(i==j) continue;
			
			double dist = 0;
			for(int d=0; d<D; d++)
			{
				double diffx = std::abs(pts[j].pos()[d] - pts[i].pos()[d]);
				diffx = std::min( diffx, 1-diffx );
				dist += diffx*diffx;
			}
			dist = sqrt(dist);
			if(dist < minDist || minDist < 0)
				minDist = dist;
		}
		if(minDist>best)
			best = minDist;
		
		double gamman_max = (1.0/6.0) * M_PI * sqrt(3); 
		double distDT = pow( (gamman_max/(double)nbpts) * tgamma(D/2 + 1)/(pow(M_PI, D/2)), 1.0/(double)D );
		
		double norm = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*pts.size()) );
		PCF current = computePCF(pts, norm);
		double lmaxpcf=lmax(current, inputPCF);
		
		std::cout << "N=" << nbpts << " radius=" << distDT << " MD=" << minDist << " LMAX=" << lmaxpcf << "\r";
		
		if(minDist > distDT && lmaxpcf < 0.002)
		{
			char name[1000];
			sprintf(name, "output_%d.dat", nb_patterns_written);
			writePointSet(pts, name);
			sprintf(name, "matrices_%d.pts", nb_patterns_written);
			writeMatrices(sampler.getMatrice(0), sampler.getMatrice(1), "matriceList.dat");
			
			std::cout << "Found N=" << nbpts << " radius=" << distDT << " MD=" << minDist << " LMAX=" << lmaxpcf << "\n";
			
			nb_patterns_written++;
			if(nb_patterns_written >= 10)
				return 0;
		}
  }
  /*int size_mat = SIZEPATTERN;
  
  int nbbitlibres = SIZEPATTERN*SIZEPATTERN;

  long long int nb_combinaisons = pow(BASIS, nbbitlibres);
  
  std::cout << "Pattern Size =" << SIZEPATTERN << std::endl;
  std::cout << "Nb Bit Libres =" << nbbitlibres << std::endl;
  
  double threshold = 0.00366;

  Matrix<double, SIZEPATTERN, SIZEPATTERN> matx;
  Matrix<double, SIZEPATTERN, SIZEPATTERN> maty;  
  
  for(int x=0; x<size_mat; x++)
    for(int y=0; y<size_mat; y++)
    {
      matx(x, y) = 0;
      maty(x, y) = 0;
    }
  
  while(1)
  {
    int n=0;
    for(int x=0; x<SIZEPATTERN; x++)
    for(int y=0; y<SIZEPATTERN; y++)
			matx(x, y) = (x==y);//rand()%BASIS;
	
    //if(matx.determinant() == 0) continue;
    
	for(int x=0; x<SIZEPATTERN; x++)
	for(int y=0; y<SIZEPATTERN; y++)
		maty(x, y) = (x == SIZEPATTERN-y-1);//rand()%BASIS;
	
	//if(maty.determinant() == 0) continue;
		
	//generate pset;
	PointSet pset;
	PointSet pset_01;
	for(int a=0; a<pow(BASIS, SIZEPATTERN); a++)
	{
		Matrix<double, SIZEPATTERN, 1> index;
		for(int i=0; i<SIZEPATTERN; i++)
			index(i,0) = 0;
		int ai = a;
		double dev = 1.0/BASIS;
		int id=0;
		while(ai != 0)
		{
			int d = ai % BASIS;
			index(id, 0) = d;// * dev;
			ai = (ai-d)/BASIS;
			dev = dev / BASIS;
			id++;
		}

		double index_val = 0;
		for(int pi=0; pi<SIZEPATTERN; pi++)
		index_val += index(pi, 0)*pow(BASIS, pi);

		Matrix<double, SIZEPATTERN, 1> px = matx*index;
		Matrix<double, SIZEPATTERN, 1> py = maty*index;
		
		double ppx = 0;
		for(int pi=0; pi<SIZEPATTERN; pi++)
		ppx += ((int)px(pi, 0)%BASIS)*pow(BASIS, -(pi+1));
		double ppy = 0;
		for(int pi=0; pi<SIZEPATTERN; pi++)
		ppy += ((int)py(pi, 0)%BASIS)*pow(BASIS, -(pi+1));

		pset_01.push_back(Point2D(ppx, ppy));
		
		ppx *= pow(BASIS, SIZEPATTERN);
		ppy *= pow(BASIS, SIZEPATTERN);
		
		pset.push_back(Point2D(ppx, ppy));
	}
	
	//compute maximal min dist
	double minDist = -1;
	for(int i=0; i<pset_01.size(); i++)
	for(int j=0; j<pset_01.size(); j++)
	{
		if(i==j) continue;
		
		double dist = 0;
		double diffx = std::abs(pset_01[j].x - pset_01[i].x);
		diffx = std::min( diffx, 1-diffx );
		double diffy = std::abs(pset_01[j].y - pset_01[i].y);
		diffy = std::min( diffy, 1-diffy );
		dist = sqrt(diffx*diffx+diffy*diffy);

		if(dist < minDist || minDist < 0)
			minDist = dist;
	}
	
	double gamman_max = (1.0/6.0) * M_PI * sqrt(3); 
	double distDT = 1.4 * pow( (gamman_max/(double)pow(BASIS, SIZEPATTERN)) * tgamma(2/2 + 1)/(pow(M_PI, 2/2)), 1.0/(double)2 );
	std::cout << "N=" << pow(BASIS, SIZEPATTERN) << " radius=" << distDT << " MD=" << minDist << "\r";
	//if(minDist > distDT)
	{
		char name[1000];
		sprintf(name, "output_%d.dat", nb_patterns_written);
		writePointSet(pset_01, name);
		sprintf(name, "matrices_%d.pts", nb_patterns_written);
		writeMatrices(matx, maty, name);
		
		std::cout << "Found N=" << pow(BASIS, SIZEPATTERN) << " radius=" << distDT << " MD=" << minDist << "\n";
		
		nb_patterns_written++;
		if(nb_patterns_written >= 1)
			return 0;
	}
  }
  
  return 0;*/
}