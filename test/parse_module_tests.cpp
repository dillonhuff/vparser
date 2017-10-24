#include "catch.hpp"

#include "parse.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace vparser {

  TEST_CASE("Empty module has no parameters") {
    verilog_module vm = parse_module("module m(); endmodule");

    REQUIRE(vm.get_port_names().size() == 0);
    
  }

  TEST_CASE("Empty module with 3 parameters") {
    verilog_module vm = parse_module("module m(a, b, c); endmodule");

    for (auto& n : vm.get_port_names()) {
      cout << n << endl;
    }

    REQUIRE(vm.get_port_names().size() == 3);
    
  }

  TEST_CASE("Reading a real file with multiple statements") {
    std::ifstream t("./test/samples/cb_unq1.v");
    std::string str((std::istreambuf_iterator<char>(t)),
		    std::istreambuf_iterator<char>());

    verilog_module vm = parse_module(str);

    REQUIRE(vm.get_port_names().size() == 16);

    REQUIRE(vm.get_statements().size() == 19);
  }

}
