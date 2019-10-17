/*
 * Adrien Hamers
 * David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
//
// Created by p1703185 on 11/04/19.
//

#include <assert.h>

#include "PMJ02SampleSequenceGenerator_Pharr.h"

#ifndef NDEBUG
#define D(x) x
#else 
#define D(x)
#endif // NDEBUG

bool isPowerOfFour(unsigned int n) 
{ 
    return n !=0 && ((n&(n-1)) == 0) && !(n & 0xAAAAAAAA); 
} 

PMJ02SampleSequenceGenerator_Pharr::PMJ02SampleSequenceGenerator_Pharr(int seed) {
    randomNumberGenerator = std::mt19937(seed);
}

PMJ02SampleSequenceGenerator_Pharr::~PMJ02SampleSequenceGenerator_Pharr() {
    for(int i = 0; i < numberOfSamplesToGenerate; i++)
        generatedSamples[i].deleteSample();
    delete[](generatedSamples);
    delete[](xhalves);
    delete[](yhalves);
}

void PMJ02SampleSequenceGenerator_Pharr::ProgressiveMultiJittered02Algorithm2D(int numberOfSamplesToGenerate,  
                                                                               int numberOfCandidates) {
    this->numberOfSamplesToGenerate = numberOfSamplesToGenerate;
    int arraySize = 4*numberOfSamplesToGenerate;
    generatedSamples = instantiateArray(arraySize);
    int next_pow_4_samples = numberOfSamplesToGenerate;
    if ( !isPowerOfFour( next_pow_4_samples ) )
    {
        next_pow_4_samples = numberOfSamplesToGenerate >> 1 << 2;
        if ( !isPowerOfFour( next_pow_4_samples ) )
            next_pow_4_samples = next_pow_4_samples << 1;
    }

    int bits = int(log2(next_pow_4_samples << 1 ));

    occupiedStrata.reserve(bits);
    for(int i = 0; i < bits;i++)
    {
       std::vector< bool > temp( next_pow_4_samples, false );
       occupiedStrata.push_back( temp );
    }
    xhalves = new int[ 4 * next_pow_4_samples ];
    yhalves = new int[4*next_pow_4_samples];

    grid_dim = int(ceil(sqrt(next_pow_4_samples)));
    std::vector<SamplePMJ*> grid_temp(next_pow_4_samples, nullptr);
    grid.swap( grid_temp );

    min_dist = next_pow_4_samples;

    generatedSamples[0][0] = generateRandomDouble();
    generatedSamples[0][1] = generateRandomDouble();
    set_grid_item( &(generatedSamples[0]) );

    {
       std::vector< Node > x_temp( pow( 2, bits ), Node() );
       x_tree.swap( x_temp );
       std::vector< Node > y_temp( pow( 2, bits ), Node() );
       y_tree.swap( y_temp );
    }
    
    {
       std::vector< int > x_temp( pow( 2, bits - 1 ), -1 );
       x_offsets.swap( x_temp );
       std::vector< int > y_temp( pow( 2, bits - 1 ), -1);
       y_offsets.swap( y_temp );
    }
    
    numSamples = 1;
    this->numberOfCandidates = numberOfCandidates;
    int currentlyGeneratedSamples = 1;
    while(currentlyGeneratedSamples < numberOfSamplesToGenerate)
    {
        extendSequenceEven(currentlyGeneratedSamples);
        extendSequenceOdd(currentlyGeneratedSamples*2);
        currentlyGeneratedSamples*=4;
    }
}

void PMJ02SampleSequenceGenerator_Pharr::exportSampleSet(std::string &outputPath) {
    std::ofstream outputStream(outputPath);
    outputStream.precision(16);
    if(outputStream) {
        for (int i = 0; i < numberOfSamplesToGenerate; i++) {
            outputStream << generatedSamples[i][0];
            outputStream << " ";
            outputStream << generatedSamples[i][1];
            outputStream << "\n";
        }
        outputStream.close();
    }
    else
        std::cout << "ERREUR d'ouverture de fichier" << std::endl;
}

void PMJ02SampleSequenceGenerator_Pharr::extendSequenceEven(int alreadyGeneratedSamples) {
    D( std::cout << "extendSequenceEven" << std::endl; )
    double n = sqrt((double)alreadyGeneratedSamples);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        unsigned int xhalf = (int)(2.0*(n*oldSample[0] - i));
        unsigned int yhalf = (int)(2.0*(n*oldSample[1] - j));
        xhalf = 1-xhalf;
        yhalf = 1-yhalf;
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }
}

void PMJ02SampleSequenceGenerator_Pharr::extendSequenceOdd(int alreadyGeneratedSamples) {
    D( std::cout << "extendSequenceOdd" << std::endl; )

    double n = sqrt((double)alreadyGeneratedSamples/2.0f);
    markOccupiedStrata(alreadyGeneratedSamples);
    for(int s  = 0; s < alreadyGeneratedSamples/2; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        unsigned int xhalf = (int)(2.0*(n*oldSample[0] - i));
        unsigned int yhalf = (int)(2.0*(n*oldSample[1] - j));
        if(generateRandomDouble() > 0.5)
            xhalf = 1-xhalf;
        else
            yhalf = 1-yhalf;
        xhalves[s] = xhalf;
        yhalves[s] = yhalf;
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }
    for(int s = 0; s < alreadyGeneratedSamples/2; s++)
    {
        SamplePMJ oldSample = generatedSamples[s];
        int i = (int)(n*oldSample[0]);
        int j = (int)(n*oldSample[1]);
        int xhalf = 1-xhalves[s];
        int yhalf = 1-yhalves[s];
        generateSamplePoint(i,j,xhalf,yhalf,n,alreadyGeneratedSamples);
    }

}

void PMJ02SampleSequenceGenerator_Pharr::markOccupiedStrata(int alreadyGeneratedSamples) {
    int NN = 2*alreadyGeneratedSamples;
    for(int i = 0; i <= log2(NN); i++)
        for(int j = 0; j < NN; j++)
            occupiedStrata[i][j] = false;

    for(int s = 0; s < alreadyGeneratedSamples; s++)
    {
        markOccupiedStrata1(generatedSamples[s], NN);
    }
}

void PMJ02SampleSequenceGenerator_Pharr::markOccupiedStrata1(SamplePMJ &pt, int NN) {
    int shape = 0;
    int xdivs = NN;
    int ydivs = 1;
    do{
        int xstratum = (int)(xdivs * pt[0]);
        int ystratum = (int)(ydivs * pt[1]);
        occupiedStrata[shape][ystratum*xdivs+xstratum] = true;
        D( std::cout << "  " << NN << ", " << shape << ", " << xstratum << ", " << ystratum << ", " << pt << std::endl; )
        shape ++;
        xdivs /= 2;
        ydivs *= 2;
    }while(xdivs != 0);
    D( std::cout << std::endl; )
}

void PMJ02SampleSequenceGenerator_Pharr::generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N) {

    int NN = 2*N;
    int lg2NN = int(log2(NN));

    int nx = int(pow(2, ceil(lg2NN/2.)) );
    int nx_2 = nx/n;
    bool is_even = lg2NN % 2 == 0;
    int ny = is_even ? nx : int(pow(2, floor(lg2NN/2.)) );
    int ny_2 = ny/n;

    D( std::cout << " generateSamplePoint - " << i << ", " << j << ", " 
                 << xhalf << ", " << yhalf << ", " << n << ", " << nx << ", "
                 << ny << ", " << NN << ", " << numSamples << std::endl; )

    double bestDist = 0.0;
    SamplePMJ pt = SamplePMJ();
    x_tree.clear();
    y_tree.clear();
    x_offsets.clear();
    y_offsets.clear();

    Node& node_x = x_tree[0];
    x_tree_idx = 0;
    initialize_x_tree( node_x, i*nx_2 + xhalf, j*ny_2 + (is_even ? yhalf : 0), 
                       floor(lg2NN/2.), nx, ny );
    valid_x_stratum( node_x, i*nx_2 + xhalf, j*ny_2 + (is_even ? yhalf : 0), nx, ny );

    Node& node_y = y_tree[0];
    y_tree_idx = 0;
   initialize_y_tree( node_y, i*ny_2 + (is_even ? xhalf : 0), j*nx_2 + yhalf,
                      ceil( lg2NN / 2. ), ny, nx );
   valid_y_stratum( node_y, i*ny_2 + (is_even ? xhalf : 0), j*nx_2 + yhalf, ny, nx );

    D( std::cout << "  X-Tree" << std::endl; )
    D( for (int i = 0; i <= x_tree_idx; ++i) std::cout << "   " << x_tree[i] << std::endl; )
    D( std::cout << "  Y-Tree" << std::endl; )
    D( for (int i = 0; i <= y_tree_idx; ++i) std::cout << "   " << y_tree[i] << std::endl; )

    SamplePMJ candpt = SamplePMJ(2);

    int i_pt = x_offsets[ 0 ];
    int j_pt = y_offsets[ 0 ];

      candpt[ 0 ] = ( i_pt + generateRandomDouble() ) / NN;
      candpt[ 1 ] = ( j_pt + generateRandomDouble() ) / NN;

    int t = 1;
    do
    {
        D( assert( 0 <= candpt[0] && candpt[0] <= 1); )
        D( assert( 0 <= candpt[1] && candpt[1] <= 1); )

        double d;
        bool has_neighbors = minDist(candpt, &d );
        if ( has_neighbors )
        {
            if(d>bestDist)
            {
                bestDist = d;
                pt = candpt;
            }
        }
        else
            pt = candpt;

        if ( ++t < numberOfCandidates )
        {
            candpt[0] = (i_pt+generateRandomDouble())/NN;
            candpt[1] = (j_pt+generateRandomDouble())/NN;
        }
    } while ( t < numberOfCandidates );

    if ( bestDist > 0 && bestDist < min_dist )
        min_dist = bestDist;

    D( assert( !isOccupied(pt, NN));)
    markOccupiedStrata1(pt, NN);
    generatedSamples[numSamples] = pt;
    set_grid_item( &(generatedSamples[numSamples]) );

    numSamples++;
}

bool PMJ02SampleSequenceGenerator_Pharr::isOccupied(SamplePMJ &pt, int NN) {
    int shape = 0;
    int xdivs = NN;
    int ydivs = 1;
    do{
        int xstratum = (int)(xdivs * pt[0]);
        int ystratum = (int)(ydivs * pt[1]);
        if(occupiedStrata[shape][ystratum*xdivs+xstratum])
            return true;
        shape++;
        xdivs/=2;
        ydivs*=2;
    }while(xdivs != 0);
    return false;
}

bool PMJ02SampleSequenceGenerator_Pharr::minDist(SamplePMJ& pt, double* min_dist)
{
    double minSquareDist = 2; // On a unit grid. so can't be larger than this

    int c_i = int(grid_dim * pt[0] );
    int c_j = int(grid_dim * pt[1] );
    for (int i = -1; i <= 1; ++i)
    {
        int ii = c_i + i;
        if ( ii >= 0 && ii < grid_dim )
            for (int j = -1; j <= 1; ++j)
            {
                int jj = c_j + j;
                if ( jj >= 0 && jj < grid_dim )
                {
                    SamplePMJ* neighbor = grid_item( ii, jj );

                    if ( neighbor != nullptr )
                    {
                        double dist = pow((*neighbor)[0] - pt[0], 2) + 
                                      pow((*neighbor)[1] - pt[1], 2);
                        if(dist < minSquareDist)
                        {
                            minSquareDist = dist;
                        }
                    }
                }

            }
    }
    *min_dist = sqrt(minSquareDist);

    return ( minSquareDist > 1 ? false : true );
}

SamplePMJ *PMJ02SampleSequenceGenerator_Pharr::instantiateArray(int size) {
    SamplePMJ* toReturn  = new SamplePMJ  [size];
    for(int i = 0; i < size; i++)
        toReturn[i] = SamplePMJ(2);
    return toReturn;
}

double PMJ02SampleSequenceGenerator_Pharr::generateRandomDouble() {
    std::uniform_real_distribution<> uniform(0.0,1.0);
    return uniform(randomNumberGenerator);
}

int PMJ02SampleSequenceGenerator_Pharr::generateRandomInt( int max_val) {
    std::uniform_int_distribution<> uniform(0, max_val);
    return uniform(randomNumberGenerator);
}

void PMJ02SampleSequenceGenerator_Pharr::initialize_x_tree( Node &node, int i, int j,
                                                            int shape, int nx,
                                                            int ny )
{
    D( std::cout << "    i_x_t - " << i << ", " << j << ", " 
                 << shape << ", " << nx << ", " << ny << std::endl; )

   node.min_val = i * ny;
   node.max_val = node.min_val + ny;

   if ( occupiedStrata[ shape ][ nx*j + i ] )
      node.occupied = true;
   else
   {
      node.occupied = false;
      node.leaf = ny == 1 ? true : false;
      if ( !node.leaf )
      {
         Node &node_left = x_tree[ ++x_tree_idx ];
         node.left = x_tree_idx;
         initialize_x_tree( node_left, 2 * i, int( j / 2 ), shape - 1, 2 * nx,
                            ny / 2 );

         Node &node_right = x_tree[ ++x_tree_idx ];
         node.right = x_tree_idx;
         initialize_x_tree( node_right, 2 * i + 1, int( j / 2 ), shape - 1, 2 * nx,
                            ny / 2 );
         node.occupied = node_left.occupied & node_right.occupied;
      }
   }
}

void PMJ02SampleSequenceGenerator_Pharr::initialize_y_tree( Node &node, int i, int j,
                                                            int shape, int nx,
                                                            int ny )
{
    D( std::cout << "    i_y_t - " << i << ", " << j << ", " 
                 << shape << ", " << nx << ", " << ny << std::endl; )

   node.min_val = j * nx;
   node.max_val = node.min_val + nx;
   if ( occupiedStrata[ shape ][ nx*j + i ] )
      node.occupied = true;
   else
   {
      node.occupied = false;
      node.leaf = nx == 1 ? true : false;
      if ( !node.leaf )
      {
         Node &node_left = y_tree[ ++y_tree_idx ];
         node.left = y_tree_idx;
         initialize_y_tree( node_left, int( i / 2 ), 2 * j, shape + 1, nx / 2,
                            2 * ny );

         Node &node_right = y_tree[ ++y_tree_idx ];
         node.right = y_tree_idx;
         initialize_y_tree( node_right, int( i / 2 ), 2 * j + 1, shape + 1, nx / 2,
                            2 * ny );
         node.occupied = node_left.occupied & node_right.occupied;
      }
   }
}

void
PMJ02SampleSequenceGenerator_Pharr::valid_x_stratum( Node& node, int i, int j, int nx, int ny )
{
    if ( !node.occupied )
    {
        if ( node.leaf )
            x_offsets.push_back(i);
        else
        {
            valid_x_stratum( x_tree[node.left],  2*i,     floor(j/2), 2*nx, ny/2 );
            valid_x_stratum( x_tree[node.right], 2*i + 1, floor(j/2), 2*nx, ny/2 );
        }
    }
}

void
PMJ02SampleSequenceGenerator_Pharr::valid_y_stratum( Node& node, int i, int j, int nx, int ny )
{
    if ( !node.occupied )
    {
        if ( node.leaf )
            y_offsets.push_back(j);
        else
        {
            valid_y_stratum( y_tree[node.left],  floor(i/2), 2*j,     nx/2, 2*ny );
            valid_y_stratum( y_tree[node.right], floor(i/2), 2*j + 1, nx/2, 2*ny );
        }
    }
}
