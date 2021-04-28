#pragma once
#include <string>

enum RETURN {
  NO_ERROR,
  PAUSE,
  INVALID_PAUSE,
  UNKNOWN_METHOD,
  INVALID_SYNTAX,
  INVALID_VERSION,
  UNEXPECTED_EOF,
  UNEXPECTED,
  ERROR
};

using version_cb = RETURN (*)(const int &,
                              const int &); // major_version,minor_version
using header_cb = RETURN (*)(const std::string &,
                             const std::string &); // header field and value
using data_cb = RETURN (*)(const std::string &);

struct settings {
  data_cb handle_method;
  version_cb handle_version;
  data_cb handle_url;
  header_cb handle_header;
  data_cb handle_body;
};

enum STATE { HEADER, BODY };

struct Parser {
private:
  char *curr_ptr;
  const char *start_ptr;
  size_t *req_size;
  STATE state;
  RETURN ret;

  inline int PARSE_INT();
  RETURN parse_http_version();
  RETURN parse_start_line();
  RETURN parse_headers();
  RETURN parse_body();

public:
  settings *sett;

  explicit Parser(settings *setting) : sett(setting), ret(RETURN::NO_ERROR) {}
  RETURN parser_execute(const char *, size_t *);
  RETURN parser_resume();
};
