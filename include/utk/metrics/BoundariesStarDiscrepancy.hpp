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

#include <utk/utils/Pointset.hpp>
#include <cmath>

namespace utk
{
    // Author : Eric Thiemard 
    // Version : 20.10.99       
    // This program is an implementation of the method proposed in Eric 
    // Thiemard's paper "An algorithm to compute bounds for the star    
    // discrepancy" submitted to Journal of Complexity. 
    //
    //  Reference:
    // 
    //    Eric Thiemard,
    //    An Algorithm to Compute Bounds for the Star Discrepancy,
    //    Journal of Complexity,
    //    Volume 17, pages 850-880, 2001.   
    class BoundariesStarDiscrepancy
    {
    public:
        BoundariesStarDiscrepancy(double eps = -1.0) 
        { epsilon_ = eps; }

        template<typename T>
        std::pair<T, T> compute(const Pointset<T>& pts)
        {
            borne_sup = 0.0;
            borne_inf = 0.0;

            n = pts.Npts();
            s = pts.Ndim();
            subtotala = 0;
            subtotalb = 0;
            num = 1;
            den = 2;
            num2 = den - num;

            if (epsilon_ < 0.0)
                epsilon = std::pow(std::log2(n), s-1) / (double)n;
            else
                epsilon = epsilon_;
            epsilon = std::clamp(epsilon, 0.0, 1 - 1e-10);
            
            double* oalpha;
            double* obeta;
            double* newPts;

            if constexpr(!std::is_same_v<T, double>)
            {
                newPts = new double[n * s];
                for (uint64_t i = 0; i < n * s; i++)
                    newPts[i] = static_cast<T>(pts.Data()[i]);
                points = newPts;
            }
            else
            {
                points = pts.Data();
            }

            oalpha = (double *) calloc ( (unsigned) s, sizeof(double) );
            obeta  = (double *) calloc ( (unsigned) s, sizeof(double) );
            
            supertree( );
		    initlex ( );
            
            for (int i = 0; i < s; i++ )
                obeta[i] = 1.0;
            
		    decomposition ( oalpha, obeta, 0, 1.0 );

            if constexpr(!std::is_same_v<T, double>)
                delete[] newPts;
                
            free(oalpha);
            free(obeta);
            free(superarbre);
            free(maxsizealpha); 
            free(lexsizealpha); 
            free(maxsizebeta); 
            free(lexsizebeta);
            // Trees : lexalpha, lexbeta
            // Are seemingly freed by 'traiter'. 
            // TODO : check this with valgrind :( 

            return {borne_inf, borne_sup };
        }
   
        template<typename T>
        std::vector<std::pair<T, T>> compute(const std::vector<Pointset<T>>& ptss)
        {
            std::vector<std::pair<T, T>> rslts;
            rslts.reserve(ptss.size());
            for (const auto& pts : ptss)
                rslts.push_back(compute(pts));
            return rslts;
        }
        
    private:
        struct sommet
        {
            int id;
            sommet *pt;
        };

        double epsilon_;

        int s;
        size_t n;
        int num;
        int den;
        int num2;
        int subtotala = 0;
        int subtotalb = 0;
        int *lexsizealpha;
        int *maxsizealpha;
        int *lexsizebeta;
        int *maxsizebeta;
        double epsilon;
        double borne_sup = 0.0;
        double borne_inf = 0.0;
        const double *points;
        sommet *superarbre;
        sommet **lexalpha;
        sommet **lexbeta;
	
	
        void quicksort ( sommet *liste, int dim, int l, int r )
        {
            int i = l;
            int j = r+1;
            int tmp;
            double pivot = points[dim+(liste[l].id)*s];

            while ( i < j )
            {
                do
                i++;
                while ( i < r && points[dim+(liste[i].id)*s] < pivot );

                do
                j--;
                while ( pivot < points[dim+(liste[j].id)*s] );

                if ( i < j )
                {
                tmp = liste[i].id;
                liste[i].id = liste[j].id;
                liste[j].id = tmp;
                }

            }

            tmp = liste[l].id;
            liste[l].id = liste[j].id;
            liste[j].id = tmp;

            if ( l < j-1 )
            {
                quicksort ( liste, dim, l, j-1 );
            }

            if ( j+1 < r )
            {
                quicksort ( liste, dim, j+1, r );
            }

            return;
        }
        
        
        //TREE

