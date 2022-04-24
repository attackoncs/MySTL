#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

#include <string>
#include <cstring>

#include "basic_string.h"

namespace mystl {
    typedef basic_string<char>     string;
    typedef basic_string<wchar_t>  w_string;
}

#endif