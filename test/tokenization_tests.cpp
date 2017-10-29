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

  TEST_CASE("/**/ style comments") {
    string str = "/* verilator lint_off UNUSED */\ninput [31:0] config_addr;\n/* verilator lint_on UNUSED */\noutput reg [15:0] out;\n";

    auto toks = tokenize(str);

    REQUIRE(toks.size() == 17);
  }

  TEST_CASE("<= assign") {
    string str = "config_cb <= 32'd0;";
    auto toks = tokenize(str);

    REQUIRE(toks[1] == "<=");
  }

  TEST_CASE("Module with number in name") {
    string str = "module cb_unq1() endmodule";
    auto toks = tokenize(str);

    REQUIRE(toks[1] == "cb_unq1");
  }

  TEST_CASE("Parse text with string") {
    string str = "xassert(in == out, \"No way man!!!\")";

    REQUIRE(tokenize(str).size() == 8);
  }

}
