#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "tokenize.h"

using namespace std;

namespace vparser {

  TEST_CASE("Empty string") {
    string test_str = "";
    vector<string> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 0);
    
  }
  
  TEST_CASE("Empty module") {
    string test_str = "module(); endmodule";
    vector<string> tokens = tokenize(test_str);

    for (auto& tok : tokens) {
      cout << tok << endl;
    }

    REQUIRE(tokens.size() == 5);
    
  }
}
