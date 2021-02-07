#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>

namespace MjB {

template <typename T1, typename T2>
std::enable_if_t<std::is_floating_point_v<T1> && std::is_floating_point_v<T2>,
                 bool>
is_equal_f(T1 const& a, T2 const& b) {
    return std::abs(a - b) < 0.00001;
}

template <typename P, size_t N>
class vec {

  protected:
    // a little sanity checking
    static_assert(N >= 1);
    static_assert(std::is_arithmetic_v<P>);

    std::array<P, N> storage_; /**< hold the values in our our vector*/

    /**************************************************************************/
    /**
     * @brief Helper method that applies a given 'function like object' to
     *        every member of the vector
     *
     * Note that order of execution is not fundementally guaranteed here
     *
     * @param function the function to apply to each vector element
     *
     */
    template <typename FN, size_t index = N - 1>
    void apply(FN const& function) {
        function(storage_[index], index);
        if constexpr (index > 0) {
            apply<FN, index - 1>(function);
        }
    }

    /**************************************************************************/
    /**
     * @brief Helper method that applies a given 'function-like object' to
     *        every member of the vector (const version)
     *
     * Note that order of execution is not fundementally guaranteed here
     *
     * @param function the function to apply to each vector element
     *
     */
    template <typename FN, size_t index = N - 1>
    void apply(FN const& function) const {
        function(storage_[index], index);
        if constexpr (index > 0) {
            apply<FN, index - 1>(function);
        }
    }

    /**************************************************************************/
    /**
     * @brief Helper method that applies a given 'function-like object' to
     *        every member of the vector
     *
     * Note that order of execution is not fundementally guaranteed here
     *
     * @param function the function to apply to each vector element
     *
     */
    template <typename FN, size_t index = N - 1>
    void apply_other(vec<P, N> const& other, FN const& function) {
        function(storage_[index], other.storage_[index], index);
        if constexpr (index > 0) {
            apply_other<FN, index - 1>(other, function);
        }
    }

    /**************************************************************************/
    /**
     * @brief Helper method that applies a given 'function-like object' to
     *        every member of the vector (const version)
     *
     * Note that order of execution is not fundementally guaranteed here
     *
     * @param function the function to apply to each vector element
     *
     */
    template <typename FN, size_t index = N - 1>
    void apply_other(vec<P, N> const& other, FN const& function) const {
        function(storage_[index], other.storage_[index], index);
        if constexpr (index > 0) {
            apply_other<FN, index - 1>(other, function);
        }
    }

  public:
    /**************************************************************************/
    /**
     * @brief Default constructor
     *
     * By default, initialize all vectors to zero
     *
     */
    constexpr vec() noexcept { storage_.fill(P(0)); }

    /**************************************************************************/
    /**
     * @brief Uniform initializer
     *
     * @param value value for each component
     *
     */
    template <typename P2,
              std::enable_if_t<std::is_convertible_v<P2, P>, int> = 0>
    constexpr vec(P2 value) noexcept {
        storage_.fill(static_cast<P>(value));
    }

    /**************************************************************************/
    /**
     * @brief Component initializer
     *
     * @param ps N components
     *
     */
    template <typename... Ps,
              std::enable_if_t<sizeof...(Ps) == N &&
                                   (... && std::is_convertible_v<Ps, P>),
                               int> = 0>
    constexpr vec(Ps... ps) noexcept : storage_{static_cast<P>(ps)...} {}

    /**************************************************************************/
    /**
     * @brief Construct from std::array
     *
     * @param source_array array to initialize from
     *
     */
    constexpr vec(std::array<P, N> const& source_array) noexcept :
        storage_(source_array) {}

    /**************************************************************************/
    /**
     * @brief Move construct from std::array
     *
     * @param source_array array r-value reference to initialize from
     *
     */
    constexpr vec(std::array<P, N>&& source_array) noexcept :
        storage_(std::move(source_array)) {}

    /**************************************************************************/
    /**
     * @brief Copy constructor
     *
     * @param other source for copy
     *
     */
    constexpr vec(vec<P, N> const& other) noexcept = default;

    /**************************************************************************/
    /**
     * @brief Move constructor
     *
     * @param other source for copy
     *
     */
    constexpr vec(vec<P, N>&& other) = default;

