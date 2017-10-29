#pragma once

#include <vector>

namespace vparser {

  class macro_def {
  };

  std::vector<macro_def> parse_macro_defs(const std::string& verilog_text);

}
