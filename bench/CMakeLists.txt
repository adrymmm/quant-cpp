add_executable(book_bench bench_book.cpp stream.cpp)
target_link_libraries(book_bench PRIVATE project_warnings quant::book)
