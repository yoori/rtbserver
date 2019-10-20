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


#define DISABLE_OPENMP
#include <xgboost/io/simple_dmatrix-inl.hpp>
#include <xgboost/learner/learner-inl.hpp>
#undef DISABLE_OPENMP

#include <Generics/MMap.hpp>
#include "XGBoostPredictor.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
namespace CTR
{
  // XGBoostPredictorPool::PredictorWrapperDescriptor
  struct XGBoostPredictorPool::PredictorWrapperDescriptor
  {
    PredictorWrapperDescriptor(const String::SubString& model_file)
      throw(InvalidConfig);

    const std::string model_file_name;
    Generics::ArrayByte model_buffer;
    std::unique_ptr<rabit::utils::MemoryFixSizeBuffer> model_stream;
  };

  // XGBoostPredictorPool::PredictorWrapper
  // thread unsafe wrapper for xgboost::learner::BoostLearner
  class XGBoostPredictorPool::PredictorWrapper:
    protected xgboost::learner::BoostLearner
  {
  public:
    PredictorWrapper(const PredictorWrapperDescriptor& init)
      throw(InvalidConfig);

    void
    init() throw();

    float
    predict(
      const HashArray& hashes,
      const HashArray* add_hashes1 = 0,
      const HashArray* add_hashes2 = 0,
      const HashArray* add_hashes3 = 0);

  protected:
    static void
    fill_feats_(
      std::vector<xgboost::RowBatch::Entry>& feats,
      const HashArray& hashes);

  protected:
    std::unique_ptr<xgboost::io::DMatrixSimple> dmatrix_;
    std::vector<xgboost::RowBatch::Entry> feats_;
    std::vector<float> preds_;
  };

  // XGBoostPredictorPool::PredictorWrapperDescriptor impl
  XGBoostPredictorPool::
  PredictorWrapperDescriptor::PredictorWrapperDescriptor(
    const String::SubString& file)
    throw(InvalidConfig)
    : model_file_name(file.str())
  {
    static const char* FUN = "XGBoostPredictorPool::"
      "PredictorWrapperDescriptor::PredictorWrapperDescriptor()";

    try
    {
      // force removing of mmapped region from RAM by OS:
      // copy it to internal buffer
      Generics::MMapFile mmap_file(file.str().c_str());
      model_buffer.reset(mmap_file.length());
      ::memcpy(model_buffer.get(), mmap_file.memory(), mmap_file.length());
      model_stream.reset(
        new rabit::utils::MemoryFixSizeBuffer(model_buffer.get(), mmap_file.length()));
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": can't open model file '" << file << "': " << ex.what();
      throw InvalidConfig(ostr);
    }
  }

  // XGBoostPredictorPool::PredictorWrapper impl
  XGBoostPredictorPool::
  PredictorWrapper::PredictorWrapper(const PredictorWrapperDescriptor& init)
    throw(InvalidConfig)
  {
    static const char* FUN = "XGBoostPredictorPool::PredictorWrapper::PredictorWrapper()";

    rabit::utils::MemoryFixSizeBuffer model_stream_copy(*init.model_stream);
    // workaround for difference in LoadModel(IStream) and LoadModel(const char*)
    char header[5] = {0, 0, 0, 0, 0};
    if(model_stream_copy.Read(header, 4) != 4)
    {
      Stream::Error ostr;
      ostr << FUN << ": invalid model file wihtout header: '" <<
        init.model_file_name << "'";
      throw InvalidConfig(ostr);
    }

    // disable num feature calculation with using rabit (thread and processes unsafe)
    // use num feature from model file
    const bool CALC_NUM_FEATURE = false;

    if(::strcmp(header, "bs64") == 0)
    {
      xgboost::utils::Base64InStream bsin(&model_stream_copy);
      bsin.InitPosition();
      BoostLearner::LoadModel(bsin, CALC_NUM_FEATURE);
    }
    else if(::strcmp(header, "binf") == 0)
    {
      BoostLearner::LoadModel(model_stream_copy, CALC_NUM_FEATURE);
    }
    else
    {
      model_stream_copy.Seek(0);
      BoostLearner::LoadModel(model_stream_copy, CALC_NUM_FEATURE);
    }

    dmatrix_.reset(new xgboost::io::DMatrixSimple());
    feats_.reserve(10*1024);
    preds_.reserve(1);
  }

