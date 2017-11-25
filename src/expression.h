#pragma once

#include <string>

namespace vparser {

  enum expression_type {
    EXPRESSION_ID,
    EXPRESSION_NUM,
    EXPRESSION_SLICE,
    EXPRESSION_STRING_LITERAL,
    EXPRESSION_BINOP
  };

  class expression {
  public:

    virtual expression_type get_type() const = 0;
  };

  class binop_expr : public expression {
    
  public:

    binop_expr(const std::string& op_,
               expression* const operand0_,
               expression* const operand1_) {}
    virtual expression_type get_type() const {
      return EXPRESSION_BINOP;
    }

  };
  
  class string_literal_expr : public expression {
    std::string str;

  public:
    string_literal_expr(const std::string& str_) : str(str_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_STRING_LITERAL;
    }

  };

  class num_expr : public expression {
  public:
    virtual expression_type get_type() const {
      return EXPRESSION_NUM;
    }
  };

  class id_expr : public expression {
    std::string name;

  public:

    id_expr(const std::string& name_) : name(name_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_ID;
    }


    std::string get_name() const { return name; }
  };

  class slice_expr : public expression {
  protected:

    expression* arg;
    expression* start;
    expression* end;

  public:

    slice_expr(expression* const arg_,
               expression* const start_,
               expression* const end_) :
      arg(arg_), start(start_), end(end_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_SLICE;
    }

    expression* get_start() const { return start; }
    expression* get_end() const { return end; }

    expression* get_arg() const { return arg; }

  };
  
}
