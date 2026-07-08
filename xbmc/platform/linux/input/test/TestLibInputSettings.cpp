/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "platform/linux/input/LibInputSettings.h"
#include "settings/lib/SettingDefinitions.h"

#include <vector>

#include <gtest/gtest.h>

TEST(TestLibInputSettings, SortLayoutsByLabel)
{
  std::vector<StringSettingOption> layouts{
      {"Zulu Layout", "aa"},
      {"Alpha Layout", "zz"},
      {"Mike Layout", "mm"},
  };

  CLibInputSettings::SortLayouts(layouts);

  ASSERT_EQ(layouts.size(), 3);
  EXPECT_EQ(layouts[0].label, "Alpha Layout");
  EXPECT_EQ(layouts[1].label, "Mike Layout");
  EXPECT_EQ(layouts[2].label, "Zulu Layout");
}
