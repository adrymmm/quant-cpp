include(GoogleTest)

add_executable(quant_tests test_payoff.cpp test_black_scholes.cpp test_monte_carlo.cpp)
target_link_libraries(quant_tests PRIVATE project_warnings quant::pricing GTest::gtest_main)
gtest_discover_tests(quant_tests PROPERTIES TIMEOUT 10)

add_executable(book_tests test_order_book.cpp) 
target_link_libraries(book_tests PRIVATE project_warnings quant::book GTest::gtest_main)
gtest_discover_tests(book_tests PROPERTIES TIMEOUT 10)

add_executable(id_pool_tests test_id_pool.cpp) 
target_link_libraries(id_pool_tests PRIVATE project_warnings quant::book GTest::gtest_main)
target_include_directories(id_pool_tests PRIVATE ${PROJECT_SOURCE_DIR}/bench)
gtest_discover_tests(id_pool_tests PROPERTIES TIMEOUT 10)

