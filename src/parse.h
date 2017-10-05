#pragma once

#include <string>
#include <vector>

namespace vparser {

  class verilog_module {

    std::vector<std::string> port_names;

  public:

    verilog_module() {}

    verilog_module(const std::vector<std::string>& port_names_) :
      port_names(port_names_) {}

    std::vector<std::string> get_port_names() const {
      return port_names;
    }
  };

  verilog_module parse_module(const std::string& mod_string);

}
