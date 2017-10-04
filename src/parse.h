#pragma once

#include <string>
#include <vector>

namespace vparser {

  class verilog_module {
  public:

    std::vector<std::string> get_port_names() const {
      return {};
    }
  };

  verilog_module parse_module(const std::string& mod_string);

}
