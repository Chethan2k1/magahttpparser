#include <gtest/gtest.h>
#include <parser.hpp>
#include <string>

#define HANDLE_METHOD(method)                                                  \
  do {                                                                         \
    sett.handle_method = [](const std::string &method_) {                      \
      EXPECT_EQ(method_, method);                                              \
      return RETURN::NO_ERROR;                                                 \
    };                                                                         \
  } while (false);

#define HANDLE_VERSION()                                                       \
  do {                                                                         \
    sett.handle_version = [](const int &mjv, const int &miv) {                 \
      EXPECT_EQ(mjv, 1);                                                       \
      EXPECT_EQ(miv, 1);                                                       \
      return RETURN::NO_ERROR;                                                 \
    };                                                                         \
  } while (false);

#define HANDLE_URL()                                                           \
  do {                                                                         \
    sett.handle_url = [](const std::string &url) {                             \
      EXPECT_EQ(url, "http://www.example.com/123");                            \
      return RETURN::NO_ERROR;                                                 \
    };                                                                         \
  } while (false);

#define HANDLE_HEADER()                                                        \
  do {                                                                         \
    sett.handle_header = [](const std::string &field,                          \
                            const std::string &val) {                          \
      if (field == "Host") {                                                   \
        EXPECT_EQ(val, "www.example.com");                                     \
        return RETURN::NO_ERROR;                                               \
      } else if (field == "User-Agent") {                                      \
        EXPECT_EQ(val, "RandomBrowser1.1");                                    \
        return RETURN::NO_ERROR;                                               \
      } else if (field == "Last-Modified") {                                   \
        EXPECT_EQ(val, "Fri, 31 Dec 1999 23:59:59 GMT");                       \
        return RETURN::NO_ERROR;                                               \
      }                                                                        \
                                                                               \
      return RETURN::ERROR;                                                    \
    };                                                                         \
  } while (false);

#define HANDLE_BODY(body)                                                      \
  do {                                                                         \
    sett.handle_body = [](const std::string &body_) {                          \
      EXPECT_EQ(body_, body);                                                  \
      return RETURN::NO_ERROR;                                                 \
    };                                                                         \
  } while (false);

TEST(Request, ResponseWithoutBody) {
  settings sett;
  std::string req = "GET http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Last-Modified: Fri, 31 Dec 1999 23:59:59 GMT\r\n"
                    "User-Agent: RandomBrowser1.1\r\n";

  HANDLE_METHOD("GET");
  HANDLE_VERSION();
  HANDLE_URL();
  HANDLE_HEADER();

  Parser parser(&sett);
  size_t req_size = req.size();
  int ret = parser.parser_execute(req.data(), &req_size);
  EXPECT_EQ(ret, 0);
}

TEST(Request, ResponseWithBody) {
  settings sett;
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

  Parser parser(&sett);
  size_t req_size = req.size();
  int ret = parser.parser_execute(req.data(), &req_size);
  EXPECT_EQ(ret, 0);
}

TEST(Request, ResponseWithPause) {
  settings sett;
  std::string req = "POST http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Content-Length: 15\r\n"
                    "User-Agent: RandomBrowser1.1\r\n\r\n";
  std::string req_only_body = "Hey there! Sup?";

  HANDLE_METHOD("POST");
  HANDLE_VERSION();
  HANDLE_URL();
  sett.handle_header = [](const std::string &field, const std::string &val) {
    if (field == "Host") {
      EXPECT_EQ(val, "www.example.com");
      return RETURN::NO_ERROR;
    } else if (field == "User-Agent") {
      EXPECT_EQ(val, "RandomBrowser1.1");
      return RETURN::NO_ERROR;
    } else if (field == "Content-Length") {
      EXPECT_EQ(val, "15");
      return RETURN::PAUSE;
    }

    return RETURN::ERROR;
  };  
  HANDLE_BODY("Hey there! Sup?");

  Parser parser(&sett);
  size_t req_size = req.size();
  int ret = parser.parser_execute(req.data(), &req_size);
  EXPECT_EQ(ret, RETURN::PAUSE);
  req += req_only_body;
  req_size += req_only_body.size();
  ret = parser.parser_resume();
  EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}