        bool supertree ()
        {
            uint32_t i;
            superarbre = (sommet *) calloc ( (unsigned) n+1, sizeof(sommet) );

            for ( i = 1; i <= n; i++ )
                superarbre[i].id = i - 1;

            superarbre[0].id = (int)n;

            quicksort ( superarbre, 0, 1, (int)n );

            return true;
        }
        
        sommet *subtree ( sommet *liste, int min, int next, int dim )
        {
            int i;
            int aux;
            int n2;
            sommet *newarbre;

            aux = min - 1;
            n2 = next - min + 1;
            newarbre = (sommet *) calloc ( (unsigned) n2+1, sizeof(sommet) );

            for ( i = 1; i <= n2; i++ )
            {
                newarbre[i].id = liste[i+aux].id;
            }

            newarbre[0].id = n2;

            if ( 1 < n2 )
            {
                quicksort ( newarbre, dim, 1, n2 );
            }

            return newarbre;
        }
        
        void freetree ( sommet *noeud )
        //  Purpose:
        //    FREETREE frees storage associated with a tree.
        {
            int i;
            int max = noeud[0].id;

            for ( i = 1; i <= max; i++ )
            {
                if ( noeud[i].pt != NULL )
                {
                freetree ( noeud[i].pt );
                }
            }

            free ( noeud );

            return;
        }
        
        
        //LEXICON
        
        void initlex ( )
        //  Purpose:
        //    INITLEX initializes the lexicon.
        {
            int i;

            maxsizealpha = (int *) calloc ( (unsigned) s, sizeof(int) );

            for ( i = 0; i < s; i++ )
            {
                maxsizealpha[i] = 1;
            }

            lexsizealpha = (int *) calloc ( (unsigned) s, sizeof(int) );
            lexsizealpha[0] = 1;
            lexalpha = (sommet **) calloc ( (unsigned) s, sizeof(sommet *) );

            for ( i = 0; i < s; i++ )
            {
                lexalpha[i] = (sommet *) calloc ( (unsigned) maxsizealpha[i], sizeof(sommet) );
            }

            lexalpha[0][0].id = 1;
            lexalpha[0][0].pt = superarbre;

            maxsizebeta = (int *) calloc ( (unsigned) s, sizeof(int) );

            for ( i = 0; i < s; i++ )
            {
                maxsizebeta[i] = 1;
            }

            lexsizebeta = (int *) calloc ( (unsigned) s, sizeof(int) );
            lexsizebeta[0] = 1;
            lexbeta = (sommet **) calloc ( (unsigned) s, sizeof(sommet *) );
            
            for ( i = 0; i < s; i++ )
            {
                lexbeta[i] = (sommet *) calloc ( (unsigned) maxsizebeta[i], sizeof(sommet) );
            }

            lexbeta[0][0].id = 1;
            lexbeta[0][0].pt = superarbre;

            return;
        }
        
        void insertlex ( sommet *noeud, int range, int *maxsize, int *lexsize, 
                sommet **lex )
        //  Purpose:
        //    INSERTLEX inserts an item into the lexicon.
        {
            int i = lexsize[range];

            if ( i == maxsize[range] )
            {
                maxsize[range] *= 2;

                lex[range] = ( sommet * ) realloc ( lex[range], 
                (unsigned) maxsize[range]*sizeof(sommet) );
            }

            lex[range][i].pt = noeud;
            lex[range][i].id = 1;
            lexsize[range] = ++i;

            return;
        }
        
        
        //ALGORITHM
        
