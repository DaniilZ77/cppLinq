#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "linq.hpp"

using linq::from;

int main() {
    // smoke tests
    system("g++ -std=c++17 -pedantic -Wall -Wextra -Werror ../src/minimal_smoke_test.cpp -I../include -o minimal_smoke_test && ./minimal_smoke_test && echo OK");
    system("g++ -std=c++17 -pedantic -Wall -Wextra -Werror ../src/smoke_test.cpp -I../include -o smoke_test && ./smoke_test && echo OK");
    remove("minimal_smoke_test");
    remove("smoke_test");

    // doctest tests
    doctest::Context context;
    context.setOption("no-breaks", true);
    int res = context.run();
    return res;
}

TEST_CASE("linq::from test") {
    std::vector<int> expected = {1, 2, 3, 4};
    auto it = expected.begin();

    std::vector<int> actualVec = {1, 2, 3, 4};
    auto actual = from(actualVec.begin(), actualVec.end());

    while (actual) {
        CHECK(*it == *actual);
        ++it; ++actual;
    }
}

TEST_CASE("linq::impl::enumerator::to_vector test") {
    std::vector<int> expected = {4, 3, 2, 1};

    std::vector<int> actualVec = {4, 3, 2, 1};
    auto actual = from(actualVec.begin(), actualVec.end())
            .to_vector();

    CHECK(actual == expected);
}

TEST_SUITE("take tests") {

    TEST_CASE("linq::impl::enumerator::take and linq::impl::take_enumerator test") {
        std::vector<int> expected = {1, 2, 3, 4};
        int n = static_cast<int>(expected.size());

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .take(n)
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::take and linq::impl::take_enumerator test negative") {
        std::vector<int> expected = {};
        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .take(INT_MIN)
                .to_vector();
        CHECK(actual == expected);
    }

    TEST_CASE("linq::impl::enumerator::take and linq::impl::take_enumerator test big positive") {
        std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8};
        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .take(INT_MAX)
                .to_vector();
        CHECK(actual == expected);
    }

}

TEST_SUITE("where tests") {

    TEST_CASE("linq::impl::enumerator::where and linq::impl::where_enumerator test") {
        std::vector<int> expected = {2, 4, 6, 8};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .where([](int x){ return x % 2 == 0; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::where and linq::impl::where_enumerator test empty expected") {
        std::vector<int> expected = {};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .where([](int x){ return x % 2 == 2; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::where_neq test") {
        std::vector<int> expected = {1, 3, 4, 5, 6, 7, 8};

        std::vector<int> actualVec = {1, 2, 3, 2, 4, 2, 5, 2, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .where_neq(2)
                .to_vector();

        CHECK(expected == actual);
    }

}

TEST_SUITE("select tests") {

    TEST_CASE("linq::impl::enumerator::select test with same type") {
        std::vector<int> expected = {1, 4, 9, 16};

        std::vector<int> actualVec = {1, 2, 3, 4};
        auto actual = from(actualVec.begin(), actualVec.end())
                .select([](int x) { return x * x; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::select test with another type") {
        std::vector<double> expected = {0.5, 1, 1.5, 2};

        std::vector<int> actualVec = {1, 2, 3, 4};
        auto actual = from(actualVec.begin(), actualVec.end())
                .select<double>([](double x) { return x / 2; })
                .to_vector();

        CHECK(expected == actual);
    }

}

TEST_CASE("linq::impl::enumerator::drop and linq::impl::drop_enumerator test") {
    std::vector<int> expected = {1, 2, 3, 4, 5};

    std::vector<int> actualVec = {100, 100, 1, 2, 3, 4, 5};
    auto actual = from(actualVec.begin(), actualVec.end())
            .drop(2)
            .to_vector();

    CHECK(expected == actual);
}

TEST_SUITE("drop tests") {

    TEST_CASE("linq::impl::enumerator::drop and linq::impl::drop_enumerator test negative") {
        std::vector<int> expected = {100, 100, 1, 2, 3, 4, 5};

        std::vector<int> actualVec = {100, 100, 1, 2, 3, 4, 5};
        auto actual = from(actualVec.begin(), actualVec.end())
                .drop(INT_MIN)
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::drop and linq::impl::drop_enumerator test big positive") {
        std::vector<int> expected = {};

        std::vector<int> actualVec = {100, 100, 1, 2, 3, 4, 5};
        auto actual = from(actualVec.begin(), actualVec.end())
                .drop(INT_MAX)
                .to_vector();

        CHECK(expected == actual);
    }

}

TEST_SUITE("until tests") {

    TEST_CASE("linq::impl::enumerator::until and linq::impl::until_enumerator test") {
        std::vector<int> expected = {1, 2, 3, 4};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .until([](int x){ return x == 5; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::until and linq::impl::until_enumerator test empty expected") {
        std::vector<int> expected = {};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .until([](int x){ return x == 1; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::until and linq::impl::until_enumerator test original vector expected") {
        std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .until([](int x){ return x == -1; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("linq::impl::enumerator::until_eq test") {
        std::vector<int> expected = {1, 2, 3, 2, 4, 2, 5, 2};

        std::vector<int> actualVec = {1, 2, 3, 2, 4, 2, 5, 2, 6, 7, 8};
        auto actual = from(actualVec.begin(), actualVec.end())
                .until_eq(6)
                .to_vector();

        CHECK(expected == actual);
    }

}

TEST_SUITE("combined tests") {

    TEST_CASE("combined test 1") {
        std::vector<int> expected = {3, 6, 9};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        auto actual = from(actualVec.begin(), actualVec.end())
                .drop(2)
                .take(8)
                .where([](int x) {return x % 3 == 0; })
                .to_vector();

        CHECK(expected == actual);
    }

    TEST_CASE("combined test 2") {
        std::vector<int> expected = {9, 12, 15, 18, 21, 24, 27, 30};

        std::vector<int> actualVec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        auto actual = from(actualVec.begin(), actualVec.end())
                .drop(2)
                .take(8)
                .select([](int x) { return x * 3; })
                .where([](int x) {return x % 3 == 0; })
                .to_vector();

        CHECK(expected == actual);
    }

}

TEST_SUITE("copying test") {
    class Person {
    public:
        static int Copies;
        Person() {
            ++Copies;
        }
        Person(const Person&) {
            ++Copies;
        }
        Person(Person&&) = default;

        bool operator!=(const Person&) const {
            return true;
        }
    };

    int Person::Copies = 0;

    TEST_CASE("copy constructor test 1") {
        Person::Copies = 0;

        std::vector<Person> data(10);
        from(data.begin(), data.end())
        .where([](const Person&) { return true; })
        .to_vector();

        CHECK(Person::Copies == data.size() * 2);
    }

    TEST_CASE("copy constructor test 2") {
        Person::Copies = 0;

        std::vector<Person> data(10);
        from(data.begin(), data.end())
        .where_neq(Person())
        .to_vector();

        CHECK(Person::Copies == data.size() * 2 + 1);
    }

    class func {
    public:
        static int Copies;
        bool operator()(int a) {
            return a % 2 == 0;
        }
        func() {
            ++Copies;
        }
        func(const func&) {
            ++Copies;
        }
        func(func&&) = default;
    };

    int func::Copies = 0;

    TEST_CASE("copy constructor test 3") {
        func::Copies = 0;

        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8};

        from(data.begin(), data.end())
        .where(func())
        .to_vector();

        CHECK(func::Copies == 1);
    }
}