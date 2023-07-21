/*
 * Coded by Mayur Patel
 * Copyright (c) 2021
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
 * either expressed or implied, of this project.
 */
#ifndef HEADERKRONECKER
#define HEADERKRONECKER

#include <limits>
#include <cstdint>
#include <cmath>

namespace Kronecker {

// ====================================
// Irrationals for driving the sequence:

// Roberts 2018
extern const double R2[2];
extern const double R3[3];
extern const double R4[4];

// Patel 2021
extern const double K21_2[2];
extern const double K21_3[3];
extern const double K21_4[4];

extern const double K21b_2[2];
extern const double K21b_3[3];
extern const double K21b_4[4];

// ====================================

//
// template parameterized by dimension:
// we expect the optimizer to be able to recognize
// that the loops should be unrolled.
//
template< uint8_t DIM >
class Generator {
  protected:
    uint32_t _alphas[DIM];
    uint32_t _values[DIM];

    static constexpr double _normalize_up   = static_cast<double>(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + static_cast<uint64_t>(1));
    static constexpr float  _normalize_down = 1.0f / static_cast<float>(_normalize_up);

  public:
    Generator(const double *alphas, uint32_t permute=1){
      for(uint8_t x=0; x<DIM; ++x)
        _alphas[x] = static_cast<uint32_t>(alphas[x] * _normalize_up);
      for(uint8_t x=0; x<DIM; ++x)
        _values[x] = permute * _alphas[(x+1)%DIM];
    }

    ~Generator(void){}

    float get_current_value(uint8_t dim) const{
      // map fixed-point representation back to floating-point
      return static_cast<float>(_values[dim]) * _normalize_down;
    }

    void next(void){
      // when using full-width fixed-point integer representation,
      // no mod 1 operation is necessary.
      for(uint8_t x=0; x<DIM; ++x)
        _values[x] += _alphas[x];
    }
};

// ====================================

//
// Jittering as presented by Patel,
// evolved from Roberts' original
//
template< uint8_t DIM >
class JitteredGenerator {
  protected:
    Generator<DIM> _s0;
    Generator<DIM> _s1;
    float          _j;
    uint32_t       _count;
    float          _amplitude;

  protected:

    void _set_amplitude(void) {
      const float edge_length = std::pow(_count, -1.0f/static_cast<float>(DIM));
      const float edge_adjust = 0.78f / std::sqrt(DIM);
      _amplitude = edge_length * edge_adjust * _j;
    }

  public:
    JitteredGenerator(const double *primary_alphas, const double *secondary_alphas, float jitter=0.02, uint32_t permute=1) :
      _s0(primary_alphas, permute),
      _s1(secondary_alphas, permute),
      _j(jitter),
      _count(1)
      { _set_amplitude(); }

    ~JitteredGenerator(void){}

    float get_current_value(uint8_t dim) const {
      float ret = _s0.get_current_value(dim) + _amplitude * _s1.get_current_value(dim);
      return ret > 1.0f ? ret - 1.0f : ret;
    }

    void next(void){
      _s0.next();
      _s1.next();
      ++_count;
      _set_amplitude();
    }
};

// ====================================


//
// stateless generation
//
double get_value(uint32_t index, uint8_t dim, const double *alphas);


// ====================================

} // namespace KronSeq

#endif
