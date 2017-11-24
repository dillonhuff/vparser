#include "catch.hpp"

#include "macro_def.h"
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

  TEST_CASE("Parse slice of variable") {
    string str = "config_mem[18:16]";
    expression* expr = parse_expression(str);

    REQUIRE(expr->get_type() == EXPRESSION_SLICE);
  }

  TEST_CASE("Module instantiation statement") {
    string str = "mem_unq1  mem_inst0\n(\n.data_out(mem_data_out0),\n.data_in(mem_data_in0),\n.clk(gclk),\n.cen(int_cen),\n.wen(int_wen),\n.addr(mem_addr[8:0])\n);";

    statement* stmt = parse_statement(str);

    REQUIRE(stmt->get_type() == STATEMENT_MODULE_INSTANTIATION);

    module_instantiation_stmt* mst =
      static_cast<module_instantiation_stmt*>(stmt);

    REQUIRE(mst->get_module_type() == "mem_unq1");
    REQUIRE(mst->get_name() == "mem_inst0");
    REQUIRE(mst->get_port_assignments().size() == 6);

    auto port_decl5 = mst->get_port_assignments()[5];

    REQUIRE(port_decl5.first == "addr");

    expression* expr = port_decl5.second;

    REQUIRE(expr->get_type() == EXPRESSION_SLICE);
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

  TEST_CASE("Preprocessing and then parsing") {
    string str = "`define xassert(condition, message) if(condition) begin $display(message); $finish(1); end\nmodule test_mod();\n `xassert(in == out, \"No way man!!!\")\nendmodule";

    preprocessed_verilog prep =
      preprocess_code(str);

    vector<macro_def> macro_defs = prep.defs;

    REQUIRE(macro_defs.size() == 1);

    cout << "prep text = " << endl;
    cout << prep.text << endl;
    
    string prep_str = " module test_mod ( ) ; if ( in == out ) begin $ display ( \"No way man!!!\" ) ; $ finish ( 1 ) ; end endmodule";

    REQUIRE(prep.text == prep_str);
    
    verilog_module vm = parse_module(prep.text);
    
  }

  TEST_CASE("Preprocess no argument macro") {
    string str = "`define MV_TO_RAM (phase==1'b0 && (input_count > 2'd1 || (input_count==2'd1 && wen)))\nMV_TO_RAM";

    preprocessed_verilog prep =
      preprocess_code(str);

    vector<macro_def> macro_defs = prep.defs;

    REQUIRE(macro_defs.size() == 1);

    cout << "prep text = " << endl;
    cout << prep.text << endl;
    
    string prep_str = " module test_mod ( ) ; if ( in == out ) begin $ display ( \"No way man!!!\" ) ; $ finish ( 1 ) ; end endmodule";

    REQUIRE(prep.text == prep_str);
    
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

    SECTION("Non blocking assignment") {
      string str = "config_cb <= 32'd0;";
      statement* stmt = parse_statement(str);

      REQUIRE(stmt->get_type() == STATEMENT_NON_BLOCKING_ASSIGN);
    }

    SECTION("Parse a define macro") {
      string str = "`define xassert(condition, message) if(condition) begin $display(message); $finish(1); end";
    }

    SECTION("17th statement is an always block containing one if-else statement") {
      REQUIRE(vm.get_statements()[17]->get_type() == STATEMENT_ALWAYS);

      always_stmt* astmt =
        static_cast<always_stmt*>(vm.get_statements()[17]);

      begin_stmt* bstmt =
        static_cast<begin_stmt*>(astmt->get_statement());

      REQUIRE(bstmt->get_statements().size() == 1);

      auto stmts = bstmt->get_statements();
      statement* inner_stmt = stmts[0];

      REQUIRE(inner_stmt->get_type() == STATEMENT_IF);

      // 18th statement

      REQUIRE(vm.get_statements()[18]->get_type() == STATEMENT_ALWAYS);

      always_stmt* always_case =
        static_cast<always_stmt*>(vm.get_statements()[18]);
      auto inner_stmts =
        static_cast<begin_stmt*>(always_case->get_statement())->get_statements();

      REQUIRE(inner_stmts.size() == 1);

      auto inner_always_stmt = inner_stmts[0];

      REQUIRE(inner_always_stmt->get_type() == STATEMENT_CASE);

      case_stmt* inner_case =
        static_cast<case_stmt*>(inner_always_stmt);

      REQUIRE(inner_case->get_cases().size() == 10);

      statement* case_0 = (inner_case->get_cases()[0]).second;

      REQUIRE(case_0->get_type() == STATEMENT_BLOCKING_ASSIGN);
    }

  }

  TEST_CASE("Reading a larger real file with multiple statements") {
    std::ifstream t("./test/samples/memory_core_unq1.v");
    std::string str((std::istreambuf_iterator<char>(t)),
		    std::istreambuf_iterator<char>());

    preprocessed_verilog prep =
      preprocess_code(str);

    vector<macro_def> macro_defs = prep.defs;

    REQUIRE(macro_defs.size() == 6);

    cout << "prep text = " << endl;
    cout << prep.text << endl;
    
    verilog_module vm = parse_module(prep.text);
  }
}
