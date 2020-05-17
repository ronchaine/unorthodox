#include "doctest.h"

#include <unorthodox/dynamic_array.hpp>
#include <vector>

#include <iostream>

struct tester
{
    inline static int constructors = 0;
    inline static int destructors = 0;

    inline static int moves = 0;

//    tester::reset();
    int value = 0;

    static void reset()
    {
        constructors = 0;
        destructors = 0;
        moves = 0;
    }

    tester() { constructors++; value = 1; }
    ~tester() { destructors++; }

    tester(const tester& r) { constructors++; value = r.value; }
    tester(tester&& r) { constructors++; moves++; value = r.value; }

    tester& operator=(const tester& r) = default;
    tester& operator=(tester&& r) = default;
};

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

    TEST_CASE("Iterators") {
        unorthodox::dynamic_array<int> integers = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        size_t n;

        SUBCASE("begin() / end()") {
            n = 0;
            for (unorthodox::dynamic_array<int>::iterator it = integers.begin(); it != integers.end(); ++it)
                REQUIRE(*it ==n++);
           
            n = 0;
            for (unorthodox::dynamic_array<int>::const_iterator it = integers.begin(); it != integers.end(); ++it)
                REQUIRE(*it ==n++);
        }
        SUBCASE("cbegin() / cend()") {
            n = 0;
            for (unorthodox::dynamic_array<int>::const_iterator it = integers.cbegin(); it != integers.cend(); ++it)
                REQUIRE(*it ==n++);
        }
        SUBCASE("reverse iterators") {
            n = 0;
            for (unorthodox::dynamic_array<int>::reverse_iterator it = integers.rbegin(); it != integers.rend(); ++it)
                REQUIRE(*it == integers.size() - ++n);

            n = 0;
            for (unorthodox::dynamic_array<int>::const_reverse_iterator it = integers.rbegin(); it != integers.rend(); ++it)
                REQUIRE(*it == integers.size() - ++n);

            n = 0;
            for (unorthodox::dynamic_array<int>::const_reverse_iterator it = integers.crbegin(); it != integers.crend(); ++it)
                REQUIRE(*it == integers.size() - ++n);
        }
    }

    TEST_CASE("Access") {
        unorthodox::dynamic_array<float> array {7.0, 4.2, 1.3, 5.8, 9.451};

        SUBCASE("[] operator") {
            CHECK(array[0] == 7.0f);
            CHECK(array[4] == 9.451f);
            CHECK(array[1] == 4.2f);
            CHECK(array[3] == 5.8f);
            CHECK(array[2] == 1.3f);
        }
        SUBCASE("Named access") {
            CHECK(array.front() == 7.0f);
            CHECK(array.back() == 9.451f);
            CHECK(array.first() == 7.0f);
            CHECK(array.last() == 9.451f);
        }
    }

    TEST_CASE("Reserve") {
        unorthodox::dynamic_array<int> array = { 5, 8 };
        SUBCASE("Growing basics") {
            auto cap = array.capacity();
            array.reserve(++cap);
            REQUIRE(array.capacity() == cap);

            array.reserve(cap - 1);
            REQUIRE(array.capacity() == cap);
        }

        SUBCASE("Growing preserves elements") {
            auto rd = array.data();

            if (array.sbo_limit / sizeof(int)) {
                array.reserve(array.sbo_limit / sizeof(int) - 1);
                REQUIRE(array.data() == rd);
            }
            CHECK(array[0] == 5);
            CHECK(array[1] == 8);

            if (array.sbo_limit / sizeof(int)) {
                array.reserve(array.sbo_limit / sizeof(int) + 1);
                REQUIRE(array.capacity() == array.sbo_limit / sizeof(int) + 1);
                REQUIRE(array.data() != rd);
            }
            CHECK(array[0] == 5);
            CHECK(array[1] == 8);
        }
        
        SUBCASE("Types larger than SBO max") {
        }
    }

    TEST_CASE("Resizing POD type") {
        unorthodox::dynamic_array<int> array;
        const size_t size_below_sbo_limit = array.sbo_limit / sizeof(int) - 1;
        const size_t size_above_sbo_limit = array.sbo_limit / sizeof(int) + 1;
        SUBCASE("Upsizing - SBO") {
            array.resize(size_below_sbo_limit);
            REQUIRE(array.capacity() >= size_below_sbo_limit);
            REQUIRE(array.size() == size_below_sbo_limit);

            // values survive resizing over SBO limit
            if (array.capacity() <= size_below_sbo_limit)
            {
                for (size_t i = 0; i < size_below_sbo_limit; ++i)
                    array[i] = i;

                array.resize(size_above_sbo_limit);

                for (size_t i = 0; i < size_below_sbo_limit; ++i)
                    CHECK(array[i] == i);
            }
        }
        SUBCASE("Upsizing - no SBO") {
            array.resize(size_above_sbo_limit);
            REQUIRE(array.capacity() >= size_above_sbo_limit);
            REQUIRE(array.size() == size_above_sbo_limit);

            // New values get default-initialised
            for (size_t i = 0; i < size_above_sbo_limit; ++i)
                CHECK(array[i] == 0);

            // values are preserved by resize
            array[0] = 5;
            array[1] = 4;
            array[2] = 3;

            array[size_above_sbo_limit - 1] = 1;
            array[size_above_sbo_limit - 2] = 2;

            array.resize(size_above_sbo_limit * 2);
            CHECK(array[0] == 5);
            CHECK(array[1] == 4);
            CHECK(array[2] == 3);
            CHECK(array[size_above_sbo_limit - 1] == 1);
            CHECK(array[size_above_sbo_limit - 2] == 2);
        }

        SUBCASE("Downsizing") {
            array.resize(30);

            for (size_t i = 0; i < 30; ++i)
                array[i] = i;

            CHECK(array.capacity() == 30);
            CHECK(array.size() == 30);

            array.resize(10);
            CHECK(array.capacity() >= 10);
            CHECK(array.size() == 10);
            
            for (size_t i = 0; i < 10; ++i)
                CHECK(array[i] == i);
            
            array.resize(0);
            CHECK(array.size() == 0);
        }
    }

    TEST_CASE("Non-POD resizes") {
        SUBCASE("Non-SBO class") {
            using non_sboable_class = std::aligned_storage<unorthodox::dynamic_array<int>::sbo_limit + 1>::type;

            unorthodox::dynamic_array<non_sboable_class> array;
            CHECK(array.capacity() == 0);

            array.resize(1);
            CHECK(array.capacity() >= 1);
            CHECK(array.size() == 1);
        }
        SUBCASE("Basic construct / destruct") {
            tester::reset();
            const size_t small_resize = unorthodox::dynamic_array<tester>::sbo_limit / sizeof(tester) - 1;
            {
                unorthodox::dynamic_array<tester> array;
                array.resize(small_resize);
                CHECK(tester::constructors == small_resize);
            }
            CHECK(tester::destructors == small_resize);

            tester::reset();
            const size_t medium_resize = unorthodox::dynamic_array<tester>::sbo_limit / sizeof(tester);
            {
                unorthodox::dynamic_array<tester> array;
                array.resize(medium_resize);
                CHECK(tester::constructors == medium_resize);
            }
            CHECK(tester::destructors == medium_resize);

            tester::reset();
            const size_t large_resize = unorthodox::dynamic_array<tester>::sbo_limit / sizeof(tester) + 2;
            {
                unorthodox::dynamic_array<tester> array;
                array.resize(large_resize);
                CHECK(tester::constructors == large_resize);
            }
            CHECK(tester::destructors == large_resize);
        }
        SUBCASE("Downsizing") {
            tester::reset();
            unorthodox::dynamic_array<tester> array;

            array.resize(50);
            for (size_t i = 0; i < 50; ++i)
                array[i].value = i;

            CHECK(tester::constructors == 50);
            CHECK(tester::destructors == 0);
            for (size_t i = 0; i < 50; ++i)
                CHECK(array[i].value == i);

            tester::reset();
            array.resize(40);
            CHECK(tester::constructors == 0);
            CHECK(tester::destructors == 10);
            for (size_t i = 0; i < 40; ++i)
                CHECK(array[i].value == i);
            
            array.resize(60);
            CHECK(tester::constructors == 60);
            CHECK(tester::destructors == 10);
            for (size_t i = 0; i < 40; ++i)
                CHECK(array[i].value == i);
        }
        SUBCASE("clear") {
            tester::reset();
            unorthodox::dynamic_array<tester> array;

            array.resize(50);
            CHECK(tester::constructors == 50);
            CHECK(tester::destructors == 0);

            array.clear();
            CHECK(tester::destructors == 50);
        }
        SUBCASE("pop_back") {
            tester::reset();
            unorthodox::dynamic_array<tester> array;

            array.resize(20);
            CHECK(tester::constructors == 20);

            array.pop_back();
            CHECK(array.size() == 19);
            CHECK(tester::destructors == 1);
        }
    }

    TEST_CASE("erase") {
        unorthodox::dynamic_array<tester> array;
        array.resize(20);
        for (size_t i = 0; i < array.size(); ++i)
            array[i].value = i;
        CHECK(array.size() == 20);

        tester::reset();
        SUBCASE("First element") {
            auto iter = array.erase(array.begin());
            CHECK(array.size() == 19);
            CHECK(tester::destructors == 1);
            for (size_t i = 0; i < array.size(); ++i)
                CHECK(array[i].value == i + 1);

            CHECK(iter == array.begin());
        }
        tester::reset();
        SUBCASE("Last element") {
            auto iter = array.erase(array.end());
            CHECK(array.size() == 19);
            CHECK(tester::destructors == 1);
            for (size_t i = 0; i < array.size(); ++i)
                CHECK(array[i].value == i);

            CHECK(iter == array.end());
        }
        tester::reset();
        SUBCASE("Middle element") {
            auto iter = array.erase(array.begin() + 2);
            CHECK(array.size() == 19);
            CHECK(tester::destructors == 1);
            for (size_t i = 0; i < array.size(); ++i)
                if (i < 2)
                    CHECK(array[i].value == i);
                else
                    CHECK(array[i].value == i + 1);
            CHECK((*iter).value == 3);
        }
        SUBCASE("Even elements") {
            for (auto it = array.begin(); it != array.end();)
            {
                if ((*it).value % 2 == 0)
                    it = array.erase(it);
                else
                    ++it;
            }
            for (const auto& i : array)
                CHECK(i.value % 2 != 0);

            CHECK(array.size() == 10);
            CHECK(tester::destructors == 10);
        }

        SUBCASE("Range: Middle elements") {
            auto iter = array.erase(array.begin() + 1, array.end() - 1);
            CHECK(array.size() == 2);
            CHECK(tester::destructors == 18);
            CHECK(iter == array.end() - 1);
            CHECK(array[0].value == 0);
            CHECK(array[1].value == 19);
        }

        SUBCASE("Range: End elements") {
            auto iter = array.erase(array.begin() + 5, array.end());
            CHECK(tester::destructors == 15);
            CHECK(array.size() == 5);
            CHECK(iter == array.end());
            CHECK(array[0].value == 0);
        }
    }

    TEST_CASE("emplace back") {
        unorthodox::dynamic_array<tester> array;
        tester::reset();
        
        array.emplace_back();
        CHECK(array.capacity() >= 1);
        CHECK(array.size() == 1);
        CHECK(tester::constructors == 1);

        array.resize(array.capacity());
        REQUIRE(array.size() == array.capacity());

        tester new_value;
        new_value.value = 42;
        tester::reset();

        CHECK(array.emplace_back(new_value).value == 42);
    }

    TEST_CASE("emplace") {
        unorthodox::dynamic_array<tester> array;
        array.resize(20);
        for (size_t i = 0; i < array.size(); ++i)
            array[i].value = i;
        REQUIRE(array.size() == 20);

        tester new_value;
        new_value.value = 21;

        tester::reset();

        array.emplace(array.begin() + 2, new_value);
        CHECK(array.size() == 21);
        CHECK(array.capacity() >= 21);

        // RVO -> 0
        CHECK(tester::constructors - tester::moves <= 1);
        CHECK(array[2].value == 21);

        array.emplace(array.begin() + 2, std::move(new_value));
        CHECK(array.size() == 22);
        CHECK(array.capacity() >= 22);

        CHECK(tester::constructors - tester::moves <= 1);
        CHECK(array[2].value == 21);

        for (size_t i = 0; i < array.size(); ++i)
        {
            if (i < 2)
                CHECK(array[i].value == i);
            else if (i == 2)
                CHECK(array[i].value == 21);
            else if (i == 3)
                CHECK(array[i].value == 21);
            else
                CHECK(array[i].value == i - 2);
        }
    }

    TEST_CASE("insert") {
        unorthodox::dynamic_array<tester> array;
        array.resize(20);
        for (size_t i = 0; i < array.size(); ++i)
            array[i].value = i;
        REQUIRE(array.size() == 20);

        SUBCASE("Single insert, middle") {
            tester new_value;
            new_value.value = 21;

            tester::reset();

            array.insert(array.begin() + 2, new_value);
            CHECK(array.size() == 21);
            CHECK(tester::constructors - tester::moves == 1);
            CHECK(array[2].value == 21);
            for (size_t i = 0; i < array.size(); ++i)
            {
                if (i < 2)
                    CHECK(array[i].value == i);
                else if (i == 2)
                    CHECK(array[i].value == 21);
                else
                    CHECK(array[i].value == i - 1);
            }
        }

        SUBCASE("Insert multiple, middle") {
            tester new_value;
            new_value.value = 21;

            tester new_value2;
            new_value2.value = 22;

            tester new_value3;
            new_value3.value = 23;

            tester::reset();

            array.insert(array.begin() + 2, new_value, new_value2, new_value3);
            CHECK(array.size() == 23);
            CHECK(tester::constructors - tester::moves == 3);
            for (size_t i = 0; i < array.size(); ++i)
            {
                if (i < 2)
                    CHECK(array[i].value == i);
                else if (i == 2)
                    CHECK(array[i].value == 21);
                else if (i == 3)
                    CHECK(array[i].value == 22);
                else if (i == 4)
                    CHECK(array[i].value == 23);
                else
                    CHECK(array[i].value == i - 3);
            }
        }
    }
}
