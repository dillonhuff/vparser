#include "catch.hpp"

#include "parse.h"

namespace vparser {

  TEST_CASE("Empty module has no parameters") {
    verilog_module vm = parse_module("module(); endmodule");

    REQUIRE(vm.get_port_names().size() == 0);
    
  }

}
