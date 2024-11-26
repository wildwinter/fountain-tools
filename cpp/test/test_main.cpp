#include <fountain_tools/fountain.h>
#include <catch_amalgamated.hpp>


TEST_CASE( "Basic test", "[say_hello]" ) {
    REQUIRE( Fountain::elementToString(Fountain::Element::BONEYARD) == "BONEYARD");
}