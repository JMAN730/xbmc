/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FileItem.h"
#include "filesystem/File.h"
#include "test/TestUtils.h"
#include "utils/FileUtils.h"

#include <gtest/gtest.h>

TEST(TestFileUtils, DeleteItem_CFileItemPtr)
{
  XFILE::CFile *tmpfile;
  std::string tmpfilepath;

  ASSERT_NE(nullptr, (tmpfile = XBMC_CREATETEMPFILE("")));
  tmpfilepath = XBMC_TEMPFILEPATH(tmpfile);

  CFileItemPtr item(new CFileItem(tmpfilepath));
  item->SetPath(tmpfilepath);
  item->SetFolder(false);
  item->Select(true);
  tmpfile->Close();  //Close tmpfile before we try to delete it
  EXPECT_TRUE(CFileUtils::DeleteItem(item));
  EXPECT_FALSE(XBMC_DELETETEMPFILE(tmpfile));
}

TEST(TestFileUtils, DeleteItemString)
{
  XFILE::CFile *tmpfile;

  ASSERT_NE(nullptr, (tmpfile = XBMC_CREATETEMPFILE("")));
  tmpfile->Close();  //Close tmpfile before we try to delete it
  EXPECT_TRUE(CFileUtils::DeleteItem(XBMC_TEMPFILEPATH(tmpfile)));
  EXPECT_FALSE(XBMC_DELETETEMPFILE(tmpfile));
}

TEST(TestFileUtils, GetFileSize)
{
  XFILE::CFile* tmpfile;
  ASSERT_NE(nullptr, (tmpfile = XBMC_CREATETEMPFILE("")));
  const std::string tmpfilepath = XBMC_TEMPFILEPATH(tmpfile);

  const std::string content = "0123456789";
  EXPECT_EQ(static_cast<ssize_t>(content.size()),
            tmpfile->Write(content.data(), content.size()));
  tmpfile->Close();

  EXPECT_EQ(static_cast<int64_t>(content.size()), CFileUtils::GetFileSize(tmpfilepath));

  XBMC_DELETETEMPFILE(tmpfile);
}

TEST(TestFileUtils, GetFileSizeNonExistingFile)
{
  EXPECT_EQ(-1, CFileUtils::GetFileSize("special://temp/does-not-exist-12345.mkv"));
}

TEST(TestFileUtils, GetFileSizeEmptyPath)
{
  EXPECT_EQ(-1, CFileUtils::GetFileSize(""));
}

/* Executing RenameFile() requires input from the user */
// static bool RenameFile(const std::string &strFile);
