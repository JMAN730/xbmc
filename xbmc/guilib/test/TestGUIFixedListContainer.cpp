/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "guilib/GUIFixedListContainer.h"
#include "guilib/GUIListItem.h"
#include "guilib/listproviders/IListProvider.h"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace
{

class TestListProvider : public IListProvider
{
public:
  TestListProvider() : IListProvider(0) {}

  std::unique_ptr<IListProvider> Clone() override { return std::make_unique<TestListProvider>(); }
  bool Update(bool forceRefresh) override { return forceRefresh; }
  void Fetch(std::vector<std::shared_ptr<CGUIListItem>>& items) override
  {
    items.clear();
    for (int i = 0; i < 5; ++i)
      items.emplace_back(std::make_shared<CGUIListItem>());
  }
  bool OnClick(const std::shared_ptr<CGUIListItem>& item) override { return false; }
};

class TestableGUIFixedListContainer : public CGUIFixedListContainer
{
public:
  TestableGUIFixedListContainer()
    : CGUIFixedListContainer(0,
                             0,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             VERTICAL,
                             CScroller{},
                             0,
                             3,
                             0,
                             0,
                             FixedListAlignY::CENTER)
  {
  }

  int Cursor() const { return GetCursor(); }
  int Offset() const { return GetOffset(); }
};

} // unnamed namespace

TEST(TestGUIFixedListContainer, PositionsInitialStaticItemsAtFocusPosition)
{
  TestableGUIFixedListContainer container;

  container.SetListProvider(std::make_unique<TestListProvider>());

  EXPECT_EQ(container.GetSelectedItem(), 0);
  EXPECT_EQ(container.Cursor(), 3);
  EXPECT_EQ(container.Offset(), -3);
}
