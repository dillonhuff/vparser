#pragma once

#include <string>
#include <vector>

namespace vparser {

  class macro_def {

    std::string name;
    std::vector<std::string> arg_names;
    std::vector<std::string> body;

  public:

    macro_def(const std::string& name_,
              const std::vector<std::string>& arg_names_,
              const std::vector<std::string>& body_) :
      name(name_), arg_names(arg_names_), body(body_) {}

    std::string get_name() const { return name; }
    std::vector<std::string> get_arg_names() const { return arg_names; }
    std::vector<std::string> get_body() const { return body; }
  };

  class preprocessed_verilog {
  public:
    std::vector<macro_def> defs;
    std::string text;
  };

  preprocessed_verilog preprocess_code(const std::string& verilog_text);

}
