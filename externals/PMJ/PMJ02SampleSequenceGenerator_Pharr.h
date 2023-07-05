/*
 * renderdude <https://github.com/renderdude>
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

#ifndef PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_PHARR_H
#define PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_PHARR_H

#include <random>
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include "SamplePMJ.h"

struct Node
{
    bool occupied, leaf;
    int min_val, max_val;
    int left, right;

    friend std::ostream& operator<<( std::ostream& os, Node const& f )
    {
        os << "(" << f.occupied << ", " << f.leaf << ", " << f.min_val << ", "
           << f.max_val << ", " << f.left << ", " << f.right << ")";

        return os;
    }
};

class PMJ02SampleSequenceGenerator_Pharr {
public:
    PMJ02SampleSequenceGenerator_Pharr(int seed = 42);
    ~PMJ02SampleSequenceGenerator_Pharr();
    void ProgressiveMultiJittered02Algorithm2D(int numberOfSamplesToGenerate, int numberOfCandidates = 10);
    void exportSampleSet(std::string &outputPath);

    
    /**
     * Get pointer to samples
     */
    const SamplePMJ* GetSamples() const { return generatedSamples; }
    /**
     * Get number of generated samples
     */
    int GetSampleCount() const { return numberOfSamplesToGenerate; }
private:
    std::mt19937 randomNumberGenerator;
    int numberOfSamplesToGenerate;
    SamplePMJ* generatedSamples;
    std::vector< std::vector<bool> > occupiedStrata;
    int* xhalves;
    int* yhalves;
    int numSamples;
    int numberOfCandidates;
    int grid_dim;
    std::vector<Node> x_tree, y_tree;
    std::vector<int> x_offsets, y_offsets;
    std::vector<SamplePMJ*> grid;
    int x_tree_idx, y_tree_idx;

    float min_dist;

    void extendSequenceEven(int alreadyGeneratedSamples);
    void extendSequenceOdd(int alreadyGeneratedSamples);
    void markOccupiedStrata(int alreadyGeneratedSamples);
    void markOccupiedStrata1(SamplePMJ &pt, int NN);
    void generateSamplePoint(int i, int j, int xhalf, int yhalf, double n, int N);
    bool isOccupied(SamplePMJ &pt, int NN);
    bool minDist(SamplePMJ& pt, double* min_dist);
    SamplePMJ* instantiateArray(int size);
    double generateRandomDouble();
    int generateRandomInt( int max_val);
    void initialize_x_tree( Node& node, int i, int j, int shape, int nx, int ny );
    void initialize_y_tree( Node& node, int i, int j, int shape, int nx, int ny );
    void valid_x_stratum( Node& node, int i, int j, int nx, int ny );
    void valid_y_stratum( Node& node, int i, int j, int nx, int ny );
    SamplePMJ* grid_item(int i, int j ) const { return grid[ j * grid_dim + i ]; }
    void set_grid_item( SamplePMJ* pt )
    { 
        int idx = int((*pt)[1]*grid_dim) * grid_dim + int((*pt)[0]*grid_dim);
        grid[ idx ] = pt;
    }
};


#endif //PROGRESSIVEMULTIJITTEREDSAMPLESEQUENCESGENERATOR_PMJ02SAMPLESEQUENCEGENERATOR_PHARR_H
