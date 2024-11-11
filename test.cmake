
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

#
# tests
#

add_executable(test_parser kiraz/test/test_parser.cc)
target_link_libraries(test_parser kiraz GTest::gtest_main ${FLEX_LIBRARIES})
gtest_discover_tests(test_parser)

# add_executable(test_parser_gicik kiraz/test/test_parser_gicik.cc)
# target_link_libraries(test_parser_gicik kiraz GTest::gtest_main ${FLEX_LIBRARIES})
# gtest_discover_tests(test_parser_gicik)

add_executable(test_semantics kiraz/test/test_semantics.cc)
target_link_libraries(test_semantics kiraz GTest::gtest_main ${FLEX_LIBRARIES})
gtest_discover_tests(test_semantics)
