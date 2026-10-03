#include <filesystem>
#include <gtest/gtest.h>
#include <logger.hpp>

TEST(unit_test, logger) {
	auto& log = zuu::Logger::GetInstance();

	log.info("this is info message");
	log.warning("this is warning message");
	log.error("this is error message");
	log.critical("this is critical message");

	namespace fs = std::filesystem;

	auto dt = zuu::Datetime::Now();
	std::string filename;

	filename.reserve(16);
	filename += dt.get_date_str();
	filename += ".log";

	auto fullpath = fs::absolute(fs::current_path() / "logs" / filename);

	EXPECT_EQ(fs::exists(fullpath), true);
}