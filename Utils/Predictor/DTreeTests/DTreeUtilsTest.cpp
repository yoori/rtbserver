/* 
 * This file is part of the RTBServer distribution (https://github.com/yoori/rtbserver).
 * RTBServer is DSP server that allow to bid (see RTB auction) targeted ad
 * via RTB protocols (OpenRTB, Google AdExchange, Yandex RTB)
 *
 * Copyright (c) 2014 Yuri Kuznecov <yuri.kuznecov@gmail.com>.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <list>
#include <vector>
#include <iterator>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <unordered_set>

#include <Generics/Rand.hpp>
#include <Generics/AppUtils.hpp>
#include <Generics/BitAlgs.hpp>
#include <Generics/TaskRunner.hpp>

#include <Commons/ErrorHandler.hpp>
#include <Commons/CsvReader.hpp>
#include <Commons/Algs.hpp>
#include <Commons/FileManip.hpp>

#include <Utils/Predictor/DTree/Utils.hpp>

// main
int
main(int, char** argv)
{
  int labels = ::atoi(argv[1]);
  double p_base = ::atof(argv[2]);
  p_base = std::min(std::max(p_base, 0.000001), 1 - 0.000001);
  //p_base = 0.5;
  double base = std::log(p_base / (1 - p_base)); // p = std::log(base)
  double exp_base = std::exp(base);

  std::deque<std::pair<double, unsigned long> > exp_array;
  //exp_array.push_back(0.999);
  /*
  exp_array.push_back(std::make_pair(exp_base, 1)); // xi = 0, p = 0.5
  exp_array.push_back(std::make_pair(exp_base, 1)); // xi = 0, p = 0.5
  exp_array.push_back(std::make_pair(exp_base, 1)); // xi = 0, p = 0.5
  */

  exp_array.push_back(std::make_pair(1.05478e-20, 1)); // xi = 0, p = 0.5
  exp_array.push_back(std::make_pair(9.76988e-17, 1)); // xi = 0, p = 0.5
  exp_array.push_back(std::make_pair(2.68812e+43, 1)); // xi = 0, p = 0.5

  std::cout << "probes: " << exp_array.size() <<
    ", labels: " << labels <<
    ", pbase: " << p_base <<
    ", base: " << base <<
    ", expbase: " << exp_base << std::endl;

  double res = Vanga::Utils::solve_grouped_logloss_min(exp_array, exp_array.size() - labels);
  std::cout << "res = " << res << std::endl;

  for(auto exp_it = exp_array.begin(); exp_it != exp_array.end(); ++exp_it)
  {
    //const double p = 1.0 / (1.0 + std::exp(- (- std::log(*exp_it) + res)));
    const double p = 1.0 / (1.0 + std::exp(- (base + res)));
    std::cout << "exp base: " << exp_it->first << ", pred = " << p << std::endl;
  }

  return 0;
}


