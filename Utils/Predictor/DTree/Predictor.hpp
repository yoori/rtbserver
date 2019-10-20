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

#ifndef PREDICTOR_HPP_
#define PREDICTOR_HPP_

#include <iostream>
#include <map>
#include <vector>

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

#include "SVM.hpp"

namespace Vanga
{
  typedef std::map<unsigned long, std::string> FeatureDictionary;

  struct PredArrayHolder: public ReferenceCounting::AtomicImpl
  {
    typedef std::deque<double> PredArray;

    PredArray values;

  protected:
    virtual ~PredArrayHolder() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<PredArrayHolder> PredArrayHolder_var;

  class DTree;
  typedef ReferenceCounting::SmartPtr<DTree> DTree_var;

  class PredictorSet;
  typedef ReferenceCounting::SmartPtr<PredictorSet> PredictorSet_var;

  // Predictor
  class Predictor: public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  public:
    virtual double
    predict(const FeatureArray& features) const throw() = 0;

    PredArrayHolder_var
    predict(const RowArray& rows) const throw();

    template<typename LabelType>
    PredArrayHolder_var
    predict(const SVM<LabelType>* svm) const throw();

    virtual void
    save(std::ostream& ostr) const = 0;

    virtual std::string
    to_string(
      const char* prefix,
      const FeatureDictionary* dict = nullptr,
      double base = 0.0)
      const throw() = 0;

    virtual DTree_var
    as_dtree() throw();

    virtual PredictorSet_var
    as_predictor_set() throw();

  protected:
    virtual ~Predictor() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<Predictor> Predictor_var;

  // LogRegPredictor
  class LogRegPredictor:
    public Predictor,
    public ReferenceCounting::AtomicImpl
  {
  public:
    LogRegPredictor(Predictor* predictor) throw();

    virtual double
    predict(const FeatureArray& features) const throw();

    virtual void
    save(std::ostream& ostr) const;

    virtual std::string
    to_string(
      const char* prefix,
      const FeatureDictionary* dict = nullptr,
      double base = 0.0)
      const throw();

    virtual DTree_var
    as_dtree() throw();

    virtual PredictorSet_var
    as_predictor_set() throw();

  protected:
    virtual ~LogRegPredictor() throw() {}

  protected:
    Predictor_var predictor_;
  };
}

#include "Predictor.tpp"

#endif /*PREDICTOR_HPP_*/