        void decomposition ( double *alpha, double *beta, int min, double value )
        //  Purpose:
        //    DECOMPOSITION carries out the decomposition of a subinterval.
        {
            int i;
            double pbetaminp = 1.0;
            double palpha = 1.0;
            double pbeta;
            double delta;
            double *subalpha;
            double *subbeta;
            double *gamma;

            subalpha = (double *) calloc ( (unsigned) s, sizeof(double) );
            subbeta  = (double *) calloc ( (unsigned) s+1, sizeof(double) );
            gamma  = (double *) calloc ( (unsigned) s+1, sizeof(double) );

            for ( i = min; i < s; i++ )
            {
                pbetaminp = pbetaminp * beta[i];
            }
            pbeta = pbetaminp;

            for ( i = 0; i < min; i++ )
            {
                pbetaminp = pbetaminp * beta[i];
                palpha = palpha * alpha[i];
            }

            pbetaminp = pbetaminp - epsilon;
            delta = pow ( pbetaminp / ( pbeta * palpha ), 1.0 / ( s - min ) );

            for ( i = 0; i < min; i++ )
            {
                gamma[i] = alpha[i];
                subalpha[i] = gamma[i];
                subbeta[i] = beta[i];
            }

            for ( i = min; i < s; i++ )
            {
                gamma[i] = delta * beta[i];
                subalpha[i] = alpha[i];
                subbeta[i] = beta[i];
            }

            subbeta[min] = gamma[min];

            value = value * delta;
            if ( epsilon < value )
            {
                for ( i = min; i < s; i++ )
                {
                decomposition ( subalpha, subbeta, i, value );
                subalpha[i]  = gamma[i];
                subbeta[i]   = beta[i];
                subbeta[i+1] = gamma[i+1];
                }
            }
            else
            {
                for ( i = min; i < s; i++ )
                {
                traiter ( subalpha, subbeta, (i==0)?0:i-1 );
                subalpha[i]  = gamma[i];
                subbeta[i]   = beta[i];
                subbeta[i+1] = gamma[i+1];
                }
            }

            traiter ( gamma, beta, s-1 );

            free ( gamma );
            free ( subalpha );
            free ( subbeta );

            return;
        }
        
        void traiter ( double *outputalpha, double *outputbeta, int range )
        {
            int i;
            double valpha = 1.0;
            double vbeta = 1.0;
            double newborne;
            int nalpha;
            int nbeta;

            for ( i = 0; i < s; i++ )
            {
                valpha = valpha * outputalpha[i];
                vbeta = vbeta * outputbeta[i];
            }

            nalpha = fastexplore ( outputalpha, range, maxsizealpha, lexsizealpha,
            lexalpha, &subtotala );

            nbeta = fastexplore ( outputbeta, range, maxsizebeta, lexsizebeta, 
                lexbeta, &subtotalb );

            newborne = ( (double) nbeta / n ) - valpha;

            if ( borne_sup < newborne )
            {
                borne_sup = newborne;
            }

            newborne = vbeta - ( (double) nalpha / n );

            if ( borne_sup < newborne )
            {
                borne_sup = newborne;
            }

            borne_inf = lowbound ( nalpha, valpha, outputalpha );
            borne_inf = lowbound ( nbeta,  vbeta,  outputbeta );

            return;
        }
        
        double lowbound ( int npoints, double volume, double *pave )
        //  Purpose:
        //    LOWBOUND computes the lower bound.
        {
            uint32_t i;
            int j;
            double tmp;

            if ( borne_inf < fabs ( volume - ( (double) npoints / n ) ) )
            {
                if ( volume < ( (double) npoints / n ) )
                {
                volume = 1.0;
                for ( j = 0; j < s; j++ )
                {
                    tmp = 0.0;
                    for ( i = 0; i < n; i++ )
                    {
                    if ( tmp < points[j+i*s] && points[j+i*s] <= pave[j] )
                    {
                        tmp = points[j+i*s];
                    }
                    }
                    volume = volume * tmp;
                }
                }
                else
                {
                volume = 1.0;
                for ( j = 0; j < s; j++ )
                {
                    tmp = 1.0;
                    for ( i = 0; i < n; i++ )
                    {
                    if ( points[j+i*s] < tmp && pave[j] <= points[j+i*s] )
                    {
                        tmp = points[j+i*s];
                    }
                    }
                    volume = volume * tmp;
                }
                }
                return fabs ( volume - ( (double) npoints / n ) );
            }
            else
            {
                return borne_inf;
            }
        }
        
