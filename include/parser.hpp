#pragma once
#include <string>

enum ERROR {
    UNKNOWN_METHOD,
    INVALID_SYNTAX,
    INVALID_VERSION,
    UNEXPECTED_EOF
};

// enum METHOD {
//     GET,
//     POST,
//     HEAD
// };

using method_cb = int(*)(const std::string&);
using version_cb = int(*)(const int&,const int&); // major_version,minor_version
using data_cb = int(*)(const std::string&);

struct settings {
    void* data;
    method_cb handle_method;
    version_cb handle_version;
    data_cb handle_url;
    data_cb handle_header_field;
    data_cb handle_header_value;
};

enum State {
    STARTLINE,
    HEADER,
    BODY
};

struct Parser {
    private:
    char* curr_ptr;
    const char* start_ptr;
    size_t* req_size;
    State* state;
    inline int PARSE_INT();
    int parse_http_version();
    int parse_start_line();
    int parse_headers();

    public:
    settings *sett;

    explicit Parser(settings *setting)
        :sett(setting){}

    int parser_execute(const char*,size_t*);
};
