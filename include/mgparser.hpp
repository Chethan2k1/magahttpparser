#pragma once
#include <string_view>

enum mg_return_t {
  SUCCESS,
  PAUSE,
  INVALID_PAUSE,
  UNKNOWN_METHOD,
  INVALID_SYNTAX,
  INVALID_VERSION,
  UNEXPECTED_EOF,
  UNEXPECTED,
  ERROR
};

using version_cb = mg_return_t (*)(const int &,
                                   const int &); // major_version,minor_version
using header_cb = mg_return_t (*)(std::string_view,
                                  std::string_view); // header field and value
using data_cb = mg_return_t (*)(std::string_view);

struct mg_settings_t {
  data_cb handle_method;
  version_cb handle_version;
  data_cb handle_url;
  header_cb handle_header;
  data_cb handle_body;
};

enum STATE { HEADER, BODY };

struct mg_parser_t {
private:
  STATE state;
  int curr_ptr_;
  std::string_view req_;
  mg_return_t ret;

  inline int PARSE_INT();
  mg_return_t parse_http_version();
  mg_return_t parse_start_line();
  mg_return_t parse_headers();
  mg_return_t parse_body();

public:
  mg_settings_t *sett;

  explicit mg_parser_t() : curr_ptr_(0), ret(mg_return_t::SUCCESS) {}
  void mg_settings_init(mg_settings_t *);
  mg_return_t mg_parser_execute(std::string_view);
  mg_return_t mg_parser_resume(std::string_view);
};
