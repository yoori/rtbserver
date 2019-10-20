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

namespace Vanga
{
  template<typename LabelType>
  PredArrayHolder_var
  Predictor::predict(const SVM<LabelType>* svm) const throw()
  {
    PredArrayHolder_var res = new PredArrayHolder();
    res->values.resize(svm->size());

    unsigned long i = 0;
    for(auto group_it = svm->grouped_rows.begin();
      group_it != svm->grouped_rows.end(); ++group_it)
    {
      for(auto row_it = (*group_it)->rows.begin();
        row_it != (*group_it)->rows.end(); ++row_it, ++i)
      {
        res->values[i] = predict((*row_it)->features);
      }
    }

    return res;
  }
}
