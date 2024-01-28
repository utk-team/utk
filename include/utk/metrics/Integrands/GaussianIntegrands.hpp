/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
 * All rights reserved.
 * 
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
#pragma once

#include <utk/utils/FastPRNG.hpp>
#include "../IntegrationTest.hpp"

#include <vector>
#include <random>

#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

namespace utk
{
    // https://rosettacode.org/wiki/QR_decomposition#C
    typedef struct {
        int m, n;
        double ** v;
    } mat_t, *mat;

    mat matrix_new(int m, int n)
    {
        mat x = (mat)malloc(sizeof(mat_t));
        x->v  = (double**)malloc(sizeof(double*) * m);
        x->v[0] = (double*)calloc(sizeof(double), m * n);
        for (int i = 0; i < m; i++)
            x->v[i] = x->v[0] + n * i;
        x->m = m;
        x->n = n;
        return x;
    }

    void matrix_delete(mat m)
    {
        free(m->v[0]);
        free(m->v);
        free(m);
    }

    void matrix_transpose(mat m)
    {
        for (int i = 0; i < m->m; i++) {
            for (int j = 0; j < i; j++) {
                double t = m->v[i][j];
                m->v[i][j] = m->v[j][i];
                m->v[j][i] = t;
            }
        }
    }

    mat matrix_copy(int n, double** a, int m)
    {
        mat x = matrix_new(m, n);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                x->v[i][j] = a[i][j];
        return x;
    }

    mat matrix_mul(mat x, mat y)
    {
        if (x->n != y->m) return 0;
        mat r = matrix_new(x->m, y->n);
        for (int i = 0; i < x->m; i++)
            for (int j = 0; j < y->n; j++)
                for (int k = 0; k < x->n; k++)
                    r->v[i][j] += x->v[i][k] * y->v[k][j];
        return r;
    }

    mat matrix_minor(mat x, int d)
    {
        mat m = matrix_new(x->m, x->n);
        for (int i = 0; i < d; i++)
            m->v[i][i] = 1;
        for (int i = d; i < x->m; i++)
            for (int j = d; j < x->n; j++)
                m->v[i][j] = x->v[i][j];
        return m;
    }

    /* c = a + b * s */
    double *vmadd(double a[], double b[], double s, double c[], int n)
    {
        for (int i = 0; i < n; i++)
            c[i] = a[i] + s * b[i];
        return c;
    }

