#pragma once
#include <string_view>

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
using header_cb = RETURN (*)(std::string_view,
                             std::string_view); // header field and value
using data_cb = RETURN (*)(std::string_view);

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
  int curr_ptr_;
  std::string_view req_;
  STATE state;
  RETURN ret;

  inline int PARSE_INT();
  RETURN parse_http_version();
  RETURN parse_start_line();
  RETURN parse_headers();
  RETURN parse_body();

public:
  settings *sett;

  explicit Parser(settings *setting)
      : curr_ptr_(0), sett(setting), ret(RETURN::NO_ERROR) {}
  RETURN parser_execute(std::string_view req);
  RETURN parser_resume(std::string_view req);
};
