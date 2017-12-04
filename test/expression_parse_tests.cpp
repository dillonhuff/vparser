#include "catch.hpp"

#include "parse.h"
#include "tokenize.h"

using namespace std;

namespace vparser {

  TEST_CASE("Parse slice of variable") {
    string str = "config_mem[18:16]";

    // auto toks = tokenize(str);

    // cout << "TOKENS: ";
    // for (auto& t : toks) {
    //   cout << t << " ";
    // }
    // cout << endl;
    // token_stream ts(toks);

    expression* expr = parse_expression(str);

    REQUIRE(expr->get_type() == EXPRESSION_SLICE);
  }
  
  TEST_CASE("Parsing & expression") {
    string str = "clk & (config_mem[2]==1'b1)";
    auto toks = tokenize(str);

    cout << "TOKENS: ";
    for (auto& t : toks) {
      cout << t.get_text() << " ";
    }
    cout << endl;
    token_stream ts(toks);
    expression* p = parse_expression(ts);

    cout << "Remaining text = " << ts.remaining_string() << endl;

    SECTION("No tokens left after parsing") {

      REQUIRE(!ts.chars_left());
    }
  }

  TEST_CASE("Conditional operation") {
    string str = "( config_mem [ 19 ] == 1 ' b1 ) ? chain_in : xdata_in";
    auto toks = tokenize(str);

    cout << "TOKENS: ";
    for (auto& t : toks) {
      cout << t.get_text() << " ";
    }
    cout << endl;
    token_stream ts(toks);
    expression* p = parse_expression(ts);

    cout << "Remaining text = " << ts.remaining_string() << endl;

    SECTION("No tokens left after parsing") {

      REQUIRE(!ts.chars_left());
    }

  }

  
}
