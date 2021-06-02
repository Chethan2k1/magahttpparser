#pragma once
#include <functional>
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

template <typename... Args> struct mg_settings_t {
  std::function<mg_return_t(std::string_view, Args...)> handle_method;
  std::function<mg_return_t(int, int, Args...)>
      handle_version; // major_version,minor_version
  std::function<mg_return_t(std::string_view, Args...)> handle_url;
  std::function<mg_return_t(std::string_view, std::string_view, Args...)>
      handle_header; // header field and value
  std::function<mg_return_t(std::string_view, Args...)> handle_body;
};

enum STATE { HEADER, BODY };

template <typename... Args> struct mg_parser_t {
private:
  STATE state;
  int curr_ptr_;
  std::string_view req_;
  mg_return_t ret;

  inline int PARSE_INT();
  mg_return_t parse_http_version(Args &&...);
  mg_return_t parse_start_line(Args &&...);
  mg_return_t parse_headers(Args &&...);
  mg_return_t parse_body(Args &&...);

public:
  mg_settings_t<Args...> *sett;

  explicit mg_parser_t() : curr_ptr_(0), ret(mg_return_t::SUCCESS) {}
  void mg_settings_init(mg_settings_t<Args...> *);
  mg_return_t mg_parser_execute(std::string_view, Args &&...);
  mg_return_t mg_parser_resume(std::string_view, Args &&...);
};

#include "mgparser.cpp"
