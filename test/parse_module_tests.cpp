#include "catch.hpp"

#include "parse.h"
#include "tokenize.h"

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

  TEST_CASE("Assignment statement") {
    string str = "assign gclk = clk;";
    statement* stmt = parse_statement(str);

    SECTION("Parsed statement is assignment") {
      REQUIRE(stmt->get_type() == STATEMENT_ASSIGN);

      assign_stmt* astmt =
        static_cast<assign_stmt*>(stmt);

      SECTION("LHS is an identifier 'gclk'") {
        expression* expr = astmt->get_lhs();

        REQUIRE(expr->get_type() == EXPRESSION_ID);

        id_expr* eid = static_cast<id_expr*>(expr);

        REQUIRE(eid->get_name() == "gclk");
      }

      SECTION("RHS is an identifier 'clk'") {
        expression* expr = astmt->get_rhs();

        REQUIRE(expr->get_type() == EXPRESSION_ID);

        id_expr* eid = static_cast<id_expr*>(expr);

        REQUIRE(eid->get_name() == "clk");
      }

    }
  }

  TEST_CASE("Reading a real file with multiple statements") {
    std::ifstream t("./test/samples/cb_unq1.v");
    std::string str((std::istreambuf_iterator<char>(t)),
		    std::istreambuf_iterator<char>());

    verilog_module vm = parse_module(str);

    SECTION("16 ports") {
      REQUIRE(vm.get_port_names().size() == 16);
    }

    SECTION("19 statements") {
      REQUIRE(vm.get_statements().size() == 19);
    }

    SECTION("First statement is a declaration") {
      REQUIRE(vm.get_statements()[0]->get_type() == STATEMENT_DECL);
    }

    SECTION("17th statement is an always block containing one if-else statement") {
      REQUIRE(vm.get_statements()[17]->get_type() == STATEMENT_ALWAYS);

      always_stmt* astmt =
        static_cast<always_stmt*>(vm.get_statements()[17]);

      REQUIRE(astmt->get_statement_block().size() == 1);

      auto stmts = astmt->get_statement_block();
      statement* inner_stmt = stmts[0];

      REQUIRE(inner_stmt->get_type() == STATEMENT_IF);

      REQUIRE(vm.get_statements()[18]->get_type() == STATEMENT_ALWAYS);

      always_stmt* always_case =
        static_cast<always_stmt*>(vm.get_statements()[18]);
      auto inner_stmts = always_case->get_statement_block();

      REQUIRE(inner_stmts.size() == 1);

      auto inner_always_stmt = inner_stmts[0];

      REQUIRE(inner_always_stmt->get_type() == STATEMENT_CASE);

      case_stmt* inner_case =
        static_cast<case_stmt*>(inner_always_stmt);

      REQUIRE(inner_case->get_cases().size() == 10);

    }

  }

}
