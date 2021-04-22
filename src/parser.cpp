#include "parser.hpp"
#include <cstddef>
#include <iostream>
#include <stdarg.h>

/*
    State will be updated and when pause is called which is called by a return
   we store the state of the pointer and if it is at start line, header or body
   which is technically storing state at every line Although we shoudn't allow
   pausing when Header Values are present To make this possible function calls
   to the next parsing are to be made in recursive manner

    Implementation mostly inspired from the picohttpparser, design from llhttp
   parser

    Think about decrementing req_size -> instead maintaining a start pointer and
   comparing
*/

#define CHECK_EOF()                                                            \
  if ((curr_ptr - start_ptr) > *(req_size))                                    \
    return ERROR::UNEXPECTED_EOF;

#define CHAR_CHECK(ch)                                                         \
  CHECK_EOF();                                                                 \
  if (*curr_ptr++ != ch)                                                       \
    return ERROR::INVALID_SYNTAX;

#define CALLBACK_MAYBE(NAME, ...)                                              \
  if (sett == nullptr || sett->NAME == nullptr)                                \
    ret = 0;                                                                   \
  else                                                                         \
    ret = sett->NAME(__VA_ARGS__);

#define PASS_WHITESPACE()                                                      \
  do {                                                                         \
    CHECK_EOF();                                                               \
  } while (*(++curr_ptr) == ' ')

inline int Parser::PARSE_INT() {
  CHECK_EOF();
  return (*(curr_ptr++) - '0');
}

int Parser::parse_http_version() {
  short int minor_version, major_version, ret;
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

  return ret;
}

int Parser::parse_start_line() {
  std::string method;
  short int ret = 0;
  do {
    CHECK_EOF();
    method += *(curr_ptr++);
  } while (*(curr_ptr) != ' ');

  CALLBACK_MAYBE(handle_method, method);
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  std::string url;
  do {
    CHECK_EOF();
    url += *(curr_ptr++);
  } while (*(curr_ptr) != ' ');

  CALLBACK_MAYBE(handle_url, url);
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  ret = parse_http_version();
  if (ret != 0)
    return ret;

  CHECK_EOF();
  if (*(curr_ptr++) == '\r') {
    CHECK_EOF();
    if (*(curr_ptr++) == '\n') {
      parse_headers();
    } else {
      return ERROR::INVALID_SYNTAX;
    }
  }

  return ERROR::INVALID_SYNTAX;
}

int Parser::parse_headers() {
  // check for \r\n if that's the case end of headers
  if (*(curr_ptr++) == '\r') {
    CHECK_EOF();
    if (*(curr_ptr++) == '\n') {
      // parse_body();
      return 0;
    } else {
      return ERROR::INVALID_SYNTAX;
    }
  }

  std::string header_field, header_value;
  CHECK_EOF();
  while (*(curr_ptr) != ':') {
    header_field += *(curr_ptr++);
    CHECK_EOF();
  }

  PASS_WHITESPACE();
  CHECK_EOF();
  while (*(curr_ptr) != '\r') {
    header_value += *(curr_ptr++);
    CHECK_EOF();
  }

  if (*(curr_ptr++) == '\r') {
    CHECK_EOF();
    if (*(curr_ptr++) == '\n') {
      short int ret = 0;
      CALLBACK_MAYBE(handle_header, header_field, header_value);
      if(ret == 0) parse_headers();
      else return ret;
    } else {
      return ERROR::INVALID_SYNTAX;
    }
  }

  return ERROR::UNEXPECTED;
}

// 0 for success
int Parser::parser_execute(const char *req, size_t *size) {
  curr_ptr = const_cast<char *>(req);
  start_ptr = req;
  req_size = size;
  return parse_start_line();
}