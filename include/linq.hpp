#pragma once

#include <utility>
#include <vector>
#include <functional>

namespace linq {
    namespace impl {
        template<typename T, typename Iter>
        class range_enumerator;

        template<typename T>
        class drop_enumerator;

        template<typename T, typename U, typename F>
        class select_enumerator;

        template<typename T, typename F>
        class until_enumerator;

        template<class T>
        class take_enumerator;

        template<typename T, typename F>
        class where_enumerator;

        template<typename T>
        class enumerator {
        public:
            virtual enumerator &operator++() = 0;

            virtual explicit operator bool() = 0;

            virtual const T &operator*() const = 0;

            std::vector<T> to_vector() {
                std::vector<T> result;
                while (*this) {
                    result.push_back(std::move(**this));
                    ++(*this);
                }
                return result;
            }

            auto take(int amount) {
                return take_enumerator<T>(*this, amount);
            }

            auto drop(int count) {
                return drop_enumerator<T>(*this, count);
            }

            template<typename U = T, typename F>
            auto select(F &&func) {
                return select_enumerator<U, T, F>(*this, std::move(func));
            }

            template<typename F>
            auto where(F &&func) {
                return where_enumerator<T, F>(*this, std::move(func));
            }

            auto where_neq(const T &predicateArg) {
                auto predicateNeq = [&predicateArg](const T &element) {
                    return element != predicateArg;
                };
                return where_enumerator<T, decltype(predicateNeq)>(*this, std::move(predicateNeq));
            }

            template<typename F>
            auto until(F &&func) {
                return until_enumerator<T, F>(*this, std::move(func));
            }

            auto until_eq(const T &predicateArg) {
                auto predicateEq = [&predicateArg](const T &element) {
                    return element == predicateArg;
                };
                return until_enumerator<T, decltype(predicateEq)>(*this, std::move(predicateEq));
            }

            template<typename Iter>
            void copy_to(Iter it) {
                while (*this) {
                    *it = std::move(**this);
                    ++(*this);
                    ++it;
                }
            }
        };

        template<class T, class Iter>
        class range_enumerator : public enumerator<T> {
        public:
            range_enumerator(Iter beg, Iter end) : beg_(beg), end_(end) {}

            range_enumerator &operator++() override {
                ++beg_;
                return *this;
            }

            explicit operator bool() override {
                return beg_ != end_;
            }

            const T &operator*() const override {
                return *beg_;
            }

        private:
            Iter beg_;
            Iter end_;
        };

        template<class T>
        class take_enumerator : public enumerator<T> {
        private:
            enumerator<T> &parent_;
            int amount_remaining_;
        public:
            take_enumerator(enumerator<T> &parent, int amount) :
                    parent_(parent), amount_remaining_(amount) {}

            take_enumerator &operator++() override {
                --amount_remaining_;
                ++parent_;
                return *this;
            }

            explicit operator bool() override {
                return amount_remaining_ > 0 && static_cast<bool>(parent_);
            }

            const T &operator*() const override {
                return *parent_;
            }
        };

        template<typename T>
        class drop_enumerator : public enumerator<T> {
        public:
            drop_enumerator(enumerator<T> &parent, int count) : parent_(parent), count_to_drop_(count) {}

            drop_enumerator &operator++() override {
                ++parent_;
                return *this;
            }

            explicit operator bool() override {
                drop();
                return static_cast<bool>(parent_);
            }

            const T &operator*() const override {
                return *parent_;
            }

        private:
            inline void drop() {
                while (count_to_drop_ > 0 && static_cast<bool>(parent_)) {
                    --count_to_drop_;
                    ++parent_;
                }
            }

        private:
            enumerator<T> &parent_;
            int count_to_drop_;
        };

        template<typename T, typename F>
        class where_enumerator : public enumerator<T> {
        public:
            where_enumerator(enumerator<T> &parent, F &&predicate) : parent_(parent),
                                                                     predicate_(std::move(predicate)) {}

            where_enumerator<T, F> &operator++() override {
                ++parent_;
                return *this;
            }

            explicit operator bool() override {
                return iterate_while_false();
            }

            const T &operator*() const override {
                return *cachedValue_;
            }

        private:
            inline bool iterate_while_false() {
                bool cachedBool;
                while ((cachedBool = static_cast<bool>(parent_)) && !predicate_(*(cachedValue_ = &(*parent_))))
                    ++parent_;
                return cachedBool;
            }

        private:
            enumerator<T> &parent_;
            F predicate_;
            const T *cachedValue_ = nullptr;
        };

        template<typename T, typename U, typename F>
        class select_enumerator : public enumerator<T> {
        public:
            select_enumerator(enumerator<U> &parent, F &&func) : parent_(parent), func_(std::move(func)) {}

            select_enumerator<T, U, F> &operator++() override {
                ++parent_;
                return *this;
            }

            explicit operator bool() override {
                return static_cast<bool>(parent_);
            }

            const T &operator*() const override {
                return cachedValue_ = func_(*parent_);
            }

        private:
            enumerator<U> &parent_;
            F func_;
            mutable T cachedValue_;
        };

        template<typename T, typename F>
        class until_enumerator : public enumerator<T> {
        public:
            until_enumerator(enumerator<T> &parent, F &&predicate) : parent_(parent),
                                                                     predicate_(std::move(predicate)) {}

            until_enumerator &operator++() override {
                ++parent_;
                return *this;
            }

            explicit operator bool() override {
                return static_cast<bool>(parent_) && !predicate_(*(cachedValue_ = &(*parent_)));
            }

            const T &operator*() const override {
                return *cachedValue_;
            }

        private:
            enumerator<T> &parent_;
            F predicate_;
            const T *cachedValue_ = nullptr;
        };
    } // namespace impl

    template<typename It>
    auto from(It begin, It end) {
        return impl::range_enumerator<typename std::iterator_traits<It>::value_type, It>(begin, end);
    }

} // namespace linq