    /* m = I - v v^T */
    mat vmul(double v[], int n)
    {
        mat x = matrix_new(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                x->v[i][j] = -2 *  v[i] * v[j];
        for (int i = 0; i < n; i++)
            x->v[i][i] += 1;

        return x;
    }

    /* ||x|| */
    double vnorm(double x[], int n)
    {
        double sum = 0;
        for (int i = 0; i < n; i++) sum += x[i] * x[i];
        return sqrt(sum);
    }

    /* y = x / d */
    double* vdiv(double x[], double d, double y[], int n)
    {
        for (int i = 0; i < n; i++) y[i] = x[i] / d;
        return y;
    }

    /* take c-th column of m, put in v */
    double* mcol(mat m, double *v, int c)
    {
        for (int i = 0; i < m->m; i++)
            v[i] = m->v[i][c];
        return v;
    }

    void householder(mat m, mat *R, mat *Q)
    {
        double* e = (double*) malloc(sizeof(double) * m->m);
        double* x = (double*) malloc(sizeof(double) * m->m);
        mat* q = (mat*) malloc(sizeof(mat) * m->m);

        mat z = m, z1;
        for (int k = 0; k < m->n && k < m->m - 1; k++) {
            double a;

            z1 = matrix_minor(z, k);
            if (z != m) matrix_delete(z);
            z = z1;

            mcol(z, x, k);
            a = vnorm(x, m->m);
            if (m->v[k][k] > 0) a = -a;

            for (int i = 0; i < m->m; i++)
                e[i] = (i == k) ? 1 : 0;
            
            vmadd(x, e, a, e, m->m);
            vdiv(e, vnorm(e, m->m), e, m->m);
            q[k] = vmul(e, m->m);
            z1 = matrix_mul(q[k], z);
            if (z != m) matrix_delete(z);
            z = z1;
        }
        matrix_delete(z);
        *Q = q[0];
        *R = matrix_mul(q[0], m);

        for (int i = 1; i < m->n && i < m->m - 1; i++) {
            z1 = matrix_mul(q[i], *Q);
            if (i > 1) matrix_delete(*Q);
            *Q = z1;
            matrix_delete(q[i]);
        }

        // matrix_delete(q[0]);
        z = matrix_mul(*Q, m);
        matrix_delete(*R);

        *R = z;
        matrix_transpose(*Q);   
    }

    void matrix_show(mat m)
    {
        for(int i = 0; i < m->m; i++) {
            for (int j = 0; j < m->n; j++) {
                printf(" %8.3f", m->v[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    mat random_unitary_matrix(uint32_t D, utk::PCG32& mt)
    {
        std::normal_distribution<double> distribution(0, 1.0);
        
        if (D == 1)
        {
            mat Q = matrix_new(1, 1);
            double val = distribution(mt);
            Q->v[0][0] = (0.0 < val) - (val < 0.0);
            return Q;
        }
        mat matrix = matrix_new(D, D);

        for (uint32_t i = 0; i < D; i++)
            for (uint32_t j = 0; j < D; j++)
                matrix->v[i][j] = distribution(mt);

        mat Q, R;
        householder(matrix, &R, &Q);

        // Normalize Q to make sure decomposition is unique
        for (uint32_t i = 0; i < D; i++)
        {
            double coef = 1.; // R->v[i][i] / abs(R->v[i][i]);
            for (uint32_t j = 0; j < D; j++) 
                Q->v[i][j] = Q->v[i][j] * coef;
        }
            
        matrix_delete(matrix);
        matrix_delete(R);

        return Q;
    }

    
    class GaussianIntegrand : public Integrand
    {
    public:
        GaussianIntegrand(uint32_t d) : Integrand(d) 
        { 
            mu.resize(dim);
            invSigma.resize(dim * dim);
        }
    
        void GenerateRandom(const GenerationParameter& params, uint64_t seed) override
        {
            // Some default values...
            double sigmaMin = 0.01;
            double sigmaMax = 1.00;

            if (params.find("smin") != params.end())
                sigmaMin = std::get<double>(params.at("smin")); 
            if (params.find("smax") != params.end())
                sigmaMax = std::get<double>(params.at("smax"));

            utk::PCG32 mt(seed);
            mat rot = random_unitary_matrix(dim, mt);
            
            std::uniform_real_distribution<double> unifMu(0, 1);
            std::uniform_real_distribution<double> unifSigma(sigmaMin, sigmaMax);
            
            for (uint32_t i = 0; i < dim; i++)
                mu[i] = unifMu(mt);
            
            std::vector<double> sigmas(dim, 0.0);
            for (uint32_t i = 0; i < dim; i++)
                sigmas[i] = 1. / unifSigma(mt);

            // Q = rot matrix (rotation), S = diagonal matrix (size)
            // Covariance matrix is QSQ' which is SDP
            // Its inverse : Q S^-1 Q', because Q^-1 = Q'
            for (uint32_t i = 0; i < dim; i++)
            {
                for (uint32_t j = 0; j < dim; j++)
                {
                    invSigma[j + i * dim] = 0.0;
                    for (uint32_t k = 0; k < dim; k++)
                    {
                        invSigma[j + i * dim] += rot->v[i][k] * sigmas[k] * rot->v[j][k];
                    }
                }
            }

            matrix_delete(rot);
        }   

        void ReadFromStream(std::istream& stream) override 
        {
            for (uint32_t i = 0; i < dim; i++)
                stream >> mu[i];
            
            for (uint32_t i = 0; i < dim; i++)
                for (uint32_t j = 0; j < dim; j++)
                    stream >> invSigma[j + i * dim];
        }

        virtual void WriteInStream (std::ostream& stream) const override
        {
            for (uint32_t i = 0; i < dim; i++)
                stream << mu[i] << " ";
            
            for (uint32_t i = 0; i < dim; i++)
                for (uint32_t j = 0; j < dim; j++)
                    stream << invSigma[j + i * dim] << " ";
        }

        virtual double eval(const double* pts) const override
        {
            double rslt = 0.0;
            for (uint32_t i = 0; i < dim; i++)
            {
                double tmp = 0.0;
                for (uint32_t j = 0; j < dim; j++)
                    tmp += invSigma[j + i * dim] * (pts[j] - mu[j]);
                rslt += tmp * (pts[i] - mu[i]);
            }
            return std::exp(-0.5 * rslt);
        }

        ~GaussianIntegrand()
        { }
        
    private:
        std::vector<double> mu;
        std::vector<double> invSigma;
    };
};
