/**
 * @file test.c
 * @author Jens Trappmann (jens.trappmann@icloud.com)
 * @brief This file contains all tests required to cover both standard cases and edge cases,
 *        and to verify the correctness of the sliding median window implementation.
 *        For this purpose, multiple test scenarios were developed whose parameters can be adjusted.
 *        It is important to note that the edge case tests are tailored to the sizes of the specific window
 *        implementations. This means that the tiny window tests validate the dedicated Sorting/Median Network
 *        implementation used for median calculation, while the big window tests validate the specific
 *        double-heap implementation.
 *        Please note: Sorting/Median Networks are used for window sizes from 2 to 8. The double-heap approach,
 *        on the other hand, is used for larger window sizes.
 * @version 0.1
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "medianwindow_api.h"
#include "mediantester.h"

#define TEST_SEED 0xC0FFEE
#define LOWEST_VALUE_NORMAL_INPUT_TEST -1000
#define HEIGHEST_VALUE_NORMAL_INPUT_TEST 1000
#define EPSILON 1e-9

#define TEST_ARRAY_SIZE_STD_TESTS 10

#define TEST_ARRAY_SIZE_EDGE_TESTS_TINY 10
#define TEST_EDGE_CASE_TINY_WINDOWSIZE 5

#define TEST_ARRAY_SIZE_EDGE_TESTS_BIG 20
#define TEST_EDGE_CASE_BIG_WINDOWSIZE 10

#define TEST_ARRAY_SIZE_FOR_CORRECTNESS 100000

#define TEST_SPC_NUMBERS_NANS_COUNT_ONE 1000
#define TEST_SPC_NUMBERS_INF_COUNT_ONE 5000
#define TEST_SPC_NUMBERS_NANS_COUNT_TWO 10000
#define TEST_SPC_NUMBERS_INF_COUNT_TWO 15000
#define TEST_SPC_NUMBERS_NANS_COUNT_THREE 35000
#define TEST_SPC_NUMBERS_INF_COUNT_THREE 40000
#define TEST_SPC_NUMBERS_NANS_COUNT_FOUR 50000

#define TEST_ONE_WINDOWSIZE 5
#define TEST_ONE_STEPS 1

#define TEST_TWO_WINDOWSIZE 7
#define TEST_TWO_STEPS 5

#define TEST_THREE_WINDOWSIZE 8
#define TEST_THREE_STEPS 8

#define TEST_FOUR_WINDOWSIZE 10
#define TEST_FOUR_STEPS 1

#define TEST_FIVE_WINDOWSIZE 10
#define TEST_FIVE_STEPS 10

#define TEST_SIX_WINDOWSIZE 100
#define TEST_SIX_STEPS 1

#define TEST_SEVEN_WINDOWSIZE 1000
#define TEST_SEVEN_STEPS 1

#define TEST_EIGHT_WINDOWSIZE 1153
#define TEST_EIGHT_STEPS 373

#define TEST_NINE_WINDOWSIZE 9999
#define TEST_NINE_STEPS 1

#define TEST_TEN_WINDOWSIZE 12000
#define TEST_TEN_STEPS 9991

static void run_standard_tests(void);

static void run_edge_case_tests_tiny_window(void);
static void run_first_edge_case_test_tiny_window(void);
static void run_second_edge_case_test_tiny_window(void);
static void run_third_edge_case_test_tiny_window(void);
static void run_fourth_edge_case_test_tiny_window(void);

static void run_edge_case_tests_big_window(void);
static void run_first_edge_case_test_big_window(void);
static void run_second_edge_case_test_big_window(void);
static void run_third_edge_case_test_big_window(void);
static void run_fourth_edge_case_test_big_window(void);

static void run_tests_normal_input(void);
static bool test_normal_input(size_t testArrayLength, size_t windowSize, size_t steps);

static void run_tests_normal_spc_input_ignoring_nan(void);
static void run_tests_normal_spc_input_not_ignoring_nan(void);
static bool test_input_with_spc_numbers(size_t testArrayLength, size_t windowSize, size_t steps,
    bool ignoreNanWindows, size_t numNaNs, size_t numInfs);

static void test_array_init(size_t length, double lowestValue, double heighestValue, double *dest);
static void test_array_init_random_nans(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void test_array_init_random_posinfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void test_array_init_random_neginfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void fill_and_shuffle_spc_number_indizes_array(size_t length, size_t *spcNumbersIndizesArray);
static bool result_array_init(size_t inputLength, size_t windowSize, size_t steps,
    size_t *resultLength, double **result);

int main(void) {
    srand(TEST_SEED);
    run_standard_tests();
    run_edge_case_tests_tiny_window();
    run_edge_case_tests_big_window();
    // run_tests_normal_input();
    run_tests_normal_spc_input_ignoring_nan();
    run_tests_normal_spc_input_not_ignoring_nan();
    return 0;
}

static void run_standard_tests(void) {
    double testArray[TEST_ARRAY_SIZE_STD_TESTS];
    test_array_init(TEST_ARRAY_SIZE_STD_TESTS,
        LOWEST_VALUE_NORMAL_INPUT_TEST,
        HEIGHEST_VALUE_NORMAL_INPUT_TEST,
        testArray);

    // All tests are independent of the outputArray, so see this as a dummy
    // Normally it would be of size ((inputArrayLength - windowSize) / steps + 1)
    // For test purposes it is oversized
    double outputArray[TEST_ARRAY_SIZE_STD_TESTS];

    // Should return false because inputArray == NULL
    assert(!sliding_medianwindow(NULL, TEST_ARRAY_SIZE_STD_TESTS, 20, 1, false, outputArray));

    // Should return false because length == 0
    assert(!sliding_medianwindow(testArray, 0, 20, 1, false, outputArray));

    // Should return false because windowSize > arraySize
    assert(!sliding_medianwindow(testArray, TEST_ARRAY_SIZE_STD_TESTS,
        TEST_ARRAY_SIZE_STD_TESTS + 1, 1, false, outputArray));

    // Should return false because windowSize < 2
    assert(!sliding_medianwindow(testArray, TEST_ARRAY_SIZE_STD_TESTS, 1, 1, false, outputArray));

    // Should return false because steps < 1
    assert(!sliding_medianwindow(testArray, TEST_ARRAY_SIZE_STD_TESTS, 2, 0, false, outputArray));

    // Should return false because outputArray == NULL
    assert(!sliding_medianwindow(testArray, TEST_ARRAY_SIZE_STD_TESTS, 20, 1, false, NULL));

    // To prove a working window the windowSize must be lower than TEST_ARRAY_SIZE_STD_TESTS
    // (e.g. 10). So we are working with a windowSize of 5 here.
    // The outputArray is also of size TEST_ARRAY_SIZE_STD_TESTS. Normally it needs the size of
    // ((inputArrayLength - windowSize) / steps + 1)
    // Should return true
    assert(sliding_medianwindow(testArray, TEST_ARRAY_SIZE_STD_TESTS, 5, 1, false, outputArray));

    printf("All standard tests passed!\n");
}

static void run_edge_case_tests_tiny_window(void) {
    run_first_edge_case_test_tiny_window();
    run_second_edge_case_test_tiny_window();
    run_third_edge_case_test_tiny_window();
    run_fourth_edge_case_test_tiny_window();
    printf("All edge case tests for the tiny window passed!\n");
}

static void run_first_edge_case_test_tiny_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_TINY];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_TINY; i++)
        testArray[i] = NAN;

    // For the test purpose of the tiny window, we choose a windowSize of 5 and steps of 1
    // Notice: Tiny Windows are windows working with median/sorting networks
    // The software supports tiny windows of size 2 - 8
    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_TINY - TEST_EDGE_CASE_TINY_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];

    // First we will test with ignoreNaNWindows = false
    // Nonetheless should be every elements in outputArray NaN
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, false, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));

    // After that we will test with ignoreNaNWindows = true
    // Also in this test every element in outputArray should be NaN
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, true, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_second_edge_case_test_tiny_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_TINY];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_TINY; i++)
        testArray[i] = NAN;

    // In this test we will see that the ignoreNaNWindows arguement of the sliding_medianwindow
    // function affects how the window treats windows containing NaN
    testArray[5] = 42.5;

    // For the test purpose of the tiny window, we choose a windowSize of 5 and steps of 1
    // Notice: Tiny Windows are windows working with median/sorting networks
    // The software supports tiny windows of size 2 - 8
    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_TINY - TEST_EDGE_CASE_TINY_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];

    // First we will test with ignoreNaNWindows = false
    // ignoreNaNWindows set on false will result in calculating the median for all elements
    // in the window that are not NaN
    // This means: If you have a window with windowSize = 5 and all values are NaN the result will
    // also be NaN (see the test above)
    // If there is more than one value not NaN then the median will be calculated of these remaining values
    // For our example that means: 4 of the 5 values are NaN and one value is not
    // The median should be the same as that value
    // If there are two values not NaN, the result should be the median of those two values and so on.
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, false, outputArray);

    // We got a windowSize of 5 and the value 42.5 lives on the 5th index of the array
    // So the first window output should be NaN
    assert(isnan(outputArray[0]));

    // After that we reach the index 5 and the value 42.5 that is now living inside the range of the window
    // So the second window output should be 42.5
    assert(outputArray[1] == 42.5);
    assert(outputArray[2] == 42.5); // Because the window just moves one step, 42.5 is still inside the window
                                    // and also the result

    // The testArray is of length 10 and the value 42.5 lives on the 5th index
    // So in the last window on the testArray the value should be the first value of the window
    // This means also the last window output should be of value 42.5
    // The outputArray has a length of ((10 - 5) / 1 + 1) = 6
    assert(outputArray[5] == 42.5);

    // After that we will test with ignoreNaNWindows = true
    // In this test everytime there lives a NaN inside the window the window output will also be NaN
    // Imagine it like throwing the window away when there is a NaN value inside
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, true, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_third_edge_case_test_tiny_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_TINY];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_TINY; i++)
        testArray[i] = NAN;

    // In this test besides NaN there will also be Infinity inside the testArray
    testArray[3] = INFINITY;
    testArray[4] = 42.5;
    testArray[5] = 50;
    testArray[6] = -INFINITY;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_TINY - TEST_EDGE_CASE_TINY_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];

    // This test works with ignoreNaNWindows = false, because if we would set it to true
    // the whole window would be invalid because of the presence of NaN
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, false, outputArray);

    // The first window should contain the values NaN, NaN, NaN, INFINITY, 42.5
    // Effectively we got two valid values inside of this window (e.g. INFINITY and 42.5)
    // So the median should be between them
    const double predictedMedianOne = ((42.5 + INFINITY) / 2);
    assert(outputArray[0] == predictedMedianOne);

    // The second window should contain the values NaN, NaN, INFINITY, 42.5, 50
    // Effectively we got three valid values inside of this window now (e.g. INFINITY, 42.5, 50)
    // So the median should be the middle value after sorting them
    // Sorting order should be 42.5, 50, INFINITY
    assert(outputArray[1] == 50);

    // After moving again the window should contain the values NaN, INFINITY, 42.5, 50, -INFINITY
    // Effectively we now got four valid values inside of this window (e.g. INFINITY, 42.5, 50, -INFINITY)
    // So the median should after sorting them the value between 42.5 and 50
    const double predictedMedianTwo = ((42.5 + 50) / 2);
    assert(outputArray[2] == predictedMedianTwo);

    // The same procedure goes on and on, until we reach the end of our input sequence
    // Notice: This only works with ignoreNaNWindows set to false
    // Otherwise the complete outputArray should be NaN
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, true, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_fourth_edge_case_test_tiny_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_TINY];
    // In this test we will fill the testArray with TEST_ARRAY_SIZE_EDGE_TESTS_TINY same values
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_TINY; i++)
        testArray[i] = 7;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_TINY - TEST_EDGE_CASE_TINY_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];

    // In such a case it doesnt matter if you set ignoreNaNWindows on true or false, because there
    // are no NaNs inside the window :)
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_TINY, TEST_EDGE_CASE_TINY_WINDOWSIZE,
        1, false, outputArray);

    // Because all values are the same the median should also be the same value (e.g. 7)
    for(size_t i = 0; i < outputArraySize; i++)
        assert(outputArray[i] == 7);
}

// Please notice: The following tests include the same test sequences as above, just for bigger windows
// (e.g. windows using a double heap implementation)
// For a more detailed description of the tests see the edge case tests for tiny windows above
// Notice: By bigger windows I mean windows with a windowSize of 9 ... upwards
static void run_edge_case_tests_big_window(void) {
    run_first_edge_case_test_big_window();
    run_second_edge_case_test_big_window();
    run_third_edge_case_test_big_window();
    run_fourth_edge_case_test_big_window();
    printf("All edge case tests for the big window passed!\n");
}

static void run_first_edge_case_test_big_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_BIG];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_BIG; i++)
        testArray[i] = NAN;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_BIG - TEST_EDGE_CASE_BIG_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, false, outputArray);
    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));

    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, true, outputArray);
    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_second_edge_case_test_big_window(void) {
 double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_BIG];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_BIG; i++)
        testArray[i] = NAN;

    testArray[10] = 42.5;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_BIG - TEST_EDGE_CASE_BIG_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, false, outputArray);

    // We got a windowSize of 10 and the value 42.5 lives on the 10th index of the array
    // So the first window output should be NaN
    assert(isnan(outputArray[0]));

    // After that we reach the index 10 and the value 42.5 that is now living inside the range of the window
    // So the second window output should be 42.5
    assert(outputArray[1] == 42.5);
    assert(outputArray[2] == 42.5); // Because the window just moves one step, 42.5 is still inside the window
                                    // and also the result
    assert(outputArray[10] == 42.5);

    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, true, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_third_edge_case_test_big_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_BIG];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_BIG; i++)
        testArray[i] = NAN;

    testArray[8] = INFINITY;
    testArray[9] = 42.5;
    testArray[10] = 50;
    testArray[11] = -INFINITY;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_BIG - TEST_EDGE_CASE_BIG_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, false, outputArray);

    // The first window should contain the values NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, INFINITY, 42.5
    // So after ignoring every NaN inside the median calculation, there are also only 42.5 and INFINITY
    // as valid values inside the window
    const double predictedMedianOne = ((42.5 + INFINITY) / 2);
    assert(outputArray[0] == predictedMedianOne);
    assert(outputArray[1] == 50);
    const double predictedMedianTwo = ((42.5 + 50) / 2);
    assert(outputArray[2] == predictedMedianTwo);

    // In the case of ignoreNaNWindows = true the complete outputArray should be NaN
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, true, outputArray);

    for(size_t i = 0; i < outputArraySize; i++)
        assert(isnan(outputArray[i]));
}

static void run_fourth_edge_case_test_big_window(void) {
    double testArray[TEST_ARRAY_SIZE_EDGE_TESTS_BIG];
    for(size_t i = 0; i < TEST_ARRAY_SIZE_EDGE_TESTS_BIG; i++)
        testArray[i] = 7;

    const size_t outputArraySize = ((TEST_ARRAY_SIZE_EDGE_TESTS_BIG - TEST_EDGE_CASE_BIG_WINDOWSIZE) / 1 + 1);
    double outputArray[outputArraySize];
    sliding_medianwindow(testArray, TEST_ARRAY_SIZE_EDGE_TESTS_BIG, TEST_EDGE_CASE_BIG_WINDOWSIZE,
        1, false, outputArray);

    // Because all values are the same the median should also be the same value (e.g. 7)
    for(size_t i = 0; i < outputArraySize; i++)
        assert(outputArray[i] == 7);
}

// The following tests are testing the correctness of the resulting median computation.
// These tests generate an array consisting of random double values in the range from LOWEST_VALUE_NORMAL_INPUT_TEST
// to HIGHEST_VALUE_NORMAL_INPUT_TEST. No NaN or infinity values are included in these tests.
// ignoreNaNWindows is set to false
static void run_tests_normal_input(void) {
    bool testOne = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_ONE_WINDOWSIZE, TEST_ONE_STEPS);
    bool testTwo = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_TWO_WINDOWSIZE, TEST_TWO_STEPS);
    bool testThree = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_THREE_WINDOWSIZE, TEST_THREE_STEPS);
    bool testFour = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_FOUR_WINDOWSIZE, TEST_FOUR_STEPS);
    bool testFive = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_FIVE_WINDOWSIZE, TEST_FIVE_STEPS);
    bool testSix = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_SIX_WINDOWSIZE, TEST_SIX_STEPS);
    bool testSeven = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_SEVEN_WINDOWSIZE, TEST_SEVEN_STEPS);
    bool testEight = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_EIGHT_WINDOWSIZE, TEST_EIGHT_STEPS);
    bool testNine = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_NINE_WINDOWSIZE, TEST_NINE_STEPS);
    bool testTen = test_normal_input(TEST_ARRAY_SIZE_FOR_CORRECTNESS, TEST_TEN_WINDOWSIZE, TEST_TEN_STEPS);

    assert(testOne);
    assert(testTwo);
    assert(testThree);
    assert(testFour);
    assert(testFive);
    assert(testSix);
    assert(testSeven);
    assert(testEight);
    assert(testNine);
    assert(testTen);

    printf("All normal input tests passed\n");
}

static bool test_normal_input(size_t testArrayLength, size_t windowSize, size_t steps) {
    double *testArray = (double* ) malloc(testArrayLength * sizeof(double));
    if(testArray == NULL)
        return false;
    test_array_init(testArrayLength,
        LOWEST_VALUE_NORMAL_INPUT_TEST,
        HEIGHEST_VALUE_NORMAL_INPUT_TEST,
        testArray);

    double *resultArray_sliding = NULL;
    size_t resultArray_sliding_length = 0;
    result_array_init(testArrayLength,
        windowSize, steps,
        &resultArray_sliding_length, &resultArray_sliding);
    if(resultArray_sliding == NULL) {
        free(testArray);
        testArray = NULL;
        return false;
    }

    double *resultArray_mediantester = NULL;
    size_t resultArray_mediantester_length = 0;
    result_array_init(testArrayLength,
        windowSize, steps,
        &resultArray_mediantester_length, &resultArray_mediantester);
    if(resultArray_mediantester == NULL) {
        free(testArray);
        testArray = NULL;

        free(resultArray_sliding);
        resultArray_sliding = NULL;
        return false;
    }

    assert(resultArray_sliding_length == resultArray_mediantester_length);

    sliding_medianwindow(testArray, testArrayLength, windowSize, steps, false, resultArray_sliding);
    median_tester_gen_medians(testArray, testArrayLength, windowSize, steps, false, resultArray_mediantester);

    for(size_t i = 0; i < resultArray_sliding_length; i++) {
        const double diff = fabs(resultArray_sliding[i] - resultArray_mediantester[i]);
        assert(diff < EPSILON);
    }

    free(testArray);
    testArray = NULL;
    free(resultArray_sliding);
    resultArray_sliding = NULL;
    free(resultArray_mediantester);
    resultArray_mediantester = NULL;

    return true;
}

// The following tests are testing the correctness of the resulting median computation.
// These tests generate an array consisting of random double values in the range from LOWEST_VALUE_NORMAL_INPUT_TEST
// to HIGHEST_VALUE_NORMAL_INPUT_TEST. Additionally NaN or infinity values are added.
// ignoreNaNWindows is set to true
// This results in a NaN output for a window if at least one NaN value is present within that window.
static void run_tests_normal_spc_input_ignoring_nan(void) {
    bool testOne = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_ONE_WINDOWSIZE, TEST_ONE_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_TWO, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testTwo = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_TWO_WINDOWSIZE, TEST_TWO_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_TWO);
    bool testThree = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_THREE_WINDOWSIZE, TEST_THREE_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testFour = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_FOUR_WINDOWSIZE, TEST_FOUR_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_ONE, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testFive = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_FIVE_WINDOWSIZE, TEST_FIVE_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_TWO);
    bool testSix = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_SIX_WINDOWSIZE, TEST_SIX_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testSeven = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_SEVEN_WINDOWSIZE, TEST_SEVEN_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testEight = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_EIGHT_WINDOWSIZE, TEST_EIGHT_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_TWO);
    bool testNine = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_NINE_WINDOWSIZE, TEST_NINE_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testTen = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_TEN_WINDOWSIZE, TEST_TEN_STEPS, true,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_THREE);

    assert(testOne);
    assert(testTwo);
    assert(testThree);
    assert(testFour);
    assert(testFive);
    assert(testSix);
    assert(testSeven);
    assert(testEight);
    assert(testNine);
    assert(testTen);

    printf("All special/normal input tests passed (ignoring nan)\n");
}

// The following tests are testing the correctness of the resulting median computation.
// These tests generate an array consisting of random double values in the range from LOWEST_VALUE_NORMAL_INPUT_TEST
// to HIGHEST_VALUE_NORMAL_INPUT_TEST. Additionally NaN or infinity values are added.
// ignoreNaNWindows is set to false
// This results in the calculation of the median from the remaining values in a window, excluding any NaN values.
static void run_tests_normal_spc_input_not_ignoring_nan(void) {
    bool testOne = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_ONE_WINDOWSIZE, TEST_ONE_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_ONE, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testTwo = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_TWO_WINDOWSIZE, TEST_TWO_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_ONE, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testThree = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_THREE_WINDOWSIZE, TEST_THREE_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_TWO, TEST_SPC_NUMBERS_INF_COUNT_TWO);
    bool testFour = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_FOUR_WINDOWSIZE, TEST_FOUR_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_ONE, TEST_SPC_NUMBERS_INF_COUNT_ONE);
    bool testFive = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_FIVE_WINDOWSIZE, TEST_FIVE_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testSix = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_SIX_WINDOWSIZE, TEST_SIX_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testSeven = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_SEVEN_WINDOWSIZE, TEST_SEVEN_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testEight = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_EIGHT_WINDOWSIZE, TEST_EIGHT_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_TWO);
    bool testNine = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_NINE_WINDOWSIZE, TEST_NINE_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_THREE, TEST_SPC_NUMBERS_INF_COUNT_THREE);
    bool testTen = test_input_with_spc_numbers(TEST_ARRAY_SIZE_FOR_CORRECTNESS,
        TEST_TEN_WINDOWSIZE, TEST_TEN_STEPS, false,
        TEST_SPC_NUMBERS_NANS_COUNT_FOUR, TEST_SPC_NUMBERS_INF_COUNT_THREE);

    assert(testOne);
    assert(testTwo);
    assert(testThree);
    assert(testFour);
    assert(testFive);
    assert(testSix);
    assert(testSeven);
    assert(testEight);
    assert(testNine);
    assert(testTen);

    printf("All special/normal input tests passed (not ignoring nan)\n");
}

static bool test_input_with_spc_numbers(size_t testArrayLength, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, size_t numNaNs, size_t numInfs) {
    double *testArray = (double* ) malloc(testArrayLength * sizeof(double));
    test_array_init(testArrayLength,
        LOWEST_VALUE_NORMAL_INPUT_TEST,
        HEIGHEST_VALUE_NORMAL_INPUT_TEST,
        testArray);

    size_t *spcNumberIndizesArray = (size_t* ) malloc(testArrayLength * sizeof(size_t));
    if(spcNumberIndizesArray == NULL) {
        free(testArray);
        testArray = NULL;
        return false;
    }

    fill_and_shuffle_spc_number_indizes_array(testArrayLength, spcNumberIndizesArray);
    size_t currentIndex = 0;
    test_array_init_random_nans(testArray, &currentIndex, numNaNs, spcNumberIndizesArray);
    const size_t posInfs = (numInfs / 2);
    test_array_init_random_posinfs(testArray, &currentIndex, posInfs, spcNumberIndizesArray);
    test_array_init_random_neginfs(testArray, &currentIndex, (numInfs - posInfs), spcNumberIndizesArray);

    double *resultArray_sliding = NULL;
    size_t resultArray_sliding_length = 0;
    result_array_init(testArrayLength,
        windowSize, steps,
        &resultArray_sliding_length, &resultArray_sliding);
    if(resultArray_sliding == NULL) {
        free(testArray);
        testArray = NULL;
        return false;
    }

    double *resultArray_mediantester = NULL;
    size_t resultArray_mediantester_length = 0;
    result_array_init(testArrayLength,
        windowSize, steps,
        &resultArray_mediantester_length, &resultArray_mediantester);
    if(resultArray_mediantester == NULL) {
        free(testArray);
        testArray = NULL;

        free(resultArray_sliding);
        resultArray_sliding = NULL;
        return false;
    }

    assert(resultArray_sliding_length == resultArray_mediantester_length);

    sliding_medianwindow(testArray, testArrayLength, windowSize, steps,
        ignoreNaNWindows, resultArray_sliding);
    median_tester_gen_medians(testArray, testArrayLength, windowSize, steps,
        ignoreNaNWindows, resultArray_mediantester);

    for(size_t i = 0; i < resultArray_sliding_length; i++) {
        if(isnan(resultArray_sliding[i])) {
            assert(isnan(resultArray_mediantester[i]));
            continue;
        }

        if(isinf(resultArray_sliding[i])) {
            assert(isinf(resultArray_mediantester[i]));
            continue;
        }

        const double diff = fabs(resultArray_sliding[i] - resultArray_mediantester[i]);
        assert(diff < EPSILON);
    }

    free(testArray);
    testArray = NULL;
    free(resultArray_sliding);
    resultArray_sliding = NULL;
    free(resultArray_mediantester);
    resultArray_mediantester = NULL;

    return true;
}

// Test Util Methods

static void test_array_init(size_t length, double lowestValue, double heighestValue, double *dest) {
    for(size_t i = 0; i < length; i++) {
        const double v = (lowestValue + (heighestValue - lowestValue) * ((double) rand() / (double) RAND_MAX));
        dest[i] = v;
    }
}

static void test_array_init_random_nans(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray) {
    for(size_t i = 0; i < num; i++) {
        const size_t index = spcNumbersIndizesArray[*currentIndex];
        testArray[index] = NAN;
        *currentIndex += 1;
    }
}

static void test_array_init_random_posinfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray) {
    for(size_t i = 0; i < num; i++) {
        const size_t index = spcNumbersIndizesArray[*currentIndex];
        testArray[index] = INFINITY;
        *currentIndex += 1;
    }
}

static void test_array_init_random_neginfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray) {
    for(size_t i = 0; i < num; i++) {
        const size_t index = spcNumbersIndizesArray[*currentIndex];
        testArray[index] = -INFINITY;
        *currentIndex += 1;
    }
}

static void fill_and_shuffle_spc_number_indizes_array(size_t length, size_t *spcNumbersIndizesArray) {
    for(size_t i = 0; i < length; i++)
        spcNumbersIndizesArray[i] = i;

    for(size_t i = 0; i < length; i++) {
        const size_t j = i + (rand() % (length - i));
        const size_t tmpIndex = spcNumbersIndizesArray[i];
        spcNumbersIndizesArray[i] = spcNumbersIndizesArray[j];
        spcNumbersIndizesArray[j] = tmpIndex;
    }
}

static bool result_array_init(size_t inputLength, size_t windowSize, size_t steps,
    size_t *resultLength, double **result) {
    if((inputLength == 0) || (windowSize <= 1) || (steps == 0) || (result == NULL) || (*result != NULL))
        return false;

    const size_t estimatedLength = (inputLength - windowSize) / steps + 1;
    *result = (double* ) malloc(estimatedLength * sizeof(double));
    if(*result == NULL)
        return false;

    *resultLength = estimatedLength;
    return true;
}
