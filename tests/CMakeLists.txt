include(GoogleTest)

add_executable(quant_tests test_payoff.cpp)
target_link_libraries(quant_tests PRIVATE project_warnings quant::pricing GTest::gtest_main)
gtest_discover_tests(quant_tests PROPERTIES TIMEOUT 10)

