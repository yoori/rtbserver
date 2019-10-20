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

#include "Predictor.hpp"
#include "DTree.hpp"

namespace Vanga
{
  // Predictor
  PredArrayHolder_var
  Predictor::predict(const RowArray& rows) const throw()
  {
    PredArrayHolder_var res = new PredArrayHolder();
    res->values.resize(rows.size());

    unsigned long i = 0;
    for(auto it = rows.begin(); it != rows.end(); ++it, ++i)
    {
      res->values[i] = predict((*it)->features);
    }

    return res;
  }

  DTree_var
  Predictor::as_dtree() throw()
  {
    return nullptr;
  }

  PredictorSet_var
  Predictor::as_predictor_set() throw()
  {
    return nullptr;
  }

  // LogRegPredictor
  LogRegPredictor::LogRegPredictor(Predictor* predictor) throw()
    : predictor_(ReferenceCounting::add_ref(predictor))
  {}

  double
  LogRegPredictor::predict(const FeatureArray& features) const throw()
  {
    const double DOUBLE_ONE = 1.0;
    return DOUBLE_ONE / (DOUBLE_ONE + std::exp(-predictor_->predict(features)));
  }

  void
  LogRegPredictor::save(std::ostream& ostr) const
  {
    predictor_->save(ostr);
  }

  std::string
  LogRegPredictor::to_string(
    const char* prefix,
    const FeatureDictionary* dict,
    double base)
    const throw()
  {
    return predictor_->to_string(prefix, dict, base);
  }

  DTree_var
  LogRegPredictor::as_dtree() throw()
  {
    return predictor_->as_dtree();
  }

  PredictorSet_var
  LogRegPredictor::as_predictor_set() throw()
  {
    return predictor_->as_predictor_set();
  }
}
