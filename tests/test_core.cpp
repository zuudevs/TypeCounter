#include "../src/date_time.hpp"
#include "../src/logger.hpp"
#include "../src/reporter.hpp"
#include <cassert>
#include <iostream>

class MockReporter : public zuu::Reporter {
public:
    explicit MockReporter(zuu::Logger& l) : Reporter(l) {}
    void initialize() override {}
    
    using Reporter::isSameDay;
    using Reporter::tryOpen;
    using Reporter::close;
};

int main() {
    using namespace zuu;

    // Test DateTime::isSameDay
    DateTime dt1 = DateTime::Now();
    DateTime dt2 = dt1;
    assert(dt1.isSameDay(dt2));
    assert(dt1 == dt2);

    // Test Logger and Reporter tryOpen / close
    Logger logger("test_run.log");
    logger.open();
    assert(logger.isOpen());

    MockReporter reporter(logger);
    bool opened = reporter.tryOpen("test_output.tmp");
    assert(opened);
    assert(reporter.isSameDay(dt1));

    reporter.close();
    logger.close();

    std::cout << "All core self-checks passed!\n";
    return 0;
}