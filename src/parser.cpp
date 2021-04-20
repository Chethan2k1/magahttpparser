#include "parser.hpp"
#include <cstddef>

/*
    State will be updated and when pause is called which is called by a return
   we store the state of the pointer and if it is at start line, header or body
   which is technically storing state at every line Although we shoudn't allow
   pausing when Header Values are present To make this possible function calls
   to the next parsing are to be made in recursive manner

    Implementation mostly inspired from the picohttpparser, design from llhttp
   parser

    Think about decrementing req_size
*/

#define CHECK_EOF()                                                            \
  if (*req_size <= 0)                                                          \
    return ERROR::UNEXPECTED_EOF;

#define CHAR_CHECK(ch)                                                         \
  CHECK_EOF();                                                                 \
  else if (*curr_ptr++ != ch) return ERROR::INVALID_SYNTAX;

#define CALLBACK_MAYBE(NAME, ...)                                              \
  do {                                                                         \
    if (sett == nullptr || sett->NAME == nullptr)                              \
      return 0;                                                              \\
    return settings->NAME(__VA_ARGS__);                                      \
  } while (0)

#define PARSE_INT()                                                            \
  CHECK_EOF();                                                                 \
  do {                                                                         \
    return (*(curr_ptr++) - '0');                                              \
  } while (0)

int Parser::parse_http_version() {
  short int minor_version, major_version;
  CHAR_CHECK('H');
  CHAR_CHECK('T');
  CHAR_CHECK('T');
  CHAR_CHECK('P');
  CHAR_CHECK('/');
  CHECK_EOF();
  major_version = PARSE_INT();
  CHAR_CHECK('.');
  minor_version = PARSE_INT();
  CALLBACK_MAYBE(handle_version, major_version, minor_version);
}

int Parser::parse_start_line() {}

// 0 for success
int Parser::parser_execute(const char *req, size_t *size) {
  curr_ptr = const_cast<char *>(req);
  req_size = size;
  return parse_start_line();
}