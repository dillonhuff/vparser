#include "macro_def.h"

#include "algorithm.h"
#include "parse.h"
#include "tokenize.h"

#include <sstream>

using namespace afk;
using namespace std;

namespace vparser {

  vector<string> split_lines(const std::string& text) {

    vector<string> lines;

    if (text == "") { return lines; }

    std::stringstream ss(text);
    std::string to;
    
    while(std::getline(ss,to,'\n')) {
      lines.push_back(to);
    }

    return lines;
  }

  vector<vector<string> >
  parse_comma_list(token_stream& ts) {
    parse_token("(", ts);

    vector<vector<string> > toks;

    vector<string> current_toks;
    int numParens = 1;

    while (true) {
      if ((ts.next() == ",") && (numParens == 1)) {
        toks.push_back(current_toks);
        current_toks = {};
      } else {
        if (ts.next() == "(") {
          numParens++;
        }

        if (ts.next() == ")") {
          numParens--;

          if (numParens == 0) {
            break;
          }
        }

        current_toks.push_back(ts.next());
      }

      ts++;
    }

    parse_token(")", ts);

    toks.push_back(current_toks);

    return toks;
  }

  std::string preprocess_text(const std::string& text,
                              const std::vector<macro_def>& defs) {
    vector<token> tokens = tokenize(text);
    token_stream ts(tokens);

    vector<string> preprocessed_tokens;

    while (ts.chars_left()) {

      auto t = ts.next();
      
      if (t == "`") {
        string macro_name = ts.next(1);
        cout << "macro name = " << macro_name << endl;
        auto mdit = find_if(begin(defs), end(defs), [macro_name](const macro_def& m) {
            return m.get_name() == macro_name;
          });

        assert(mdit != end(defs));

        macro_def md = *mdit;

        cout << md.get_name() << endl;

        ts++;
        ts++;

        if (md.get_arg_names().size() > 0) {
          vector<vector<string>> args =
            parse_comma_list(ts);

          cout << "Args" << endl;
          for (auto& arg : args) {
            cout << "---- A" << endl;
            for (auto& tok : arg) {
              cout << "\t" << tok << endl;
            }
          }

          assert(args.size() == md.get_arg_names().size());

          map<string, vector<string> > arg_expansions;
          for (int i = 0; i < args.size(); i++) {
            arg_expansions.insert({md.get_arg_names()[i], args[i]});
          }

          for (auto& tok : md.get_body()) {
            if (contains_key(tok, arg_expansions)) {
              for (auto& arg_tok : arg_expansions[tok]) {
                preprocessed_tokens.push_back(arg_tok);
              }
            } else {
              preprocessed_tokens.push_back(tok);
            }
          }
        } else {

          for (auto& tok : md.get_body()) {
            preprocessed_tokens.push_back(tok);
          }

        }

      } else {
        preprocessed_tokens.push_back(t);
        ts++;
      }

    }

    string prep_text;
    for (auto& t : preprocessed_tokens) {
      prep_text += " " + t;
    }
    return prep_text;
  }

  preprocessed_verilog
  preprocess_code(const std::string& verilog_text) {
    vector<string> lines = split_lines(verilog_text);

    cout << "LINES" << endl;
    for (auto& ln : lines) {
      cout << ln << endl;
    }
    cout << "END LINES" << endl;

    vector<macro_def> defs;

    string prep_text = "";

    for (auto& line : lines) {
      if (line[0] == '`') {

        vector<token> toks = tokenize(line);

        if ((toks[0].get_text() == "`") && (toks[1].get_text() == "define")) {
          cout << "MACRO: " << line << endl;
          token_stream ts(toks);
          ts++;
          ts++;

          string macro_name = ts.next();

          cout << "Name: " << macro_name << endl;
          ts++;

          vector<vector<string> > subsequent_text =
            parse_comma_list(ts);

          bool is_arg_macro = all_of(subsequent_text, [](const vector<string>& txt) {
              return txt.size() == 1;
            });

          cout << "Is argmacro ? " << is_arg_macro << endl;

          if (is_arg_macro) {
            vector<string> args_names;
            for (auto& text : subsequent_text) {
              args_names.push_back(text[0]);
            }

            vector<string> text;
            while (ts.chars_left()) {
              text.push_back(ts.next());
              ts++;
            }

            cout << "Macro text" << endl;
            for (auto& tx : text) {
              cout << tx << " ";
            }
            cout << endl;

            defs.push_back(macro_def(macro_name, args_names, text));

          } else {
            vector<string> text;
            text.push_back("(");
            for (auto& txt : subsequent_text) {
              concat(text, txt);
            }
            text.push_back(")");

            cout << "Macro body: ";
            for (auto& t : text) {
              cout << t << " ";
            }
            cout << endl;

            defs.push_back(macro_def(macro_name, {}, text));
          }
        } else {

          prep_text += line + "\n";
        }
      } else {

        prep_text += line + "\n";
      }
    }

    string tr_prep_text =
      preprocess_text(prep_text, defs);

    return {defs, tr_prep_text};
  }

}
