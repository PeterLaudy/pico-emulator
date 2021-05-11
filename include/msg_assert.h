#ifndef _ASSERT_
#define _ASSERT_

#include <iostream>

#define assert(COND, MSG) if (!(COND)) { std::cerr << MSG << std::endl; abort(); }

#endif