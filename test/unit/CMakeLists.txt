include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

add_executable(unit_tests test_helpers.cpp test_sstring.cpp)
target_link_libraries(unit_tests PRIVATE libmeta doctest_with_main)

add_test(NAME unit COMMAND unit_tests)
