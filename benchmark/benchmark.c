/**
 * @file benchmark.c
 * @author Jens Trappmann (jens.trappmann@icloud.com)
 * @brief This file enables benchmarks for specific parameter inputs. All parameters are freely configurable,
 *        including the length of the input sequence, the number of NaN values, the number of Inf values,
 *        the lower bound of the randomly generated numbers, the upper bound of the randomly generated numbers,
 *        the window size, the window step size, and the ignoreNaNWindows option.
 *        To run a benchmark, the project must be compiled using Makefile.benchmark, after which the executable named
 *        run_benchmark should be started. The previously mentioned parameters must be provided as command-line
 *        arguments. Important: with the exception of the lower and upper bounds of the randomly generated numbers,
 *        all input parameters are of type unsigned integer.
 * @version 0.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

#include "medianwindow_api.h"

#define RANDOM_SEED 0xC0FFEE
#define HYPHEN_ASCII 45

#define VALID_IGNORENANWINDOWS_TRUE_STR "true"
#define VALID_IGNORENANWINDOWS_TRUE true
#define VALID_IGNORENANWINDOWS_FALSE_STR "false"
#define VALID_IGNORENANWINDOWS_FALSE false

static bool check_unsigned_digit(char *string, size_t *resultDigit);
static bool check_signed_digit(char *string, int64_t *resultDigit);
static size_t convert_str_to_size_t(char *string);
static int64_t convert_str_to_int64_t(char *string, bool negative);
static bool check_valid_ignoreNaNWindows(char *string, bool *result);

static bool benchmark_start(size_t length, size_t nanValues, size_t infValues, double lowestValue,
    double highestValue, size_t windowSize, size_t steps, bool ignoreNaNWindows);
static void test_array_init(size_t length, double lowestValue, double highestValue, double *dest);
static void test_array_init_random_nans(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void test_array_init_random_posinfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void test_array_init_random_neginfs(double *testArray, size_t *currentIndex, size_t num,
    size_t *spcNumbersIndizesArray);
static void fill_and_shuffle_spc_number_indizes_array(size_t length, size_t *spcNumbersArray);
static bool result_array_init(size_t inputLength, size_t windowSize, size_t steps,
    size_t *resultLength, double **result);

static void difference_time_specs(struct timespec *spec1, struct timespec *spec2, struct timespec *result);

int main(int argc, char *argv[]) {
    if((argc <= 1) || (argc > 9)) {
        printf("Please enter eight valid arguments:\n");
        printf("(inputSequenceLength, nanValues, infValues, lowestRandomValue, highestRandomValue) -> for the array\n");
        printf("(windowSize, steps, ignoreNaNWindows) -> for the window\n");
        return EXIT_FAILURE;
    }

    size_t inputSequenceLength = 0, nanValues = 0, infValues = 0;
    int64_t lowestPossibleValue = 0;
    int64_t highestPossibleValue = 0;
    if((!check_unsigned_digit(argv[1], &inputSequenceLength)) || (!check_unsigned_digit(argv[2], &nanValues))
        || (!check_unsigned_digit(argv[3], &infValues)) || (!check_signed_digit(argv[4], &lowestPossibleValue))
        || (!check_signed_digit(argv[5], &highestPossibleValue))) {
        printf("Please enter valid digits!\n");
        return EXIT_FAILURE;
    }

    size_t windowSize = 0, steps = 0;
    if((!check_unsigned_digit(argv[6], &windowSize)) || (!check_unsigned_digit(argv[7], &steps))) {
        printf("Please enter valid digits!\n");
        return EXIT_FAILURE;
    }

    bool ignoreNaNWindows = false;
    if(!check_valid_ignoreNaNWindows(argv[8], &ignoreNaNWindows)) {
        printf("Please enter valid ignoreNaNWindows property (true/false).\n");
        return EXIT_FAILURE;
    }

    srand(RANDOM_SEED);
    const bool benchmarkSuccess = benchmark_start(inputSequenceLength,
                                                nanValues,
                                                infValues,
                                                ((double) lowestPossibleValue),
                                                ((double) highestPossibleValue),
                                                windowSize,
                                                steps,
                                                ignoreNaNWindows);

    if(!benchmarkSuccess) {
        printf("It seems like there was an error!\n");
        return EXIT_FAILURE;
    }

    printf("Benchmark success!\n");
    return EXIT_SUCCESS;
}

static bool check_unsigned_digit(char *string, size_t *resultDigit) {
    const size_t stringLength = strlen(string);

    for(size_t i = 0; i < stringLength; i++) {
        if(!isdigit(string[i]))
            return false;
    }

    *resultDigit = convert_str_to_size_t(string);
    return true;
}

static bool check_signed_digit(char *string, int64_t *resultDigit) {
    const size_t stringLength = strlen(string);
    size_t startingIndex = 0;
    bool negativeDigit = false;
    if(string[0] == HYPHEN_ASCII) {
        startingIndex = 1;
        negativeDigit = true;
    }

    if((stringLength == 1) && (negativeDigit))
        return false;

    for(size_t i = startingIndex; i < stringLength; i++) {
        if(!isdigit(string[i]))
            return false;
    }

    *resultDigit = convert_str_to_int64_t(string, negativeDigit);
    return true;
}

static size_t convert_str_to_size_t(char *string) {
    const size_t stringLength = strlen(string);
    size_t resultNum = 0;
    for(size_t i = 0; i < stringLength; i++)
        resultNum = (resultNum * 10) + (string[i] - '0');
    return resultNum;
}

static int64_t convert_str_to_int64_t(char *string, bool negative) {
    const size_t stringLength = strlen(string);
    const size_t startingIndex = (negative) ? 1 : 0;
    size_t resultNum = 0;
    for(size_t i = startingIndex; i < stringLength; i++)
        resultNum = (resultNum * 10) + (string[i] - '0');
    return (negative) ? (-1 * resultNum) : resultNum;
}

static bool check_valid_ignoreNaNWindows(char *string, bool *result) {
    const size_t stringLength = strlen(string);

    if(stringLength == strlen(VALID_IGNORENANWINDOWS_TRUE_STR)) {
        bool success = true;
        for(size_t i = 0; i < stringLength; i++) {
            if(string[i] != VALID_IGNORENANWINDOWS_TRUE_STR[i]) {
                success = false;
                break;
            }
        }
        if(success) {
            *result = VALID_IGNORENANWINDOWS_TRUE;
            return true;
        }
    }

    if(stringLength == strlen(VALID_IGNORENANWINDOWS_FALSE_STR)) {
        for(size_t i = 0; i < stringLength; i++) {
            if(string[i] != VALID_IGNORENANWINDOWS_FALSE_STR[i])
                return false;
        }

        *result = VALID_IGNORENANWINDOWS_FALSE;
        return true;
    }

    return false;
}

static bool benchmark_start(size_t length, size_t nanValues, size_t infValues, double lowestValue,
    double highestValue, size_t windowSize, size_t steps, bool ignoreNaNWindows) {
    if(length == 0)
        return false;
    if((nanValues > length) || (infValues > length) || ((nanValues + infValues) > length))
        return false;
    if(lowestValue >= highestValue)
        return false;

    double *inputSequence = (double* ) malloc(length * sizeof(double));
    if(inputSequence == NULL)
        return false;
    test_array_init(length, lowestValue, highestValue, inputSequence);

    const size_t spcNumbersCombNum = (nanValues + infValues);
    if(spcNumbersCombNum > length) {
        free(inputSequence);
        inputSequence = NULL;
        return false;
    }

    if(spcNumbersCombNum > 0) {
        size_t *spcNumbersIndizesArray = (size_t* ) malloc(length * sizeof(size_t));
        if(spcNumbersIndizesArray == NULL) {
            free(inputSequence);
            inputSequence = NULL;
            return false;
        }

        fill_and_shuffle_spc_number_indizes_array(length, spcNumbersIndizesArray);
        size_t currentIndex = 0;
        if(nanValues > 0)
            test_array_init_random_nans(inputSequence, &currentIndex, nanValues, spcNumbersIndizesArray);

        if(infValues > 0) {
            const size_t posInfNums = (infValues / 2);
            test_array_init_random_posinfs(inputSequence, &currentIndex, posInfNums, spcNumbersIndizesArray);
            test_array_init_random_neginfs(inputSequence, &currentIndex, (infValues - posInfNums),
                                            spcNumbersIndizesArray);
        }

        free(spcNumbersIndizesArray);
        spcNumbersIndizesArray = NULL;
    }


    double *outputArray = NULL;
    size_t outputArrayLength = 0;
    if(!result_array_init(length, windowSize, steps, &outputArrayLength, &outputArray))
        return false;

    struct timespec start, end, result;
    clock_gettime(CLOCK_MONOTONIC, &start);
    const bool success = sliding_medianwindow(inputSequence, length, windowSize,
        steps, ignoreNaNWindows, outputArray);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if(success) {
        difference_time_specs(&start, &end, &result);
        const double timeTaken = (result.tv_sec + (result.tv_nsec / 1000000000.0));
        printf("Time taken: %f\n", timeTaken);
    }

    free(inputSequence);
    inputSequence = NULL;
    free(outputArray);
    outputArray = NULL;
    return success;
}

static void test_array_init(size_t length, double lowestValue, double highestValue, double *dest) {
    for(size_t i = 0; i < length; i++) {
        const double v = (lowestValue + (highestValue - lowestValue) * ((double) rand() / (double) RAND_MAX));
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

static void difference_time_specs(struct timespec *spec1, struct timespec *spec2, struct timespec *result) {
    int64_t calculatedSeconds = (spec2->tv_sec - spec1->tv_sec);
    long calculatedNanoseconds = (spec2->tv_nsec - spec1->tv_nsec);
    if(calculatedNanoseconds < 0) {
        result->tv_sec = calculatedSeconds - 1;
        result->tv_nsec = calculatedNanoseconds + 1000000000;
    } else {
        result->tv_sec = calculatedSeconds;
        result->tv_nsec = calculatedNanoseconds;
    }
}
