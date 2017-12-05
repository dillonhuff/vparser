#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "tokenize.h"

using namespace std;

namespace vparser {

  TEST_CASE("Empty string") {
    string test_str = "";
    vector<token> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 0);
    
  }
  
  TEST_CASE("Empty module") {
    string test_str = "module(); endmodule";
    vector<token> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 5);
  }

  TEST_CASE("Logical and") {
    string test_str = "a && b";
    vector<token> toks = tokenize(test_str);

    REQUIRE(toks[1].get_text() == "&&");
  }

  TEST_CASE("Comment line") {
    string test_str = " \n // Hello this is a comment string // asdf\n asdf //";
    vector<token> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 1);
  }

  TEST_CASE("Equals sign") {
    vector<token> tokens = tokenize("assign a = b;");
    REQUIRE(tokens[2].get_text() == "=");
    REQUIRE(tokens.size() == 5);
  }

  TEST_CASE("Equals sign with strange ending") {
    vector<token> tokens = tokenize("assign a = b;\n\n  ");
    REQUIRE(tokens[2].get_text() == "=");
    REQUIRE(tokens.size() == 5);

    REQUIRE(tokens[0].get_pos().lineNo == 1);
    REQUIRE(tokens[0].get_pos().linePos == 1);

    REQUIRE(tokens[1].get_pos().lineNo == 1);
    REQUIRE(tokens[1].get_pos().linePos == 8);

  }
  
  TEST_CASE("Multiple comments in a row") {
    string test_str = " \n // Hello this is a comment string \n// ";
    vector<token> tokens = tokenize(test_str);

    REQUIRE(tokens.size() == 0);
  }
  
  TEST_CASE("Real verilog file") {
    std::ifstream t("./test/samples/memory_core_unq1.v");
    std::string str((std::istreambuf_iterator<char>(t)),
		    std::istreambuf_iterator<char>());

    vector<token> tokens = tokenize(str);

    REQUIRE(tokens.size() > 0);
  }

  TEST_CASE("/**/ style comments") {
    string str = "/* verilator lint_off UNUSED */\ninput [31:0] config_addr;\n/* verilator lint_on UNUSED */\noutput reg [15:0] out;\n";

    auto toks = tokenize(str);

    REQUIRE(toks.size() == 17);
  }

  TEST_CASE("== Comparison") {
    string str = "config_cb == 32'd0";
    auto toks = tokenize(str);

    REQUIRE(toks[1].get_text() == "==");

  }

  TEST_CASE("<= assign") {
    string str = "config_cb <= 32'd0;";
    auto toks = tokenize(str);

    REQUIRE(toks[1].get_text() == "<=");
  }

  TEST_CASE(">= operator") {
    string str = "config_cb >= 32'd0;";
    auto toks = tokenize(str);

    REQUIRE(toks[1].get_text() == ">=");
  }
  
  TEST_CASE("Module with number in name") {
    string str = "module cb_unq1() endmodule";
    auto toks = tokenize(str);

    REQUIRE(toks[1].get_text() == "cb_unq1");
  }

  TEST_CASE("Parse text with string") {
    string str = "xassert(in == out, \"No way man!!!\")";

    cout << "Parse tex tokens" << endl;
    for (auto& tk : tokenize(str)) {
      cout << tk.get_text() << endl;
    }

    REQUIRE(tokenize(str).size() == 8);
  }

  TEST_CASE("String literal parsing preserves quotes") {
    string str = "\"hello\"";
    auto toks = tokenize(str);

    REQUIRE(toks[0].get_text() == "\"hello\"");
  }

}
