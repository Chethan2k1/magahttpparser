#include <gtest/gtest.h>
#include <iostream>
#include <parser.hpp>
#include <string>

TEST(Request, ResponseWithoutBody) {
  settings sett;
  sett.handle_method = [](const std::string &method) {
    EXPECT_EQ(method, "GET");
    return 0;
  };

  sett.handle_version = [](const int &mjv, const int &miv) {
    EXPECT_EQ(mjv, 1);
    EXPECT_EQ(miv, 1);
    return 0;
  };

  sett.handle_url = [](const std::string &url) {
    EXPECT_EQ(url, "http://www.example.com/123");
    return 0;
  };

  sett.handle_header = [](const std::string &field, const std::string &val) {
    if (field == "Host") {
      EXPECT_EQ(val, "www.example.com");
      return 0;
    } else if (field == "User-Agent") {
      EXPECT_EQ(val, "RandomBrowser1.1");
      return 0;
    }
    
    return -1;
  };

  std::string req = "GET http://www.example.com/123 HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "User-Agent: RandomBrowser1.1\r\n";

  Parser parser(&sett);
  size_t req_size = req.size();
  int ret = parser.parser_execute(req.data(), &req_size);
  EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}