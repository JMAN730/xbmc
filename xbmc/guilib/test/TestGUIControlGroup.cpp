/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "guilib/GUIControlGroup.h"

#include <gtest/gtest.h>

TEST(TestGUIControlGroup, ClearAllRemovesDescendantLookup)
{
  CGUIControlGroup parent{0, 1, 0.0f, 0.0f, 0.0f, 0.0f};
  auto* child = new CGUIControlGroup{1, 2, 0.0f, 0.0f, 0.0f, 0.0f};
  child->AddControl(new CGUIControlGroup{2, 3, 0.0f, 0.0f, 0.0f, 0.0f});
  parent.AddControl(child);

  child->ClearAll();

  EXPECT_EQ(parent.GetControl(3), nullptr);
}

TEST(TestGUIControlGroup, RemoveControlDetachesChildBeforeParentDestruction)
{
  auto* child = new CGUIControlGroup{1, 2, 0.0f, 0.0f, 0.0f, 0.0f};
  {
    CGUIControlGroup parent{0, 1, 0.0f, 0.0f, 0.0f, 0.0f};
    parent.AddControl(child);
    EXPECT_TRUE(parent.RemoveControl(child));
  }

  child->ClearAll();
  delete child;
}
