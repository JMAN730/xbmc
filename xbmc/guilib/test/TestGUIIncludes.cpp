/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "filesystem/File.h"
#include "guilib/GUIIncludes.h"
#include "test/TestUtils.h"
#include "utils/XBMCTinyXML.h"

#include <string>

#include <gtest/gtest.h>

TEST(TestGUIIncludes, ResolveNestedControlWithinWrapper)
{
  const std::string includeDefinition = R"(<includes>
    <include name="NestedTest">
      <control type="group">
        <control type="label">
          <nested />
        </control>
      </control>
    </include>
  </includes>)";

  XFILE::CFile* includeFile = XBMC_CREATETEMPFILE(".xml");
  ASSERT_NE(includeFile, nullptr);
  const std::string includePath = XBMC_TEMPFILEPATH(includeFile);
  includeFile->Close();
  ASSERT_TRUE(includeFile->OpenForWrite(includePath, true));
  const auto bytesWritten = includeFile->Write(includeDefinition.data(), includeDefinition.size());
  ASSERT_EQ(bytesWritten, static_cast<decltype(bytesWritten)>(includeDefinition.size()));
  includeFile->Close();

  CGUIIncludes includes;
  includes.Load(includePath);

  CXBMCTinyXML document;
  ASSERT_TRUE(document.Parse(R"(<window>
    <controls>
      <include content="NestedTest">
        <label>Expected label</label>
      </include>
    </controls>
  </window>)"));
  includes.Resolve(document.RootElement());

  const TiXmlElement* controls = document.RootElement()->FirstChildElement("controls");
  ASSERT_NE(controls, nullptr);
  const TiXmlElement* group = controls->FirstChildElement("control");
  ASSERT_NE(group, nullptr);
  const TiXmlElement* labelControl = group->FirstChildElement("control");
  ASSERT_NE(labelControl, nullptr);
  const TiXmlElement* label = labelControl->FirstChildElement("label");
  ASSERT_NE(label, nullptr);
  ASSERT_NE(label->FirstChild(), nullptr);
  EXPECT_STREQ(label->FirstChild()->Value(), "Expected label");
  EXPECT_EQ(labelControl->FirstChildElement("nested"), nullptr);

  EXPECT_TRUE(XBMC_DELETETEMPFILE(includeFile));
}
