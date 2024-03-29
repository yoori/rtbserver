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

#pragma once

#include <algorithm>
#include <list>
#include <Generics/DirSelector.hpp>
#include <Generics/Time.hpp>
#include <Logger/Logger.hpp>

namespace AdServer
{
  namespace Predictor
  {

    DECLARE_EXCEPTION(InvalidFile, eh::DescriptiveException);

    /**
     * File object descriptor
     */
    struct FileObject
    {
      /**
       * @brief Constructor.
       *
       * @param file path
       */
      FileObject(const char* filepath);

      std::string path;
      std::string filename;       // basename for filepath 
      Generics::Time timestamp;   // timestamp from filename
      Generics::Time last_modify; // modification time
    };

    typedef std::list<FileObject> FileObjectList;

    /**
     * Campaign folder files container (sorted by timestamps)
     */
    class CampaignFolder
    {
    public:
      typedef std::pair<unsigned long, FileObject> FilePair;
      typedef std::list<FilePair> FileList;
      
    public:
      
      /**
       * @brief Constructor.
       *
       * @param full path of the campaign folder
       * @param campaign id
       * @param interval to keep files in the container
       */
      CampaignFolder(
        const std::string& full_path,
        unsigned long campaign_id,
        Generics::Time keep_interval);

      /**
       * @brief Get campaign file list.
       *
       * @return file object list
       */
      const FileObjectList& get_files() const;

      /**
       * @brief Move file from top of the container
       * to <campaig_id, FileObject> pair list for processing.
       *
       * @param timestamp
       * @param[out] <campaig_id, FileObject> pair list
       */
      void move_file_to_list(
        Generics::Time timestamp,
        FileList& file_pairs);

      /**
       * @brief Destructor.
       */
      virtual ~CampaignFolder() throw() = default;
      
    private:
      std::string full_path_;
      unsigned long campaign_id_;
      FileObjectList file_list_;
    };

    /**
     * Campaign folders container (root folder)
     */
    class CampaignFoldersContainer
    {
      DECLARE_EXCEPTION(InvalidCampaignPath, eh::DescriptiveException);

      typedef std::list<CampaignFolder> FolderTable;
      
    public:

      /**
       * @brief Constructor.
       *
       * @param path
       * @param interval to keep files in the container
       * @param logger
       */
      CampaignFoldersContainer(
        const char* path,
        Generics::Time keep_interval,
        Logging::Logger* logger);
      
      /**
       * @brief Get files.
       *
       * @param[out] <campaig_id, FileObject> pair list
       */
      void get_files(
        CampaignFolder::FileList& file_pairs);

      /**
       * @brief Destructor.
       */
      virtual ~CampaignFoldersContainer() throw();
    private:
      FolderTable campaign_folder_list_;
      std::string path_;
      Generics::Time keep_interval_;
      Logging::Logger_var logger_;
    };      

    /**
     * @brief Check file has been changed since the last processing.
     *
     * @param <campaig_id, FileObject> pair
     * @param output (LibSVM) path
     */
    bool check_file_is_changed(
      const CampaignFolder::FilePair& file_pair,
      const std::string& output_path);

    /**
     * @brief Check files have been changed since the last processing.
     *
     * @param <campaig_id, FileObject> pair list
     * @param output (LibSVM) path
     */
    bool check_files_changed(
      const CampaignFolder::FileList& file_list,
      const std::string& output_path);

  }
}

