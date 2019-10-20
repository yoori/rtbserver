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

#ifndef XGBOOSTPREDICTOR_HPP_
#define XGBOOSTPREDICTOR_HPP_

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Sync/SyncPolicy.hpp>
#include <String/SubString.hpp>

#include "CTRFeatureCalculators.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
namespace CTR
{
  class XGBoostPredictorPool:
    public ReferenceCounting::AtomicImpl
  {
  protected:
    // pimpl for xgboost library
    class PredictorWrapperDescriptor;
    class PredictorWrapper;
    typedef std::unique_ptr<PredictorWrapper> PredictorWrapperPtr;
    typedef std::list<PredictorWrapperPtr> PredictorWrapperList;

  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidConfig, Exception);

    struct Predictor: public ReferenceCounting::AtomicImpl
    {
    public:
      Predictor(
        XGBoostPredictorPool* pool,
        PredictorWrapperPtr&& predictor_impl)
        throw();

      float
      predict(
        const HashArray& hashes,
        const HashArray* add_hashes1 = 0,
        const HashArray* add_hashes2 = 0,
        const HashArray* add_hashes3 = 0);

    protected:
      virtual ~Predictor() throw();

    protected:
      ReferenceCounting::SmartPtr<XGBoostPredictorPool> pool_;
      PredictorWrapperPtr predictor_impl_;
    };

    typedef ReferenceCounting::SmartPtr<Predictor> Predictor_var;

  public:
    XGBoostPredictorPool(const String::SubString& model_file)
      throw(InvalidConfig);

    Predictor_var
    get_predictor() throw();

  protected:
    typedef Sync::Policy::PosixThread SyncPolicy;

  protected:
    virtual ~XGBoostPredictorPool() throw();

    void release_(PredictorWrapperPtr&& predictor_impl) throw();

  protected:
    // data required for initialize new PredictorWrapper
    std::unique_ptr<PredictorWrapperDescriptor> predictor_descriptor_;

    // PredictorWrapper pool
    SyncPolicy::Mutex lock_;
    PredictorWrapperList predictors_;
  };

  typedef ReferenceCounting::SmartPtr<XGBoostPredictorPool>
    XGBoostPredictorPool_var;
}
}
}
  
#endif /*XGBOOSTPREDICTOR_HPP_*/
