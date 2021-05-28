#include "mgparser.hpp"
#include <gtest/gtest.h>
#include <string_view>

#define HANDLE_METHOD(method)                                                  \
  do {                                                                         \
    sett.handle_method = [](std::string_view method_) {                        \
      EXPECT_EQ(method_, method);                                              \
      return mg_return_t::SUCCESS;                                             \
    };                                                                         \
  } while (false);

#define HANDLE_VERSION()                                                       \
  do {                                                                         \
    sett.handle_version = [](const int &mjv, const int &miv) {                 \
      EXPECT_EQ(mjv, 1);                                                       \
      EXPECT_EQ(miv, 1);                                                       \
      return mg_return_t::SUCCESS;                                             \
    };                                                                         \
  } while (false);

#define HANDLE_URL()                                                           \
  do {                                                                         \
    sett.handle_url = [](std::string_view url) {                               \
      EXPECT_EQ(url, "http://www.example.com/123");                            \
      return mg_return_t::SUCCESS;                                             \
    };                                                                         \
  } while (false);

#define HANDLE_HEADER()                                                        \
  do {                                                                         \
    sett.handle_header = [](std::string_view field, std::string_view val) {    \
      if (field == "Host") {                                                   \
        EXPECT_EQ(val, "www.example.com");                                     \
        return mg_return_t::SUCCESS;                                           \
      } else if (field == "User-Agent") {                                      \
        EXPECT_EQ(val, "RandomBrowser1.1");                                    \
        return mg_return_t::SUCCESS;                                           \
      } else if (field == "Last-Modified") {                                   \
        EXPECT_EQ(val, "Fri, 31 Dec 1999 23:59:59 GMT");                       \
        return mg_return_t::SUCCESS;                                           \
      }                                                                        \
                                                                               \
      return mg_return_t::ERROR;                                               \
    };                                                                         \
  } while (false);

#define HANDLE_BODY(body)                                                      \
  do {                                                                         \
    sett.handle_body = [](std::string_view body_) {                            \
      EXPECT_EQ(body_, body);                                                  \
      return mg_return_t::SUCCESS;                                             \
    };                                                                         \
  } while (false);

TEST(Request, ResponseWithoutBody) {
  mg_settings_t<> sett;
  std::string req = "GET http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Last-Modified: Fri, 31 Dec 1999 23:59:59 GMT\r\n"
                    "User-Agent: RandomBrowser1.1\r\n";

  HANDLE_METHOD("GET");
  HANDLE_VERSION();
  HANDLE_URL();
  HANDLE_HEADER();

  mg_parser_t<> parser;
  parser.mg_settings_init(&sett);
  int ret = parser.mg_parser_execute(req);
  EXPECT_EQ(ret, 0);
}

TEST(Request, ResponseWithBody) {
  mg_settings_t<> sett;
  std::string req = "POST http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Last-Modified: Fri, 31 Dec 1999 23:59:59 GMT\r\n"
                    "User-Agent: RandomBrowser1.1\r\n\r\n"
                    "Hey there! Sup?";

  HANDLE_METHOD("POST");
  HANDLE_VERSION();
  HANDLE_URL();
  HANDLE_HEADER();
  HANDLE_BODY("Hey there! Sup?");

  mg_parser_t<> parser;
  parser.mg_settings_init(&sett);
  int ret = parser.mg_parser_execute(req);
  EXPECT_EQ(ret, 0);
}

TEST(Request, ResponseWithPause) {
  mg_settings_t<> sett;
  std::string req = "POST http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Content-Length: 15\r\n"
                    "User-Agent: RandomBrowser1.1\r\n\r\n";
  std::string req_only_body = "Hey there! Sup?";

  HANDLE_METHOD("POST");
  HANDLE_VERSION();
  HANDLE_URL();
  sett.handle_header = [](std::string_view field, std::string_view val) {
    if (field == "Host") {
      EXPECT_EQ(val, "www.example.com");
      return mg_return_t::SUCCESS;
    } else if (field == "User-Agent") {
      EXPECT_EQ(val, "RandomBrowser1.1");
      return mg_return_t::SUCCESS;
    } else if (field == "Content-Length") {
      EXPECT_EQ(val, "15");
      return mg_return_t::PAUSE;
    }

    return mg_return_t::ERROR;
  };
  HANDLE_BODY("Hey there! Sup?");

  mg_parser_t<> parser;
  parser.mg_settings_init(&sett);
  int ret = parser.mg_parser_execute(req);
  EXPECT_EQ(ret, mg_return_t::PAUSE);
  req += req_only_body;
  ret = parser.mg_parser_resume(req);
  EXPECT_EQ(ret, 0);
}

TEST(Request, CallbackWithAdditionalArguments) {
  mg_settings_t<int &> sett;
  std::string req = "GET http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Last-Modified: Fri, 31 Dec 1999 23:59:59 GMT\r\n"
                    "User-Agent: RandomBrowser1.1\r\n";
  int count = 1;

  sett.handle_method = [](std::string_view method, int &count) -> mg_return_t {
    EXPECT_EQ(method, "GET");
    EXPECT_EQ(count++, 1);
    return mg_return_t::SUCCESS;
  };

  sett.handle_version = [](int mjv, int miv, int &count) -> mg_return_t {
    EXPECT_EQ(mjv, 1);
    EXPECT_EQ(miv, 1);
    EXPECT_EQ(count++, 2);
    return mg_return_t::SUCCESS;
  };

  sett.handle_url = [](std::string_view url, int &count) -> mg_return_t {
    EXPECT_EQ(url, "http://www.example.com/123");
    return mg_return_t::SUCCESS;
  };

  sett.handle_header = [](std::string_view field, std::string_view val,
                          int &count) -> mg_return_t {
    if (field == "Host") {
      EXPECT_EQ(val, "www.example.com");
      EXPECT_EQ(count++, 4);
      return mg_return_t::SUCCESS;
    } else if (field == "User-Agent") {
      EXPECT_EQ(val, "RandomBrowser1.1");
      EXPECT_EQ(count++, 6);
      return mg_return_t::SUCCESS;
    } else if (field == "Last-Modified") {
      EXPECT_EQ(val, "Fri, 31 Dec 1999 23:59:59 GMT");
      EXPECT_EQ(count++, 5);
      return mg_return_t::SUCCESS;
    }

    return mg_return_t::ERROR;
  };

  mg_parser_t<int &> parser;
  parser.mg_settings_init(&sett);
  int ret = parser.mg_parser_execute(req, count);
  EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}