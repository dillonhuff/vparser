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
    while (true) {
      ts++;

      if (ts.next() == ")") {
        ts++;
        break;
      } else if (ts.next() == ",") {
        toks.push_back(current_toks);
      } else {
        current_toks.push_back(ts.next());
      }
      
    }

    toks.push_back(current_toks);

    return toks;
  }

  std::string preprocess_text(const std::string& text,
                              const std::vector<macro_def>& defs) {
    vector<string> tokens = tokenize(text);
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

        vector<vector<string>> args =
          parse_comma_list(ts);

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
        preprocessed_tokens.push_back(t);
      }

      ts++;
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
        cout << "MACRO: " << line << endl;

        vector<string> toks = tokenize(line);
        token_stream ts(toks);
        ts++;
        ts++;

        string macro_name = ts.next();

        cout << "Name: " << macro_name << endl;
        ts++;

        vector<string> args_names;

        parse_token("(", ts);

        while (true) {
          args_names.push_back(ts.next());
          ts++;

          if (ts.next() == ")") {
            break;
          }

          parse_token(",", ts);
        }

        parse_token(")", ts);

        cout << "Arg names" << endl;
        for (auto& nm : args_names) {
          cout << nm << endl;
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
        cout << "Appending line " << line << endl;
        prep_text += line + "\n";
      }
    }

    string tr_prep_text =
      preprocess_text(prep_text, defs);

    return {defs, tr_prep_text};
  }

}
