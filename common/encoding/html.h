// Copyright (c) 2015, Tencent Inc.
// Author: Li Wenting <wentingli@tencent.com>

#ifndef COMMON_ENCODING_HTML_H_
#define COMMON_ENCODING_HTML_H_

#include <string>
#include "common/base/string/string_piece.h"

namespace gdt {

// Reserved characters in HTML must be replaced with character reference.
// There are two types of character reference:
//
//    1. Numeric character reference: &#nnnn; or &#xhhhh;
//    2. Character entity reference:  &name;
//
// In a HTML document the character entity references &lt;, &gt;, &quot;
// and &amp; are predefined in HTML and SGML, because <, >, " and & are
// already used to delimit markup.(NOT include XML's &apos; entity)
//
// The following functions are defined:
//
//    1. HtmlEncode
//       Encode an input string with the character entity references above
//
//    2. HtmlDecode
//       Decode an input string according to the rules below:
//
//          * Support ASCII printable characters 32~126 when
//            a numeric entity in decimal or hexadecimal form is encountered
//
//          * Support &quot;, &amp;, &lt;, &gt;, &nbsp; when
//            a named entity is encountered
//
//    3. HtmlDecodeIgnoreError
//       The same as HtmlDecode but output entity literally
//       when error occurs during decoding
//
// References:
//
//    1. Character encodings in HTML
//       https://en.wikipedia.org/wiki/Character_encodings_in_HTML
//
//    2. List of XML and HTML character entity references
//       https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references

bool HtmlEncode(const StringPiece& in, std::string* out);
bool HtmlDecode(const StringPiece& in, std::string* out);
std::string HtmlDecodeIgnoreError(const StringPiece& in);

}  // namespace gdt

#endif  // COMMON_ENCODING_HTML_H_
