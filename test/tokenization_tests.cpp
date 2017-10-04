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

    REQUIRE(tokens.size() == 5);
  }

  TEST_CASE("Comment line") {
    string test_str = " \n // Hello this is a comment string // asdf\n asdf //";
    vector<string> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 1);
  }

  TEST_CASE("Equals sign") {
    vector<string> tokens = tokenize("assign a = b;");
    REQUIRE(tokens[2] == "=");
    REQUIRE(tokens.size() == 5);
  }

  TEST_CASE("Equals sign with strange ending") {
    vector<string> tokens = tokenize("assign a = b;\n\n  ");
    REQUIRE(tokens[2] == "=");
    REQUIRE(tokens.size() == 5);
  }
  
  TEST_CASE("Multiple comments in a row") {
    string test_str = " \n // Hello this is a comment string \n// ";
    vector<string> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 0);
  }
  
  TEST_CASE("Real verilog file") {
    std::ifstream t("./test/samples/memory_core_unq1.v");
    std::string str((std::istreambuf_iterator<char>(t)),
		    std::istreambuf_iterator<char>());

    vector<string> tokens = tokenize(str);

    REQUIRE(tokens.size() > 0);
  }

}