    /**************************************************************************/
    /**
     * @brief Assignment operator
     *
     * @param other source for assignment
     *
     */
    constexpr vec<P, N>& operator=(vec<P, N> const& other) {
        storage_ = other.storage_;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief Equality operator
     *
     * Note that this uses is_equal_f if the vector is floating point valued to
     * handle floating point vagary
     *
     * @param other second operand for comparison
     *
     */
    constexpr bool operator==(vec<P, N> const& other) const {
        bool is_equal = true;

        // TODO(mjb): benchmark to see if checking the aggregate comparison is
        // indeed faster than implementing an 'early out'
        apply_other(other, [&is_equal](P const& this_value,
                                       P const& other_value,
                                       size_t const index) {
            if constexpr (std::is_floating_point_v<P>) {
                is_equal = is_equal and is_equal_f(this_value, other_value);
            } else {
                return is_equal = is_equal and (this_value == other_value);
            }
        });

        return is_equal;
    }

    /**************************************************************************/
    /**
     * @brief Addition operator
     *
     * @param other second term for addition
     *
     */
    constexpr vec<P, N> operator+(vec<P, N> const& other) const {
        std::array<P, N> new_storage;

        apply_other(other,
                    [&new_storage](P const& this_value, P const& other_value,
                                   size_t const index) {
                        new_storage[index] = this_value + other_value;
                    });

        return new_storage;
    }

    /**************************************************************************/
    /**
     * @brief Subraction operator
     *
     * @param other second term for subtraction
     *
     */
    constexpr vec<P, N> operator-(vec<P, N> const& other) const {
        std::array<P, N> new_storage;

        apply_other(other,
                    [&new_storage](P const& this_value, P const& other_value,
                                   size_t const index) {
                        new_storage[index] = this_value - other_value;
                    });

        return new_storage;
    }

    /**************************************************************************/
    /**
     * @brief Negation operator
     *
     * @param other second term for addition
     *
     */
    constexpr vec<P, N> operator-() const {
        std::array<P, N> new_storage;

        apply([&new_storage](P const& this_value, size_t const index) {
            new_storage[index] = this_value * -1;
        });

        return new_storage;
    }

    /**************************************************************************/
    /**
     * @brief Scalar multiplication operator
     *
     * @param value scalar for multiplication
     *
     */
    template <typename S>
    constexpr std::enable_if_t<std::is_convertible_v<S, P>, vec<P, N>>
    operator*(S const& value) const {
        std::array<P, N> new_storage;

        apply([&new_storage, &value](P const& this_value, size_t const index) {
            new_storage[index] = static_cast<P>(this_value) * value;
        });

        return new_storage;
    }

    /**************************************************************************/
    /**
     * @brief Scalar division operator
     *
     * @param value scalar for division
     *
     */
    template <typename S>
    constexpr std::enable_if_t<std::is_convertible_v<S, P>, vec<P, N>>
    operator/(S value) const {
        std::array<P, N> new_storage;

        apply([&new_storage, &value](P const& this_value, size_t const index) {
            new_storage[index] = static_cast<P>(this_value) / value;
        });

        return new_storage;
    }

    /**************************************************************************/
    /**
     * @brief Subscript operator
     *
     * @param index index to retreive
     *
     */
    constexpr P& operator[](size_t const index) { return storage_[index]; }

    /**************************************************************************/
    /**
     * @brief Subscript operator (const version)
     *
     * @param index index to retreive
     *
     */
    constexpr P const& operator[](size_t const index) const {
        return storage_[index];
    }
};

// standard streaming operator overloading for the vector
template <typename P, size_t N>
std::ostream& operator<<(std::ostream& os, vec<P, N> const& value) {

    os << "[ ";
    for (size_t index = 0; index < N; ++index) {
        os << value[index];
        if (index != N - 1) {
            os << ", ";
        }
    }
    os << " ]";
    return os;
}

template <typename P>
class vec4 : public vec<P, 4> {

  public:
    // alias the components for clearer code
    P& x = vec<P, 4>::storage_[0];
    P& y = vec<P, 4>::storage_[1];
    P& z = vec<P, 4>::storage_[2];
    P& w = vec<P, 4>::storage_[3];

    using vec<P, 4>::vec;

    // Note: I don't really like these, but I'm following a tutorial for now
    constexpr bool is_point() const { return w == 1.0; }
    constexpr bool is_vector() const { return w == 0.0; }

    // determine the magnitude
    float magnitude() { return std::sqrt(x * x + y * y + z * z + w * w); }

    // produce a normalized version
    // TODO(mjb): move to base class?
    vec4<P> normalize() {
        auto const mag = magnitude();
        // TODO(mjb): determine if the w/mag is really necessary

        return {x / mag, y / mag, z / mag, w / mag};
    }

    // produce the dot product with a given vector
    P dot(vec4<P> const& other) {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // produce the cross product with a given vector
    vec4<P> cross(vec4<P> const& other) {
        return {y * other.z - z * other.y, z * other.x - x * other.z,
                x * other.y - y * other.x, P(0)};
    }
};

using vec4f = vec4<float>;

// produce a float valued 'point'
inline vec4f point(float const& x, float const& y, float const& z) {
    return vec4f(x, y, z, 1.0);
}

// produce a float valued 'vector'
inline vec4f vector(float const& x, float const& y, float const& z) {
    return vec4f(x, y, z, 0.0);
}

} // namespace MjB