#pragma once

#include <string>
#include <vector>

namespace vparser {

  class macro_def {
  };

  class preprocessed_verilog {
  public:
    std::vector<macro_def> defs;
    std::string text;
  };

  preprocessed_verilog preprocess_code(const std::string& verilog_text);

}
