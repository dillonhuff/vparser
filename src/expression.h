#pragma once

#include <string>
#include <vector>

namespace vparser {

  static inline std::string indent(const int indent_level) {
    std::string ind = "";

    for (int i = 0; i < indent_level; i++) {
      ind += "\t";
    }

    return ind;
  }
  
  enum expression_type {
    EXPRESSION_ID,
    EXPRESSION_NUM,
    EXPRESSION_SLICE,
    EXPRESSION_STRING_LITERAL,
    EXPRESSION_UNOP,
    EXPRESSION_BINOP,
    EXPRESSION_TRINOP,
    EXPRESSION_CONCAT
  };

  class expression {
  public:

    virtual expression_type get_type() const = 0;

    virtual std::string to_string() const {
      return "EXPR PLACEHOLDER";
    }
  };

  class concat_expr : public expression {
  public:
    concat_expr(const std::vector<expression*> exprs) {}

    virtual expression_type get_type() const {
      return EXPRESSION_CONCAT;
    }
    
  };

  class unop_expr : public expression {
    
  public:

    unop_expr(const std::string& op_,
              expression* const operand0_) {}
    virtual expression_type get_type() const {
      return EXPRESSION_UNOP;
    }

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

  class trinop_expr : public expression {
    
  public:

    trinop_expr(const std::string& op_,
                expression* const operand0_,
                expression* const operand1_,
                expression* const operand2_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_TRINOP;
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
