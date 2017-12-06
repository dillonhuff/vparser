#pragma once

#include <cassert>
#include <string>
#include <vector>

namespace vparser {

  static inline std::string parens(const std::string& str) {
    return "(" + str + ")";
  }

  static inline std::string indent(const int indent_level) {
    std::string ind = "";

    for (int i = 0; i < indent_level; i++) {
      ind += "  ";
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
    EXPRESSION_CONCAT,
    EXPRESSION_FLOAT
  };

  class expression {
  public:

    virtual expression_type get_type() const = 0;

    virtual std::string to_string() const {
      return "EXPR PLACEHOLDER";
    }
  };

  class float_expr : public expression {
    double val;

  public:

    float_expr(const double val_) : val(val_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_FLOAT;
    }

    virtual std::string to_string() const {
      return std::to_string(val);
    }
  };
  
  class concat_expr : public expression {
    std::vector<expression*> exprs;

  public:
    concat_expr(const std::vector<expression*> exprs_) : exprs(exprs_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_CONCAT;
    }

    virtual std::string to_string() const {
      std::string str = "{ ";

      for (unsigned i = 0; i < exprs.size(); i++) {
        str += exprs[i]->to_string();
        if (i < exprs.size() - 1) {
          str += ", ";
        }
      }

      str += " }";
      return str;
    }
  };

  class unop_expr : public expression {

    std::string op;
    expression* operand0;
    
  public:

    unop_expr(const std::string& op_,
              expression* const operand0_) : op(op_), operand0(operand0_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_UNOP;
    }

    virtual std::string to_string() const {
      return parens(op + " " + operand0->to_string());
    }

  };
  
  class binop_expr : public expression {

    std::string op;
    expression* operand0;
    expression* operand1;
    
  public:

    binop_expr(const std::string& op_,
               expression* const operand0_,
               expression* const operand1_) :
      op(op_), operand0(operand0_), operand1(operand1_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_BINOP;
    }

    virtual std::string to_string() const {
      return parens(operand0->to_string() + " " + op + " " + operand1->to_string());
    }

  };

  class trinop_expr : public expression {

    std::string op;
    expression* operand0;
    expression* operand1;
    expression* operand2;
    
  public:

    trinop_expr(const std::string& op_,
                expression* const operand0_,
                expression* const operand1_,
                expression* const operand2_) :
      op(op_), operand0(operand0_), operand1(operand1_), operand2(operand2_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_TRINOP;
    }

    virtual std::string to_string() const {
      assert(op == "?");

      return parens(operand0->to_string() + " ? " + operand1->to_string() + " : " + operand2->to_string());
    }
    
  };
  
  class string_literal_expr : public expression {
    std::string str;

  public:
    string_literal_expr(const std::string& str_) : str(str_) {}

    virtual std::string to_string() const {
      return str;
    }
    
    virtual expression_type get_type() const {
      return EXPRESSION_STRING_LITERAL;
    }

  };

  class num_expr : public expression {
    int size;
    bool is_signed;
    char radix;
    std::string value;
    
    
  public:

    num_expr(const std::string& value_) :
      size(32), is_signed(false), radix('d'), value(value_) {}

    num_expr(const int size_,
             const char radix_,
             const std::string& value_) :
      size(size_), is_signed(false), radix(radix_), value(value_) {}

    std::string to_string() const {
      assert(!is_signed);

      return std::to_string(size) + "'" + std::string(1, radix) + value;
    }
    
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

    virtual std::string to_string() const {
      return name;
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

    virtual std::string to_string() const {
      return parens(get_arg()->to_string() + "[ " + get_start()->to_string() + " : " + get_end()->to_string() + " ]");
    }
    
  };
  
}
