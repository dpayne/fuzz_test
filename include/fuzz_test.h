#include <stdint.h>
#include <cstddef>
#include <string>

/**
 *
 * Much of this code is lifted from https://github.com/trailofbits/deepstate
 * The modifications mostly center around simplification and only support libfuzzer
 *
 */
struct FuzzTestInfo
{
        struct FuzzTestInfo * prev;
        const char * test_name;
        void (* test)( const uint8_t *, size_t );
};

extern std::string __test_to_fuzz;
extern struct FuzzTestInfo * __fuzz_last_test;

// Stringify a macro parameter
#define FUZZ_TEST_TO_STR(a) _FUZZ_TEST_TO_STR(a)
#define _FUZZ_TEST_TO_STR(a) __FUZZ_TEST_TO_STR(a)
#define __FUZZ_TEST_TO_STR(a) #a

// Initializer/finalizer sample for MSVC and GCC/Clang.
// 2010-2016 Joe Lowe. Released into the public domain.
#ifdef __cplusplus
    #define INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#endif

// This is a hack to create a linked list of test names to their function pointers at initialization *before* main is run
// The hack is done by creating a static struct containing the function name with a pointer to the function definition
//
#define FUZZ_TEST(category, name, var1, var2) \
    /* function declaration of the test, note this has to be done before the static struct is created */ \
    void FuzzTest_Test_ ## category ## _ ## name(const uint8_t * Data, size_t Size); \
                                                                                       \
    /* Create the static struct containing the function pointer and test name, set the previous struct to null */ \
     static struct FuzzTestInfo FuzzTestInfo_ ## category ## _ ## name = { \
       NULL, \
       FUZZ_TEST_TO_STR(category ## _ ## name), \
       FuzzTest_Test_ ## category ## _ ## name \
     }; \
                                                                                       \
    /* This is run before main. This appends the newly created struct to the end of the list */ \
    INITIALIZER(initialize ## category ## _ ## name) { \
       FuzzTestInfo_ ## category ## _ ## name.prev = __fuzz_last_test; \
       __fuzz_last_test = &(FuzzTestInfo_ ## category ## _ ## name); \
    } \
    /* Start of the function implementation, the function name will be FuzzTest_Test_Category_name */ \
    void FuzzTest_Test_ ## category ## _ ## name( var1, var2 )


// This is a useful way to effectively disable a test without having to delete the whole thing
#define FUZZ_TEST_DISABLED(category, name, var1, var2) \
    /* Disable this test */ \
    void FuzzTest_TestDisabled_ ## category ## _ ## name( var1, var2 )
