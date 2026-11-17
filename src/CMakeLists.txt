add_library(quant_pricing
    pricing/payoff.cpp
    pricing/black_scholes.cpp
)
add_library(quant::pricing ALIAS quant_pricing)

target_include_directories(quant_pricing
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(quant_pricing PUBLIC cxx_std_20)

target_link_libraries(quant_pricing PRIVATE project_warnings)