  void
  XGBoostPredictorPool::
  PredictorWrapper::init() throw()
  {
    // do mock predict for initalize big buffers inside predictor
    predict(HashArray(), 0);
  }

  float
  XGBoostPredictorPool::
  PredictorWrapper::predict(
    const HashArray& hashes,
    const HashArray* add_hashes1,
    const HashArray* add_hashes2,
    const HashArray* add_hashes3)
  {
    // input SVM contains indexes, predict require indexes - 1
    fill_feats_(feats_, hashes);

    if(add_hashes1)
    {
      fill_feats_(feats_, *add_hashes1);
    }

    if(add_hashes2)
    {
      fill_feats_(feats_, *add_hashes2);
    }

    if(add_hashes3)
    {
      fill_feats_(feats_, *add_hashes3);
    }

    dmatrix_->AddRow(feats_);

    BoostLearner::Predict(*dmatrix_, false, &preds_);
    float result = *preds_.begin();

    feats_.clear();
    dmatrix_->Clear();
    preds_.clear();

    return result;
  }

  void
  XGBoostPredictorPool::
  PredictorWrapper::fill_feats_(
    std::vector<xgboost::RowBatch::Entry>& feats,
    const HashArray& hashes)
  {
    for(HashArray::const_iterator it = hashes.begin(); it != hashes.end(); ++it)
    {
      feats.push_back(xgboost::RowBatch::Entry(it->first - 1, static_cast<float>(it->second)));
    }
  }
  
  // XGBoostPredictorPool::Predictor impl
  XGBoostPredictorPool::Predictor::Predictor(
    XGBoostPredictorPool* pool,
    PredictorWrapperPtr&& predictor_impl)
    throw()
    : pool_(ReferenceCounting::add_ref(pool)),
      predictor_impl_(std::move(predictor_impl))
  {}

  XGBoostPredictorPool::Predictor::~Predictor() throw()
  {
    // return PredictorWrapper to pool
    pool_->release_(std::move(predictor_impl_));
  }

  float
  XGBoostPredictorPool::
  Predictor::predict(
    const HashArray& hashes,
    const HashArray* add_hashes1,
    const HashArray* add_hashes2,
    const HashArray* add_hashes3)
  {
    return predictor_impl_->predict(
      hashes, add_hashes1, add_hashes2, add_hashes3);
  }

  // XGBoostPredictorPool::XGBoostPredictorPool impl
  XGBoostPredictorPool::XGBoostPredictorPool(
    const String::SubString& model_file)
    throw(InvalidConfig)
  {
    predictor_descriptor_.reset(new PredictorWrapperDescriptor(model_file));
    // create one PredictorWrapper (validate model)
    predictors_.push_back(
      PredictorWrapperPtr(new PredictorWrapper(*predictor_descriptor_)));
    (*predictors_.begin())->init();
  }

  XGBoostPredictorPool::~XGBoostPredictorPool()
    throw()
  {}

  XGBoostPredictorPool::Predictor_var
  XGBoostPredictorPool::get_predictor()
    throw()
  {
    PredictorWrapperList predictors;

    {
      SyncPolicy::WriteGuard lock(lock_);
      if(!predictors_.empty())
      {
        predictors.splice(predictors.begin(), predictors_, predictors_.begin());
      }
    }

    if(predictors.empty())
    {
      // create new learner
      predictors.push_back(
        PredictorWrapperPtr(new PredictorWrapper(*predictor_descriptor_)));
    }

    return new Predictor(this, std::move(*predictors.begin()));
  }

  void
  XGBoostPredictorPool::release_(PredictorWrapperPtr&& predictor_impl)
    throw()
  {
    PredictorWrapperList predictors;
    predictors.push_back(std::move(predictor_impl));

    SyncPolicy::WriteGuard lock(lock_);
    predictors_.splice(predictors_.begin(), predictors);
  }
}
}
}
