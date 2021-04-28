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
    return RETURN::UNEXPECTED_EOF;

#define IS_EOF()                                                               \
  if ((curr_ptr - start_ptr) == *(req_size))                                   \
    return RETURN::NO_ERROR;

#define CHAR_CHECK(ch)                                                         \
  CHECK_EOF();                                                                 \
  if (*curr_ptr++ != ch)                                                       \
    return RETURN::INVALID_SYNTAX;

#define CALLBACK_MAYBE(NAME, ...)                                              \
  if (sett == nullptr || sett->NAME == nullptr)                                \
    ret = RETURN::NO_ERROR;                                                    \
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

RETURN Parser::parse_http_version() {
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

  return ret;
}

RETURN Parser::parse_start_line() {
  std::string method;
  do {
    CHECK_EOF();
    method += *(curr_ptr++);
  } while (*(curr_ptr) != ' ');

  CALLBACK_MAYBE(handle_method, method);
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  std::string url;
  do {
    CHECK_EOF();
    url += *(curr_ptr++);
  } while (*(curr_ptr) != ' ');

  CALLBACK_MAYBE(handle_url, url);
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  ret = parse_http_version();
  if (ret != 0 && ret != RETURN::PAUSE)
    return ret;

  state = STATE::BODY;
  CHECK_EOF();
  if (*(curr_ptr++) == '\r') {
    CHECK_EOF();
    if (*(curr_ptr++) == '\n') {
      if (ret == RETURN::PAUSE)
        return ret;
      return parse_headers();
    } else {
      return RETURN::INVALID_SYNTAX;
    }
  }

  return RETURN::INVALID_SYNTAX;
}

RETURN Parser::parse_headers() {
  // check for \r\n if that's the case end of headers
  if (*(curr_ptr) == '\r') {
    CHECK_EOF();
    if (*(++curr_ptr) == '\n') {
      IS_EOF();
      ++curr_ptr;
      return parse_body();
    } else {
      return RETURN::INVALID_SYNTAX;
    }
  }

  std::string header_field, header_value;
  IS_EOF();
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
      CALLBACK_MAYBE(handle_header, header_field, header_value);
      if (ret != 0)
        return ret;
      else
        return parse_headers();
    } else {
      return RETURN::INVALID_SYNTAX;
    }
  }

  return RETURN::UNEXPECTED;
}

RETURN Parser::parse_body() {
  std::string body;
  do {
    body += *(curr_ptr++);
  } while ((curr_ptr - start_ptr) != *(req_size));

  CALLBACK_MAYBE(handle_body, body);
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;

  return ret;
}

// RETURN::NO_ERROR for success
RETURN Parser::parser_execute(const char *req, size_t *size) {
  curr_ptr = const_cast<char *>(req);
  start_ptr = req;
  req_size = size;
  return parse_start_line();
}

// Pausing possible only after parsing start line and headers by calling
// RETURN::PAUSE from callbacks
RETURN Parser::parser_resume() {
  if (ret != RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;

  if (state == STATE::HEADER) {
    return parse_start_line();
  } else if (state == STATE::BODY) {
    return parse_body();
  }

  return INVALID_PAUSE;
}