#pragma once
#include <iostream>

#define STUB_WARNING( text )\
std::cerr << __FILE__ << " line:" << __LINE__ << " function stub called! - " << text << std::endl;\
exit(1);

