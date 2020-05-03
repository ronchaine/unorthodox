#include "doctest.h"

#include <unorthodox/dynamic_array.hpp>
#include <vector>

#include <iostream>

TEST_SUITE("Dynamic Array") {

    struct empty_struct{};

    TEST_CASE_TEMPLATE("General type requirements", T, int, float, double, uint64_t, empty_struct) {
        REQUIRE(sizeof(unorthodox::dynamic_array<T>{}) <= 32);
    }

    TEST_CASE("Construction and destruction") {
        SUBCASE("Initialiser list, SBO") {
            unorthodox::dynamic_array<int> sbo_array{0, 5, 7, 12};
            REQUIRE(sbo_array.size() == 4);

            REQUIRE((*(sbo_array.data() + 0)) == 0);
            REQUIRE((*(sbo_array.data() + 1)) == 5);
            REQUIRE((*(sbo_array.data() + 2)) == 7);
            REQUIRE((*(sbo_array.data() + 3)) == 12);
        }

        SUBCASE("Initialiser list") {
            unorthodox::dynamic_array<int> array{7, 4, 2, 8, 5, 17, 9, 42, 16, 25, 9};
            REQUIRE(array.size() == 11);
            
            REQUIRE((*(array.data() + 0)) == 7);
            REQUIRE((*(array.data() + 1)) == 4);
            REQUIRE((*(array.data() + 2)) == 2);
            REQUIRE((*(array.data() + 3)) == 8);
            REQUIRE((*(array.data() + 4)) == 5);
            REQUIRE((*(array.data() + 5)) == 17);
            REQUIRE((*(array.data() + 6)) == 9);
            REQUIRE((*(array.data() + 7)) == 42);
            REQUIRE((*(array.data() + 8)) == 16);
            REQUIRE((*(array.data() + 9)) == 25);
            REQUIRE((*(array.data() + 10)) == 9);
        }

        unorthodox::dynamic_array<int> array_neg{-1, -2, -3, -4};
        unorthodox::dynamic_array<int> array_pos{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

        SUBCASE("Copy, SBO") {
            auto array2(array_neg);

            REQUIRE(array2.size() == 4);

            REQUIRE((*(array2.data() + 0)) == -1);
            REQUIRE((*(array2.data() + 1)) == -2);
            REQUIRE((*(array2.data() + 2)) == -3);
            REQUIRE((*(array2.data() + 3)) == -4);
        }

        SUBCASE("Copy") {
            auto array2(array_pos);

            REQUIRE(array2.size() == 16);
            for (int i = 0; i < 16; ++i)
                REQUIRE((*(array2.data() + i)) == i + 1);
        }

        SUBCASE("Move, SBO") {
            auto moved_array(array_neg);

            REQUIRE(moved_array.size() == 4);

            REQUIRE((*(moved_array.data() + 0)) == -1);
            REQUIRE((*(moved_array.data() + 1)) == -2);
            REQUIRE((*(moved_array.data() + 2)) == -3);
            REQUIRE((*(moved_array.data() + 3)) == -4);
        }

        SUBCASE("Move") {
            auto moved_array(array_pos);

            REQUIRE(moved_array.size() == 16);
            for (int i = 0; i < 16; ++i)
                REQUIRE((*(moved_array.data() + i)) == i + 1);
        }

        std::vector<int> small_vector_source = {0, 1, 2, 3, 4};
        const std::vector<int> const_small_vector_source = {0, -1, -2, -3, -4};
        
        std::vector<int> vector_source = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        const std::vector<int> const_vector_source = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16};

        SUBCASE("From a range") {
            unorthodox::dynamic_array<int> full_array_small(array_neg.begin(), array_neg.end());

            unorthodox::dynamic_array<int> full_array_from_small_vector(small_vector_source.begin(), small_vector_source.end());
            unorthodox::dynamic_array<int> full_array_from_vector(vector_source.begin(), vector_source.end());
            
            const unorthodox::dynamic_array<int> const_full_array_from_small_vector(small_vector_source.begin(), small_vector_source.end());
            const unorthodox::dynamic_array<int> const_full_array_from_vector(vector_source.begin(), vector_source.end());

            unorthodox::dynamic_array<int> full_array_from_const_source(const_vector_source.begin(), const_vector_source.end());

            unorthodox::dynamic_array<int> clipped_begin_array_small(array_neg.begin() + 2, array_neg.end());
            unorthodox::dynamic_array<int> clipped_ends_array_small(array_neg.begin() + 1, array_neg.end() - 1);

            REQUIRE(full_array_small.size() == 4);
            REQUIRE(full_array_from_small_vector.size() == 5);
            REQUIRE(const_full_array_from_small_vector.size() == 5);
            REQUIRE(full_array_from_vector.size() == 17);
            REQUIRE(const_full_array_from_vector.size() == 17);
            REQUIRE(full_array_from_const_source.size() == 17);
            
            REQUIRE(clipped_begin_array_small.size() == 2);
            REQUIRE(clipped_ends_array_small.size() == 2);

            for (unsigned int i = 0; i < array_neg.size(); ++i)
                REQUIRE((*(full_array_small.data() + i )) == -(i + 1));

            for (unsigned int i = 0; i < small_vector_source.size(); ++i)
            {
                REQUIRE((*(full_array_from_small_vector.data() + i)) == i);
                REQUIRE((*(const_full_array_from_small_vector.data() + i)) == i);
            }

            for (int i = 0; i < static_cast<int>(vector_source.size()); ++i)
            {
                REQUIRE((*(full_array_from_vector.data() + i)) == i);
                REQUIRE((*(const_full_array_from_vector.data() + i)) == i);
                REQUIRE((*(full_array_from_const_source.data() + i)) == -i);
            }

            REQUIRE((*(clipped_begin_array_small.data() + 0)) == -3);
            REQUIRE((*(clipped_begin_array_small.data() + 1)) == -4);
            REQUIRE((*(clipped_ends_array_small.data() + 0)) == -2);
            REQUIRE((*(clipped_ends_array_small.data() + 1)) == -3);
        }

        SUBCASE("From a container") {
            std::vector<int> in_vector = {0, 5, 7, 12};

            auto a = static_cast<unorthodox::dynamic_array<int>>(in_vector);
        }
    }
    
    TEST_CASE("Access") {
        unorthodox::dynamic_array<float> array {7.0, 4.2, 1.3, 5.8, 9.451};

        SUBCASE("[] operator") {
            REQUIRE(array[0] == 7.0f);
            REQUIRE(array[4] == 9.451f);
            REQUIRE(array[1] == 4.2f);
            REQUIRE(array[3] == 5.8f);
            REQUIRE(array[2] == 1.3f);
        }
        SUBCASE("Named access") {
            REQUIRE(array.front() == 7.0f);
            REQUIRE(array.back() == 9.451f);
            REQUIRE(array.first() == 7.0f);
            REQUIRE(array.last() == 9.451f);
        }
    }
}
