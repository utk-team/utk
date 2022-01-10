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

#include "Kronecker.hpp"

const double Kronecker::R2[2] = {
  0.75487766624669276,
  0.56984029099805327
};

const double Kronecker::R3[3] = {
  0.81917251339616444,
  0.67104360670378921,
  0.54970047790197027
};

const double Kronecker::R4[4] = {
  0.85667488385450287,
  0.73389185662712599,
  0.62870672103780863,
  0.53859725722361005
};

const double Kronecker::K21_2[2] = {
  static_cast<double>(std::sqrt((long double)506598872547596.0)/(long double)29147227.0),
  static_cast<double>(std::sqrt((long double)107882942223468.0)/(long double)28993644.0)
};

const double Kronecker::K21_3[3] = {
  static_cast<double>(std::sqrt((long double)136155583282554.0)/(long double)19015340.0),
  static_cast<double>(std::sqrt((long double)263438703080803.0)/(long double)17181595.0),
  static_cast<double>(std::sqrt((long double)352662070147437.0)/(long double)22118332.0)
};

const double Kronecker::K21_4[4] = {
  static_cast<double>(std::sqrt((long double)1062447381118571.0)/(long double)33084971.0),
  static_cast<double>(std::sqrt((long double)147063651917932.0)/(long double)30639341.0),
  static_cast<double>(std::sqrt((long double)711707016062345.0)/(long double)29661368.0),
  static_cast<double>(std::sqrt((long double)328399936443598.0)/(long double)27256281.0)
};

const double Kronecker::K21b_2[2] = {
  static_cast<double>(std::sqrt((long double)415745956465435.0)/(long double)32662800.0),
  static_cast<double>(std::sqrt((long double)16340581432791.0)/(long double)25338159.0)
};

const double Kronecker::K21b_3[3] = {
  static_cast<double>(std::sqrt((long double)6742281674969.0)/(long double)20126138.0),
  static_cast<double>(std::sqrt((long double)42845384312863.0)/(long double)18315113.0),
  static_cast<double>(std::sqrt((long double)1044922263929.0)/(long double)25238999.0)
};

const double Kronecker::K21b_4[4] = {
  static_cast<double>(std::sqrt((long double)79054014721081.0)/(long double)17204034.0),
  static_cast<double>(std::sqrt((long double)7916082904289.0)/(long double)18894472.0),
  static_cast<double>(std::sqrt((long double)859650028021546.0)/(long double)29772799.0),
  static_cast<double>(std::sqrt((long double)623200003618550.0)/(long double)27601088.0)
};

// ====================================

//
// stateless generation
//
double Kronecker::get_value(uint32_t index, uint8_t dim, const double *alphas)
{
  double ret = alphas[dim] * static_cast<double>(index);
  ret -= std::floor(ret);
  return ret;
}
