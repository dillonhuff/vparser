#include "catch.hpp"

#include "parse.h"

#include <iostream>

using namespace std;

namespace vparser {

  TEST_CASE("Empty module has no parameters") {
    verilog_module vm = parse_module("module(); endmodule");

    REQUIRE(vm.get_port_names().size() == 0);
    
  }

  TEST_CASE("Empty module with 3 parameters") {
    verilog_module vm = parse_module("module(a, b, c); endmodule");

    for (auto& n : vm.get_port_names()) {
      cout << n << endl;
    }

    REQUIRE(vm.get_port_names().size() == 3);
    
  }
  
}