#pragma once

#include <string>
#include <vector>

namespace vparser {

  class source_position {
  public:
    int lineNo;
    int linePos;

    source_position() : lineNo(1), linePos(1) {}

    source_position(const int lineNo_,
                    const int linePos_) : lineNo(lineNo_), linePos(linePos_) {}
  };

  class token {

    std::string text;
    source_position pos;

  public:
    token(const std::string& text_,
          const source_position& pos_) : text(text_), pos(pos_) {}

    token() : text("") {}

    std::string get_text() { return text; }
    source_position get_pos() { return pos; }
  };

  std::vector<token> tokenize(const std::string& verilog_code);

}
