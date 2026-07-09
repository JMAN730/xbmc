/*
 *  Copyright (C) 2010-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

class CFileItem;
class CDateTime;

class CFileUtils
{
public:
  static bool CheckFileAccessAllowed(const std::string &filePath);
  static bool DeleteItem(const std::shared_ptr<CFileItem>& item);
  static bool DeleteItem(const std::string &strPath);
  static bool Exists(const std::string& strFileName, bool bUseCache = true);
  /** \brief Delete a file with or without gui confirmation unless setting is off.
  */
  static bool DeleteItemWithConfirm(const std::shared_ptr<CFileItem>& item);
  static bool RenameFile(const std::string &strFile);
  static bool RemoteAccessAllowed(const std::string &strPath);
  static unsigned int LoadFile(const std::string &filename, void* &outputBuffer);
  /*! \brief Get the modified date of a file if its invalid it returns the creation date - this behavior changes when you set bUseLatestDate
  \param strFileNameAndPath path to the file
  \param bUseLatestDate use the newer datetime of the files mtime and ctime
  \return Returns the file date, can return a invalid date if problems occur
  */
  static CDateTime GetModificationDate(const std::string& strFileNameAndPath, const bool& bUseLatestDate);
  static CDateTime GetModificationDate(const int& code, const std::string& strFileNameAndPath);
  /*! \brief Get the size of a file in bytes. Stacks are the sum of their parts,
  for files in archives the size of the archive is returned.
  \param strFileNameAndPath path to the file
  \return Returns the file size in bytes, -1 if it could not be determined
  */
  static int64_t GetFileSize(const std::string& strFileNameAndPath);
};
