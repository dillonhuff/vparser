#include "macro_def.h"

#include "parse.h"
#include "tokenize.h"

#include <iostream>
#include <sstream>

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

  std::string preprocess_text(const std::string& text,
                              const std::vector<macro_def>& defs) {
    vector<string> tokens = tokenize(text);

    vector<string> preprocessed_tokens;

    //for (auto& t : tokens) {
    for (int i = 0; i < tokens.size(); i++) {
      auto t = tokens[i];
      
      if (t == "`") {
        string macro_name = tokens[i + 1];
        cout << "macro name = " << macro_name << endl;
        // macro_def md =
        //   find_by(defs, [macro_name](const macro_def& m) {
        //       return m.get_name() == macro_name;
        //     });

        // cout << md.get_name() << endl;
      } else {
        preprocessed_tokens.push_back(t);
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

    vector<macro_def> defs;

    string prep_text = "";

    for (auto& line : lines) {
      if (line[0] == '`') {
        cout << "MACRO: " << line << endl;

        vector<string> toks = tokenize(line);
        token_stream ts(toks);
        ts++;
        ts++;

        defs.push_back(macro_def(ts.next()));

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

      } else {
        prep_text += line + "\n";
      }
    }

    string tr_prep_text =
      preprocess_text(prep_text, defs);

    return {defs, tr_prep_text};
  }

}
