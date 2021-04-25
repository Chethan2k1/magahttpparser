#pragma once
#include <string>

enum RETURN {
  NO_ERROR,
  PAUSE,
  UNKNOWN_METHOD,
  INVALID_SYNTAX,
  INVALID_VERSION,
  UNEXPECTED_EOF,
  UNEXPECTED
};

using version_cb = int (*)(const int &,
                           const int &); // major_version,minor_version
using header_cb = int (*)(const std::string &,
                          const std::string &); // header field and value
using data_cb = int (*)(const std::string &);

struct settings {
  data_cb handle_method;
  version_cb handle_version;
  data_cb handle_url;
  header_cb handle_header;
  data_cb handle_body;
};

enum State { STARTLINE, HEADER, BODY };

struct Parser {
private:
  short int ret;
  char *curr_ptr;
  const char *start_ptr;
  size_t *req_size;
  State state;

  inline int PARSE_INT();
  int parse_http_version();
  int parse_start_line();
  int parse_headers();
  int parse_body();

public:
  settings *sett;

  explicit Parser(settings *setting) : sett(setting), ret(0) {}
  int parser_execute(const char *, size_t *);
};
