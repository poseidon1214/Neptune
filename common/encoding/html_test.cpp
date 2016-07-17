// Copyright (c) 2015, Tencent Inc.
// Author: Li Wenting <wentingli@tencent.com>

#include "common/encoding/html.h"

#include <string>
#include "thirdparty/gtest/gtest.h"

const std::string document = "<!DOCTYPE html><html><body>"
                             "<h1>My First Heading</h1>"
                             "<p>My first \"paragraph & word\".</p>"
                             " </body></html>";

TEST(HtmlEncodeDecode, EntityName) {
  std::string output;
  gdt::HtmlEncode(document, &output);
  gdt::HtmlDecode(output, &output);
  EXPECT_EQ(document, output);
}

TEST(HtmlEncodeDecode, EntityNumber) {
  std::string output;
  std::string encoded_doc = "&#60;!DOCTYPE html&#62;&#60;html&#62;&#60;body"
                            "&#62;&#60;h1&#62;My &#x46;irst Heading&#60;/h1"
                            "&#62;&#60;p&#62;My first &#34;paragraph &#38;"
                            "&#32;word&#34;.&#60;/p&#62;&nbsp;&#x3c;/body"
                            "&#62;&#60;/html&#x3E;";
  gdt::HtmlDecode(encoded_doc, &output);
  EXPECT_EQ(document, output);
}

TEST(HtmlEncodeDecode, DecodeFailure) {
  std::string output;
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#62body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&copy;body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#162;body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#62z;body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#x7F;body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#x;body", &output));
  EXPECT_FALSE(gdt::HtmlDecode("&#60;!DOCTYPE html&#;body", &output));
}

TEST(HtmlEncodeDecode, IgnoreError) {
  EXPECT_EQ("<!DOCTYPE html&#62body",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#62body"));
  EXPECT_EQ("<!DOCTYPE html&copy;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&copy;body&#62;"));
  EXPECT_EQ("<!DOCTYPE html&#162;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#162;body&#62;"));
  EXPECT_EQ("<!DOCTYPE html&#62z;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#62z;body&#62;"));
  EXPECT_EQ("<!DOCTYPE html&#x7F;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#x7F;body&#62;"));
  EXPECT_EQ("<!DOCTYPE html&#x;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#x;body&#62;"));
  EXPECT_EQ("<!DOCTYPE html&#;body>",
            gdt::HtmlDecodeIgnoreError("&#60;!DOCTYPE html&#;body&#62;"));
}

