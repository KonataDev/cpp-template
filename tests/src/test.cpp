#include "test.h"
#include "cpp_template/hello.h"

SCENARIO("catch2 hello", "[cpp-template]") // NOLINT
{
    STATIC_REQUIRE(foo() == 42);
    INFO("hello world!");
}
