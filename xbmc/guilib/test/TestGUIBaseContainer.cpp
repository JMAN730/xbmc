/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "guilib/GUIBaseContainer.h"
#include "guilib/GUIListItem.h"

#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

class TestableGUIBaseContainer : public CGUIBaseContainer
{
public:
  TestableGUIBaseContainer()
    : CGUIBaseContainer(0, 0, 0.0f, 0.0f, 0.0f, 0.0f, VERTICAL, CScroller{}, 0)
  {
  }

  CGUIControl* Clone() const override { return nullptr; }

  void SetItems(std::initializer_list<std::string_view> labels)
  {
    m_items.clear();
    for (const std::string_view label : labels)
      m_items.emplace_back(std::make_shared<CGUIListItem>(std::string{label}));
  }

  const std::vector<std::pair<int, std::string>>& GetLetterOffsets()
  {
    UpdateScrollByLetter();
    return m_letterOffsets;
  }
};

TEST(TestGUIBaseContainer, GroupsAccentedLeadingLetters)
{
  TestableGUIBaseContainer container;
  container.SetItems({"Alpha", "\xC3\x80 propos", "\xC3\x82me", "Bravo"});

  const auto& offsets = container.GetLetterOffsets();

  ASSERT_EQ(offsets.size(), 2U);
  EXPECT_EQ(offsets[0], std::make_pair(0, std::string{"A"}));
  EXPECT_EQ(offsets[1], std::make_pair(3, std::string{"B"}));
}
