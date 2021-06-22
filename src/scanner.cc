#include <tree_sitter/parser.h>

#include <vector>
#include <string>
#include <wctype.h>
#include <assert.h>
#include <stdio.h>

namespace {

using std::vector;
using std::string;

enum TokenType {
  QUOTED_TEMPLATE_START,
  QUOTED_TEMPLATE_END,
  TEMPLATE_LITERAL_CHUNK,
  TEMPLATE_INTERPOLATION_START,
  TEMPLATE_INTERPOLATION_END,
  HEREDOC_IDENTIFIER,
};

enum ContextType {
  TEMPLATE_INTERPOLATION,
  QUOTED_TEMPLATE,
  HEREDOC_TEMPLATE,
};

struct Context {
  ContextType type;

  // valid if type == HEREDOC_TEMPLATE
  char* identifier;
  size_t identifier_size;
};

struct Scanner {

public:
  // TODO: implement properly
  unsigned serialize(char* buf) {
    return 0;
  }

  // TODO: implement properly
  void deserialize(const char* buf, unsigned n) {
  }

  bool scan(TSLexer* lexer, const bool* valid_symbols) {
    while (iswspace(lexer->lookahead)) {
      skip(lexer);
    }
    if (lexer->lookahead == '\0') {
      return false;
    }
    // manage quoted context
    if (valid_symbols[QUOTED_TEMPLATE_START] && !in_quoted_context() && lexer->lookahead == '"') {
      context_stack.push_back({ .type = QUOTED_TEMPLATE });
      return accept_and_advance(lexer, QUOTED_TEMPLATE_START);
    }
    if (valid_symbols[QUOTED_TEMPLATE_END] && in_quoted_context() && lexer->lookahead == '"') {
      context_stack.pop_back();
      return accept_and_advance(lexer, QUOTED_TEMPLATE_END);
    }

    // manage template interpolations
    if (valid_symbols[TEMPLATE_INTERPOLATION_START] && !in_interpolation_context() && lexer->lookahead == '$') {
      advance(lexer);
      if (lexer->lookahead == '{') {
        context_stack.push_back({ .type = TEMPLATE_INTERPOLATION});
        return accept_and_advance(lexer, TEMPLATE_INTERPOLATION_START);
      }
      if (valid_symbols[TEMPLATE_LITERAL_CHUNK]) {
        // try to scan escape sequence
        if (lexer->lookahead == '$') {
          advance(lexer);
          if (lexer->lookahead == '{') {
            // $${
            return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
          }
        }
        return accept_inplace(lexer, TEMPLATE_LITERAL_CHUNK);
      }
    }
    if (valid_symbols[TEMPLATE_INTERPOLATION_END] && in_interpolation_context() && lexer->lookahead == '}') {
      context_stack.pop_back();
      return accept_and_advance(lexer, TEMPLATE_INTERPOLATION_END);
    }

    // handle template literal chunks

    // handle template literal chunks in quoted contexts
    //
    // they may not contain newlines and may contain escape sequences

    if (valid_symbols[TEMPLATE_LITERAL_CHUNK] && in_quoted_context()) {
      switch (lexer->lookahead) {
        case '\\':
          advance(lexer);
          switch (lexer->lookahead) {
            case '"':
            case 'n':
            case 'r':
            case 't':
            case '\\':
              return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
            case 'u':
              for (int i = 0; i < 4; i++) {
                if (!consume_wxdigit(lexer)) return false;
              }
              return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
            case 'U':
              for (int i = 0; i < 8; i++) {
                if (!consume_wxdigit(lexer)) return false;
              }
              return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
            default:
              return false;
          }
      }
    }

    // handle escaped template interpolations in string literals
    if (valid_symbols[TEMPLATE_LITERAL_CHUNK] && !valid_symbols[TEMPLATE_INTERPOLATION_START] && in_quoted_context()) {
      // try to scan escaped template interpolation
      switch (lexer->lookahead) {
        case '$':
          advance(lexer);
          if (lexer->lookahead == '{') {
            // unescaped template interpolation
            return false;
          }
          if (lexer->lookahead == '$') {
            advance(lexer);
            if (lexer->lookahead == '{') {
              // $${
              return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
            }
          return accept_inplace(lexer, TEMPLATE_LITERAL_CHUNK);
          }
      }
    }

    // handle heredoc identifier
    if (valid_symbols[HEREDOC_IDENTIFIER]) {
      if (lexer->lookahead != 'E') {
        if (valid_symbols[TEMPLATE_LITERAL_CHUNK]) {
          return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
        } else {
          return false;
        }
      }
      advance(lexer);
      if (lexer->lookahead != 'O') {
        if (valid_symbols[TEMPLATE_LITERAL_CHUNK]) {
          return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
        } else {
          return false;
        }
      }
      advance(lexer);
      if (lexer->lookahead != 'F') {
        if (valid_symbols[TEMPLATE_LITERAL_CHUNK]) {
          return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
        } else {
          return false;
        }
      }
      return accept_and_advance(lexer, HEREDOC_IDENTIFIER);
    }

    // handle all other quoted template or string literal characters
    if (valid_symbols[TEMPLATE_LITERAL_CHUNK] && in_template_context()) {
      return accept_and_advance(lexer, TEMPLATE_LITERAL_CHUNK);
    }

    // probably not handled by the external scanner
    return false;
  }

private:
  vector<Context> context_stack;
  vector<string> heredoc_identifier_stack;

  void advance(TSLexer* lexer) {
    lexer->advance(lexer, false);
  }

  void skip(TSLexer* lexer) { lexer->advance(lexer, true); }

  bool accept_inplace(TSLexer* lexer, TokenType token) {
    lexer->result_symbol = token;
    return true;
  }

  bool accept_and_advance(TSLexer* lexer, TokenType token) {
    advance(lexer);
    return accept_inplace(lexer, token);
  }
  bool consume_wxdigit(TSLexer* lexer) {
    advance(lexer);
    return iswxdigit(lexer->lookahead);
  }

  bool in_context_type(ContextType type) {
    if (context_stack.empty()) {
      return false;
    }
    return context_stack.back().type == type;
  }

  bool in_quoted_context() {
    return in_context_type(QUOTED_TEMPLATE);
  }

  bool in_template_context() {
    return in_context_type(QUOTED_TEMPLATE) || in_context_type(HEREDOC_TEMPLATE);
  }

  bool in_interpolation_context() {
    return in_context_type(TEMPLATE_INTERPOLATION);
  }

};

} // namespace

extern "C" {

// tree sitter callbacks
void* tree_sitter_hcl_external_scanner_create() {
  return new Scanner();
}

void tree_sitter_hcl_external_scanner_destroy(void* p) {
  Scanner* scanner = static_cast<Scanner*>(p);
  delete scanner;
}

unsigned tree_sitter_hcl_external_scanner_serialize(void* p, char* b) {
  Scanner* scanner = static_cast<Scanner*>(p);
  return scanner->serialize(b);
}

void tree_sitter_hcl_external_scanner_deserialize(void* p, const char* b, unsigned n) {
  Scanner* scanner = static_cast<Scanner*>(p);
  return scanner->deserialize(b, n);
}

bool tree_sitter_hcl_external_scanner_scan(void* p, TSLexer* lexer, const bool* valid_symbols) {
  Scanner* scanner = static_cast<Scanner*>(p);
  return scanner->scan(lexer, valid_symbols);
}

} // extern "C"
