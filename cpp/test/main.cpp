#include <fountain_tools/fountain.h>
#include <catch_amalgamated.hpp>


TEST_CASE( "Basic test", "[say_hello]" ) {
    REQUIRE( fountain::say_hello() == "Hello from Fountain!");
}