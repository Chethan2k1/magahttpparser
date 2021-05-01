#include "parser.hpp"
#include <cstddef>
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
  if (curr_ptr_ >= req_.size())                                                \
    return RETURN::UNEXPECTED_EOF;

#define IS_EOF()                                                               \
  if (curr_ptr_ == (req_.size()))                                              \
    return RETURN::NO_ERROR;

#define CHAR_CHECK(ch)                                                         \
  CHECK_EOF();                                                                 \
  if (req_[curr_ptr_++] != ch)                                                 \
    return RETURN::INVALID_SYNTAX;

#define CALLBACK_MAYBE(NAME, ...)                                              \
  if (sett == nullptr || sett->NAME == nullptr)                                \
    ret = RETURN::NO_ERROR;                                                    \
  else                                                                         \
    ret = sett->NAME(__VA_ARGS__);

#define PASS_WHITESPACE()                                                      \
  do {                                                                         \
    CHECK_EOF();                                                               \
  } while (req_[++curr_ptr_] == ' ')

inline int Parser::PARSE_INT() {
  CHECK_EOF();
  return (req_[curr_ptr_++] - '0');
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
  int start_ptr = curr_ptr_;
  do {
    CHECK_EOF();
    curr_ptr_++;
  } while (req_[curr_ptr_] != ' ');

  CALLBACK_MAYBE(handle_method, req_.substr(start_ptr, curr_ptr_ - start_ptr));
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  start_ptr = curr_ptr_;
  do {
    CHECK_EOF();
    curr_ptr_++;
  } while (req_[curr_ptr_] != ' ');

  CALLBACK_MAYBE(handle_url, req_.substr(start_ptr, curr_ptr_ - start_ptr));
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;
  if (ret != 0)
    return ret;

  PASS_WHITESPACE();
  ret = parse_http_version();
  if (ret != 0 && ret != RETURN::PAUSE)
    return ret;

  state = STATE::HEADER;
  CHECK_EOF();
  if (req_[curr_ptr_++] == '\r') {
    CHECK_EOF();
    if (req_[curr_ptr_++] == '\n') {
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
  if (req_[curr_ptr_] == '\r') {
    CHECK_EOF();
    if (req_[++curr_ptr_] == '\n') {
      IS_EOF();
      ++curr_ptr_;
      return parse_body();
    } else {
      return RETURN::INVALID_SYNTAX;
    }
  }

  IS_EOF();
  int start_ptr = curr_ptr_;
  while (req_[curr_ptr_] != ':') {
    curr_ptr_++;
    CHECK_EOF();
  }

  std::string_view field = req_.substr(start_ptr, curr_ptr_ - start_ptr);

  PASS_WHITESPACE();
  start_ptr = curr_ptr_;
  CHECK_EOF();
  while (req_[curr_ptr_] != '\r') {
    curr_ptr_++;
    CHECK_EOF();
  }

  std::string_view value = req_.substr(start_ptr, curr_ptr_ - start_ptr);
  if (req_[curr_ptr_++] == '\r') {
    CHECK_EOF();
    if (req_[curr_ptr_++] == '\n') {
      CALLBACK_MAYBE(handle_header, field, value);
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
  int start_ptr = curr_ptr_;
  do {
    curr_ptr_++;
  } while (curr_ptr_ < req_.size());

  CALLBACK_MAYBE(handle_body, req_.substr(start_ptr, curr_ptr_ - start_ptr));
  if (ret == RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;

  return ret;
}

// RETURN::NO_ERROR for success
RETURN Parser::parser_execute(std::string_view req) {
  req_ = req;
  return parse_start_line();
}

// Pausing possible only after parsing start line and headers by calling
// RETURN::PAUSE from callbacks
RETURN Parser::parser_resume(std::string_view req) {
  req_ = req;
  if (ret != RETURN::PAUSE)
    return RETURN::INVALID_PAUSE;

  if (state == STATE::HEADER) {
    return parse_headers();
  } else if (state == STATE::BODY) {
    return parse_body();
  }

  return INVALID_PAUSE;
}