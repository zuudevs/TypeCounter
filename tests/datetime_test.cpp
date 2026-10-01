#include <datetime.hpp>
#include <gtest/gtest.h>

TEST(unit_tests, datetime) {
	constexpr auto first_date_in_oct_2026 = 1790787600000ull;
	auto dt = zuu::Datetime::Now();

	EXPECT_EQ(dt.get_timestamp().length(), 23);
	EXPECT_EQ(dt.get_datetime_str().length(), 19);
	EXPECT_EQ(dt.get_date_str().length(), 10);
	EXPECT_EQ(sizeof(dt.get_epoch()), sizeof(long long));
	EXPECT_EQ(zuu::Datetime::FromEpoch(first_date_in_oct_2026).get_timestamp(), "01-10-2026 00:00:00.000");
}