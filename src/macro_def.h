#pragma once

#include <string>
#include <vector>

namespace vparser {

  class macro_def {

    std::string name;

  public:

    macro_def(const std::string& name,
              const std::vector<std::string>& arg_names) {}

    std::string get_name() const { return name; }
  };

  class preprocessed_verilog {
  public:
    std::vector<macro_def> defs;
    std::string text;
  };

  preprocessed_verilog preprocess_code(const std::string& verilog_text);

}
