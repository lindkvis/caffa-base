#include "gtest/gtest.h"

#include "cafUuidGenerator.h"

TEST( TestUuid, simpleUuids )
{
    ASSERT_TRUE( caffa::UuidGenerator::isUuid( "00000000-0000-0000-0000-000000000000" ) );
    ASSERT_TRUE( caffa::UuidGenerator::isUuid( "00000000-0000-0000-0000-000000000001" ) );
}

TEST( TestUuid, randomUuids )
{
    constexpr size_t testCount = 10000;
    for (size_t i = 0; i < testCount; ++i)
    {
        auto uuid = caffa::UuidGenerator::generate();
        ASSERT_TRUE( caffa::UuidGenerator::isUuid( uuid ) );
        ASSERT_EQ( 36u, uuid.length() );
    }
}