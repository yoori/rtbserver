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


#ifndef AD_SERVER_CHANNEL_SERVICE_CHANNEL_ADMIN_APPLICATION_HPP_
#define AD_SERVER_CHANNEL_SERVICE_CHANNEL_ADMIN_APPLICATION_HPP_

#include <list>
#include <string>
#include <vector>
#include <memory>

#include <eh/Exception.hpp>

#include <Generics/Singleton.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <ChannelSvcs/ChannelCommons/ChannelUtils.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServer.hpp>
#include <ChannelSvcs/ChannelServer/ChannelLoadSessionImpl.hpp>
#include <ChannelSvcs/ChannelManagerController/ChannelManagerController.hpp>
#include <ChannelSvcs/ChannelManagerController/ChannelSessionFactory.hpp>

/**
 * \class Application Application.hpp "ChannelAdmin/Application.hpp"
 * Class which requests Channel Server and outputs result into std output.
 */
class Application
{
public:
    
  /**
   * Macros defining Application base exception class.
   */
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  
  /**
   * Macros defining InvalidArgument exception class.
   */
  DECLARE_EXCEPTION(InvalidArgument, Exception);

  class StatMarker
  {
  public:
    enum StatOption
    {
      STAT_NO,
      STAT_ALL,
      STAT_NORM,
      STAT_NOMAX,
      STAT_NOFIRST
    };

    StatMarker(
      const std::string& option,
      const char* name,
      unsigned long times)
      throw();

    template<typename Object, typename Function, typename...Args>
      void calc_stat(Object* call, Function func, Args&... args);

    template<typename Object, typename Function, typename ReturnType, typename...Args>
      void calc_stat_r(Object* call, Function func, ReturnType& ret, Args&... args);

    virtual ~StatMarker() throw();

  protected:

    void calc_value_(Generics::Time&& elapsed_time, size_t iteration)
      throw();

  private:
    StatOption option_;
    unsigned long times_;
    Generics::Statistics::DumpRunner_var stat_runner_;
    Generics::Statistics::DumpPolicy_var policy_;
    Generics::Statistics::Collection_var collection_;
    std::string main_name_;
    std::string no_max_name_;
    std::string no_first_name_;
    Generics::Time max_time_;
  };
    
public:
    
  /**
   * Construct Application object.
   */
  Application() throw(Exception, eh::Exception);

  /**
   * Destructs Application object.
   */
  virtual ~Application() throw();

/**
 * Parses arguments and issues corresponding request to ChannelServer.
 * @param argc Number of arguments passed to utility process
 * @param argv Arguments passed to utility process
 */
  int run(int argc, char** argv)
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  void deactivate_objects()
    throw(Generics::CompositeActiveObject::Exception, eh::Exception);

/**
 * Returns pointer to singleton object.
 */
  static Application& instance() throw(eh::Exception);


  int help(const std::string& topic) const 
    throw();

  template<class ITER>
  static std::string concat_sequence(ITER begin, ITER end) throw();

private:
  void init_server_interface_() throw(InvalidArgument);

  void init_update_interface_() throw(InvalidArgument);

  void init_capmaign_interface_() throw(InvalidArgument);

  int match_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  template<class T1, class T2>
  void make_match_query(
    T1* iface_ptr,
    T2& res)
    throw(Exception);

  int update_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int check_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int smartcheck_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int ccg_() 
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int pos_ccg_() 
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int ccg_traits_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  int lexemes_()
    throw(InvalidArgument, Exception, eh::Exception, CORBA::SystemException);

  void print_ccg_query_params_() throw();

  int parse_args_(int argc, char** argv) throw(InvalidArgument);

  static void print_ccg_answer_header_(int source_id, unsigned long start_id)
    throw();

  static void print_ccg_deleted_(
    const AdServer::ChannelSvcs::ChannelCurrent::TriggerVersionSeq& deleted)
    throw();

  static
  void
  parse_ids_(const String::SubString& str, std::vector<unsigned long>& out)
    throw(eh::Exception);

  static void print_unmatched(
    const char* name,
    const CORBACommons::StringSeq& unmatched)
    throw(eh::Exception);

  static void add_query_header_(
    const char* query,
    const char* reference,
    Table::Row& row) throw();

  template<typename CORBATYPE>
  void add_triggers(
    const CORBATYPE& data,
    std::ostream& positive_page_stream,
    std::ostream& negative_page_stream,
    std::ostream& positive_search_stream,
    std::ostream& negative_search_stream,
    std::ostream& positive_url_keyword_stream,
    std::ostream& negative_url_keyword_stream,
    std::ostream& positive_url_stream,
    std::ostream& negative_url_stream,
    std::ostream& uid_stream,
    bool print_cht_id)
    throw (eh::Exception);

private:

  bool use_session_;
  Generics::Time date_;
  std::vector<unsigned long> channels_id_;
  std::string reference_;
  std::string topic_;
  std::vector<std::string> lexemes_data_;
  Logging::Logger_var logger_;
  Generics::Statistics::Collection_var collection_;
  CORBA::Object_var obj_ref_; 
  AdServer::ChannelSvcs::ChannelServer_var channel_server_;
  AdServer::ChannelSvcs::ChannelUpdateBase_v33_var channel_update_;
  AdServer::ChannelSvcs::ChannelServerSession_var channel_session_;
  AdServer::ChannelSvcs::ChannelLoadSession_var load_session_;
  ChannelServerSessionFactoryImpl_var server_session_factory_;
  ChannelLoadSessionFactoryImpl_var load_session_factory_;
  CORBACommons::CorbaClientAdapter_var adapter_;
  Table::Filters filters_;
};

/**
 * Defines singleton class.
 */
typedef Generics::Singleton<Application> AppSingleton;
  
///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

//
// Application class
//  
inline
Application&
Application::instance() throw(eh::Exception)
{
  return Generics::Singleton<Application>::instance();
}

template<typename Object, typename Function, typename...Args>
void Application::StatMarker::calc_stat(Object* call, Function func, Args&... args)
{
  for(size_t i = 0; i < times_; i++)
  {
    Generics::Timer timer;
    try
    {
      timer.start();
    }
    catch(...)
    {
    }
    (*call.*func)(args...);
    try
    {
      timer.stop();
      calc_value_(timer.elapsed_time(), i);
    }
    catch(...)
    {
    }
  }
}

template<typename Object, typename Function, typename ReturnType, typename...Args>
void Application::StatMarker::calc_stat_r(Object* call, Function func, ReturnType& ret, Args&... args)
{
  for(size_t i = 0; i < times_; i++)
  {
    Generics::Timer timer;
    try
    {
      timer.start();
    }
    catch(...)
    {
    }
    ret = (*call.*func)(args...);
    try
    {
      timer.stop();
      calc_value_(timer.elapsed_time(), i);
    }
    catch(...)
    {
    }
  }
}

#endif // AD_SERVER_CHANNEL_SERVICE_CHANNEL_ADMIN_APPLICATION_HPP_