        int fastexplore ( double *pave, int range, int *maxsize, int *lexsize,
                        sommet **lex, int *subtotal )
        {
            int j;
            int i;
            int min;
            int max;
            int next;
            int start;
            int size = lexsize[range];
            int right;
            int total = 0;
            double seuil = pave[range];
            sommet refnoeud;
            sommet *noeud;

            if ( range == s - 1 )
            {
                for ( i = size-1; 0 <= i; i-- )
                {
                refnoeud = lex[range][i];
                noeud = refnoeud.pt;
                min = refnoeud.id;
                max = noeud[0].id;
                if ( max < min )
                {
                    lexsize[range]--;
                    lex[range][i] = lex[range][lexsize[range]];
                    *subtotal = *subtotal + min - 1;
                }
                else
                {
                    total = total + min - 1;
                    right = 1;
                    while ( min <= max )
                    {
                    next = ( min + max + 1 ) / 2;
                    if ( points[range+(noeud[next].id)*s] < seuil )
                    {    
                        total = total + next - min + 1;
                        min = next + 1;
                        if ( right == 1 )
                        {
                        lex[range][i].id = min;
                        }
                    }
                    else
                    {
                        right = 0;
                        max = next - 1;
                    }
                    }
                }
                }
                total = total + *subtotal;
            }
            else
            {
                *subtotal = 0;
                lexsize[range+1] = 0;
                for ( i = 0; i < size; i++ )
                {
                refnoeud = lex[range][i];
                noeud = refnoeud.pt;
                start = refnoeud.id;
                min = 1;
                max = noeud[0].id;
                while ( min != start )
                {
                    next = ( ( 1 + min ) * num2 + max * num ) / den;
                    insertlex ( noeud[next].pt, range+1, maxsize, lexsize, lex );
                    total = total + explore ( noeud[next].pt, pave, range+1 );
                    min = next + 1;
                }
                right = 1;
                while ( min <= max )
                {
                    next = ( ( 1 + min ) * num2 + max * num ) / den;
                    if ( points[range+(noeud[next].id)*s] < seuil )
                    {
                    if ( noeud[next].pt == NULL )
                    {
                        noeud[next].pt = subtree ( noeud, min, next, range+1 );
                    }
                    insertlex ( noeud[next].pt, range+1, maxsize, lexsize, lex );
                    total = total + explore ( noeud[next].pt, pave, range+1 );
                    min = next + 1;
                    if ( right == 1 )
                    {
                        if ( range == 0 )
                        {
                        if ( lex == lexalpha )
                        {
                            for ( j = lex[range][i].id; j < next; j++ )
                            {
                            if ( noeud[j].pt != NULL )
                            {
                                freetree ( noeud[j].pt );
                            }
                            }
                        }
                        }
                        lex[range][i].id = min;
                    }
                    }
                    else
                    {
                    right = 0;
                    max = next - 1;
                    }
                }
                }
            }
            return total;
        }
        
        int explore ( sommet *liste, double *pave, int dim )
        {
            int i;
            int min = 1;
            int max;
            int next;
            int total;

            if ( pave[dim] <= points[dim+(liste[1].id)*s] )
            {
                return 0;
            }

            if ( liste[0].id == 1 )
            {
                total = 1;
                next = liste[1].id; 
                for ( i = dim; i < s; i++ )
                {
                if ( pave[i] <= points[i+next*s] )
                {
                    total = 0;
                    break;
                }
                }
            }
            else
            {
                total = 0;
                max = liste[0].id;

                if ( dim == s-1 )
                {
                if ( points[dim+(liste[max].id)*s] < pave[dim] )
                {
                    total = max;
                }
                else
                {
                    while ( min <= max )
                    {
                    next = ( min + max + 1 ) / 2;
                    if ( points[dim+(liste[next].id)*s] < pave[dim] )
                    {    
                        total = total + next - min + 1;
                        min = next + 1;
                    }
                    else
                    {
                        max = next - 1;
                    }
                    }
                }
                }
                else
                {
                while ( min <= max )
                {
                    next = ( ( 1 + min ) * num2 + max * num ) / den;
                    if ( points[dim+(liste[next].id)*s] < pave[dim] )
                    {
                    if ( liste[next].pt == NULL )
                    {
                        liste[next].pt = subtree ( liste, min, next, dim+1 );
                    }
                    total = total + explore ( liste[next].pt, pave, dim+1 );
                    min = next + 1;
                    }
                    else
                    {
                    max = next - 1;
                    }
                }
                }
            }
            return total;
        }
    };
};
