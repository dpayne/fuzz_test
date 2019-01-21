/**
 *
 * Much of this code is lifted from https://github.com/trailofbits/deepstate
 * The modifications mostly center around simplification and only support libfuzzer
 *
 */
#include <iostream>
#include <string>
#include "fuzz_test.h"

static std::string RUN_TEST_PARAM = "-run_test=";
std::string __test_to_fuzz = "";
static FuzzTestInfo FIRST_TEST_INFO { NULL, NULL, NULL };
struct FuzzTestInfo * __fuzz_last_test = &FIRST_TEST_INFO;

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    for ( int i = 0; i < *argc; ++i )
    {
        std::string arg{(*argv)[i]};
        if ( arg.size() >= RUN_TEST_PARAM.size() && arg.substr( 0, RUN_TEST_PARAM.size() ) == RUN_TEST_PARAM )
        {
            __test_to_fuzz = arg.substr( RUN_TEST_PARAM.size() );
        }
    }

    if ( __test_to_fuzz.empty() )
    {
        std::cerr << "\u001b[31mNo test specified\u001b[0m: Specify test with run_test=<test_to_fuzz>" << std::endl;
        exit(1);
    }

    return 0;
}

extern "C" int LLVMFuzzerTestOneInput( const uint8_t * Data, size_t Size )
{
    void (* test)( const uint8_t *, size_t ) = nullptr;

    for ( auto testInfo = __fuzz_last_test; testInfo != nullptr; testInfo = testInfo->prev )
    {
        if ( testInfo->test_name != nullptr )
        {
            if ( std::string{ testInfo->test_name } == __test_to_fuzz )
            {
                test = testInfo->test;
            }
        }
    }

    if ( test != nullptr )
    {
        (*test)( Data, Size );
    }
    else
    {
        std::cerr << "\u001b[31mNo test with name " << __test_to_fuzz << "\u001b[0m" << std::endl;
        exit(1);
    }
    return 0;
}
