//
// 3dmath.h
//
//
//  Created by Micah J Best on 2017-01-07.
//  Copyright © 2017 Micah J Best. All rights reserved.
//

// Basic 3d math primitives -- header only version

#include <array>
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>

#ifndef MJB_3DMATH
#define MJB_3DMATH

namespace MJB_3D {

// Define the fundamental floating point type we'll be using (float by default)
#ifndef MATH3DTYPE
#define MATH3DTYPE float
#endif
using FloatType = MATH3DTYPE;

/******
 *
 * Vector
 *
 */

/**************************************************************************/
/**
 * @name Vector
 * @brief basic vector type, we'll assume column vector
 *
 *
 *
 */
template <size_t SIZE>
struct Vector {
  private:
    inline void assignComponent(size_t pos){};

    template <typename... Args>
    inline void assignComponent(size_t pos, FloatType c, Args const&... args) {
        components[pos] = c;
        assignComponent(pos + 1, args...);
    }

  public:
    std::array<FloatType, SIZE> components;

    static const size_t size = SIZE;

    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     *
     */
    Vector() {}

    /**************************************************************************/
    /**
     * @brief  Constructor with component initialization
     *
     * @param components Array of components (designed for braced
     * initialization)
     *
     */
    template <typename... Args>
    Vector(FloatType c, Args const&... args) {
        static_assert(sizeof...(Args) + 1 == SIZE, "Wrong number of parameters "
                                                   "to vector constructor");
        assignComponent(0, c, args...);
    }

    /**************************************************************************/
    /**
     * @brief  List initializer constructor
     *
     * @param initializer The initializer list
     *
     */
    Vector(std::initializer_list<FloatType> initializer) {
        //         TODO: upgrade to C++14/17
        //         static_assert( initializer.size() == SIZE, "Wrong number of
        //         parameters "
        //                                                    "to vector
        //                                                    constructor" );
        size_t index = 0;
        for (auto const& c : initializer) {
            components[index] = c;
            index++;
        }
    }

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Vector to copy
     *
     */
    Vector(Vector<SIZE> const& other) : components(other.components) {}

    /**************************************************************************/
    /**
     * @brief  Add all the components
     *
     * @returns Sum of the components
     *
     */
    FloatType sum();

    /**************************************************************************/
    /**
     * @brief  Convenience operator for accessing the components
     *
     */
    FloatType& operator[](size_t index) { return components[index]; }

    /**************************************************************************/
    /**
     * @brief  Convenience operator for accessing the components (const version)
     *
     */
    FloatType const& operator[](size_t index) const {
        return components[index];
    }
};

/**************************************************************************/
/**
 * @brief  Utility class for applying operations to a vector without loops
 *
 */
template <size_t SIZE, typename OP, size_t N>
struct VectorOperation {

    template <typename... Args>
    inline void operator()(OP& op, Args&... args) {
        op(N, args...);
        VectorOperation<SIZE, OP, N - 1> adder;
        adder(args...);
    }

    template <typename... Args>
    inline void operator()(Args&... args) {
        OP op;
        op(N, args...);
        VectorOperation<SIZE, OP, N - 1> adder;
        adder(args...);
    }
};

template <size_t SIZE, typename OP>
struct VectorOperation<SIZE, OP, 0> {

    template <typename... Args>
    inline void operator()(OP& op, Args&... args) {
        op(0, args...);
    }

    template <typename... Args>
    inline void operator()(Args&... args) {
        OP op;
        op(0, args...);
    }
};

template <size_t SIZE>
struct VectorSum {
    FloatType total;
    inline void operator()(size_t n, Vector<SIZE> const& V) { total += V[n]; }
};

template <size_t SIZE>
FloatType Vector<SIZE>::sum() {
    VectorOperation<SIZE, VectorSum<SIZE>, SIZE - 1> operation;
    operation(*this);
    return operation.total;
}

template <size_t SIZE>
struct VectorPairwiseAdd {

    inline void operator()(size_t n, Vector<SIZE>& R, Vector<SIZE> const& A,
                           Vector<SIZE> const& B) {
        R.components[n] = A.components[n] + B.components[n];
    }
};

template <size_t SIZE>
struct VectorCompoundAdd {
    inline void operator()(size_t n, Vector<SIZE>& A, Vector<SIZE> const& B) {
        A.components[n] += B.components[n];
    }
};

template <size_t SIZE>
struct VectorPairwiseSubtract {
    inline void operator()(size_t n, Vector<SIZE>& R, Vector<SIZE> const& A,
                           Vector<SIZE> const& B) {
        R.components[n] = A.components[n] - B.components[n];
    }
};

template <size_t SIZE>
struct VectorCompoundSubtract {
    inline void operator()(size_t n, Vector<SIZE>& A, Vector<SIZE> const& B) {
        A.components[n] -= B.components[n];
    }
};

template <size_t SIZE>
struct VectorScalarMultiply {
    inline void operator()(size_t n, Vector<SIZE>& R, FloatType c,
                           Vector<SIZE> const& V) {
        R.components[n] = c * V.components[n];
    }
};

template <size_t SIZE>
struct VectorCompoundScalarMultiply {
    inline void operator()(size_t n, FloatType c, Vector<SIZE>& V) {
        V.components[n] = c * V.components[n];
    }
};

template <size_t SIZE>
struct VectorScalarDivide {
    inline void operator()(size_t n, Vector<SIZE>& R, FloatType c,
                           Vector<SIZE> const& V) {
        R.components[n] = V.components[n] / c;
    }
};

template <size_t SIZE>
struct VectorCompoundScalarDivide {
    inline void operator()(size_t n, FloatType c, Vector<SIZE>& V) {
        V.components[n] = V.components[n] / c;
    }
};

template <size_t SIZE>
Vector<SIZE> operator+(Vector<SIZE> const& A, Vector<SIZE> const& B) {
    Vector<SIZE> R;

    VectorOperation<SIZE, VectorPairwiseAdd<SIZE>, SIZE - 1> operation;
    operation(R, A, B);

    return R;
}

template <size_t SIZE>
Vector<SIZE>& operator+=(Vector<SIZE>& A, Vector<SIZE> const& B) {
    VectorOperation<SIZE, VectorCompoundAdd<SIZE>, SIZE - 1> operation;
    operation(A, B);
    return A;
}

template <size_t SIZE>
Vector<SIZE> operator-(Vector<SIZE> const& A, Vector<SIZE> const& B) {
    Vector<SIZE> R;

    VectorOperation<SIZE, VectorPairwiseSubtract<SIZE>, SIZE - 1> operation;
    operation(R, A, B);

    return R;
}

template <size_t SIZE>
Vector<SIZE>& operator-=(Vector<SIZE>& A, Vector<SIZE> const& B) {
    VectorOperation<SIZE, VectorCompoundSubtract<SIZE>, SIZE - 1> operation;
    operation(A, B);
    return A;
}

template <size_t SIZE>
Vector<SIZE> operator*(FloatType c, Vector<SIZE> const& V) {
    Vector<SIZE> R;

    VectorOperation<SIZE, VectorScalarMultiply<SIZE>, SIZE - 1> operation;
    operation(R, c, V);

    return R;
}

template <size_t SIZE>
Vector<SIZE> operator*(Vector<SIZE> const& V, FloatType c) {
    Vector<SIZE> R;

    VectorOperation<SIZE, VectorScalarMultiply<SIZE>, SIZE - 1> operation;
    operation(R, c, V);

    return R;
}

template <size_t SIZE>
Vector<SIZE>& operator*=(Vector<SIZE>& V, FloatType c) {
    VectorOperation<SIZE, VectorCompoundScalarMultiply<SIZE>, SIZE - 1>
        operation;
    operation(c, V);
    return V;
}

template <size_t SIZE>
Vector<SIZE> operator/(Vector<SIZE> const& V, FloatType c) {
    Vector<SIZE> R;

    VectorOperation<SIZE, VectorScalarDivide<SIZE>, SIZE - 1> operation;
    operation(R, c, V);

    return R;
}

template <size_t SIZE>
Vector<SIZE>& operator/=(Vector<SIZE>& V, FloatType c) {
    VectorOperation<SIZE, VectorCompoundScalarDivide<SIZE>, SIZE - 1> operation;
    operation(c, V);
    return V;
}

/**************************************************************************/
/**
 * @name Vector2D
 * @brief 2D (column) vector with extra magic for 2D specific operations/names
 *
 *
 */
struct Vector2D : public Vector<2> {

    FloatType& x; /**< Name alias for x component */
    FloatType& y; /**< Name alias for y component */

    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     */
    Vector2D() : x(components[0]), y(components[1]) {}

    /**************************************************************************/
    /**
     * @brief  Component initialization constructor
     *
     * @param _x New x value
     * @param _y New y value
     *
     */
    Vector2D(FloatType _x, FloatType _y) :
        Vector<2>(_x, _y), x(components[0]), y(components[1]) {}

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Vector to copy
     *
     */
    Vector2D(Vector2D const& other) :
        Vector<2>(other), x(components[0]), y(components[1]) {}

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Vector to copy
     *
     */
    Vector2D(Vector<2> const& other) :
        Vector<2>(other), x(components[0]), y(components[1]) {}

    Vector2D& operator=(Vector2D const& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
};

/**************************************************************************/
/**
 * @name Vector3D
 * @brief 3D (column) vector with extra magic for 2D specific operations/names
 *
 *
 */
struct Vector3D : public Vector<3> {

    FloatType& x; /**< Name alias for x component */
    FloatType& y; /**< Name alias for y component */
    FloatType& z; /**< Name alias for z component */

    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     */
    Vector3D() : x(components[0]), y(components[1]), z(components[2]) {}

    /**************************************************************************/
    /**
     * @brief  Component initialization constructor
     *
     * @param _x New x value
     * @param _y New y value
     * @param _z New z value
     *
     */
    Vector3D(FloatType _x, FloatType _y, FloatType _z) :
        Vector<3>(_x, _y, _z), x(components[0]), y(components[1]),
        z(components[2]) {}

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Vector to copy
     *
     */
    Vector3D(Vector3D const& other) :
        Vector<3>(other), x(components[0]), y(components[1]), z(components[2]) {
    }

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Vector to copy
     *
     */
    Vector3D(Vector<3> const& other) :
        Vector<3>(other), x(components[0]), y(components[1]), z(components[2]) {
    }
};

/**************************************************************************/
/**
 * @name Matrix
 * @brief An M * N matrix
 *
 * Going to take a page out of the OpenGL playbook and store a matrix
 *  as column major and indexed by the columns -- lets see how this works out
 *
 */
template <size_t SIZE_M, size_t SIZE_N>
struct Matrix {
  private:
    inline void assignComponent(size_t pos){};

    template <typename... Args>
    inline void assignComponent(size_t pos, FloatType c, Args const&... args) {
        size_t row = pos % SIZE_N;
        size_t column = (pos - row) / SIZE_M;

        columns[column][row] = c;

        assignComponent(pos + 1, args...);
    }

  public:
    std::array<Vector<SIZE_M>, SIZE_N> columns;

    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     */
    Matrix() {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor
     *
     */
    template <typename... Args>
    Matrix(FloatType c, Args const&... args) {
        static_assert(sizeof...(Args) + 1 == SIZE_M * SIZE_N,
                      "Wrong number of "
                      "parameters to "
                      "vector constructor");
        assignComponent(0, c, args...);
    }

    /**************************************************************************/
    /**
     * @brief  Initialization list constructor
     *
     */
    Matrix(std::initializer_list<Vector<SIZE_M>> initializer) {
        size_t index = 0;
        for (auto const& col : initializer) {
            columns[index] = col;
            ++index;
        }
    }

    /**************************************************************************/
    /**
     * @brief  Convenience operator for accessing the columns
     *
     */
    Vector<SIZE_M>& operator[](size_t col) { return columns[col]; }

    /**************************************************************************/
    /**
     * @brief  Convenience operator for accessing the columns
     *
     */
    Vector<SIZE_M> const& operator[](size_t col) const { return columns[col]; }
};

template <size_t SIZE_M, size_t SIZE_N, size_t N>
struct MultiplyMatrixVector {

    inline void operator()(Vector<SIZE_M>& result,
                           Matrix<SIZE_M, SIZE_N> const& M,
                           Vector<SIZE_N> const& V) {

        MultiplyMatrixVector<SIZE_M, SIZE_N, N - 1> multiplier;
        multiplier(result, M, V);
        result += V[N] * M[N];
    }
};

template <size_t SIZE_M, size_t SIZE_N>
struct MultiplyMatrixVector<SIZE_M, SIZE_N, 0> {

    inline void operator()(Vector<SIZE_M>& result,
                           Matrix<SIZE_M, SIZE_N> const& M,
                           Vector<SIZE_N> const& V) {
        result = V[0] * M[0];
    }
};

template <size_t SIZE_M, size_t SIZE_N>
inline Vector<SIZE_M> operator*(Matrix<SIZE_M, SIZE_N> const& M,
                                Vector<SIZE_N> const& V) {

    Vector<SIZE_M> result;
    MultiplyMatrixVector<SIZE_M, SIZE_N, SIZE_N - 1> multiplier;
    multiplier(result, M, V);

    return result;
}

/******
 *
 * Boxes
 *
 */

/**************************************************************************/
/**
 * @name AABox2D
 * @brief Simple 2d axis aligned box
 *
 *
 *
 */
struct AABox2D {
  private:
    // given two pairs, assign our end points
    inline void assignPoints(FloatType x1, FloatType y1, FloatType x2,
                             FloatType y2) {

        if (x1 <= x2) {
            UL.x = x1;
            LR.x = x2;
        } else {
            UL.x = x2;
            LR.x = x1;
        }

        if (y1 >= y2) {
            UL.y = y1;
            LR.y = y2;
        } else {
            UL.y = y2;
            LR.y = y1;
        }
    }

  public:
    Vector2D UL; /**< Upper lefthand corner of the box */
    Vector2D LR; /**< Upper lefthand corner of the box */

    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     */
    AABox2D(){};

    /**************************************************************************/
    /**
     * @brief  Copy contructor
     *
     */
    AABox2D(AABox2D const& other) : UL(other.UL), LR(other.LR) {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor
     *
     * @param _UL Upper left point
     * @param _LR Lower right point
     *
     */
    AABox2D(Vector<2> _UL, Vector<2> _LR) {
        // Just make sure that this is indeed the relative position we want
        assignPoints(_UL[0], _UL[1], _LR[0], _LR[1]);
    };

    /**************************************************************************/
    /**
     * @brief  Initialization constructor
     *
     * @param x1 First x component
     * @param y1 First y component
     * @param x2 Second x component
     * @param y2 Second y component
     *
     */
    AABox2D(FloatType x1, FloatType y1, FloatType x2, FloatType y2) {
        // Just make sure that this is indeed the relative position we want
        assignPoints(x1, y1, x2, y2);
    };

    /**************************************************************************/
    /**
     * @brief  Move this to a given position (preserving dimensions)
     *
     * @param _UL New upper left coordinate
     *
     */
    void moveTo(MJB_3D::Vector2D _UL) {
        auto w = width();
        auto h = height();
        UL = _UL;
        LR = Vector2D(UL.x + w, UL.y - h);
    }

    /**************************************************************************/
    /**
     * @brief  Compute the width
     *
     * @returns Width as floating point
     *
     */
    FloatType width() { return LR.x - UL.x; }

    /**************************************************************************/
    /**
     * @brief  Compute the height
     *
     * @returns Height as floating point
     *
     */
    FloatType height() { return UL.y - LR.y; }

    /**************************************************************************/
    /**
     * @brief Determines if this box contains a given point
     *
     * @param p 2D point (vector)
     *
     * @returns true if point is contained, false otherwise
     *
     */

    inline bool contains(Vector<2> const& p) {
        return p[0] >= UL.x and p[0] <= LR.x and p[1] <= UL.y and p[1] >= LR.y;
    }

    /**************************************************************************/
    /**
     * @brief Determines if this box contains another box
     *
     * @param Vector2D
     *
     */
    inline bool contains(AABox2D const& b) {
        return contains(b.UL) and contains(b.LR);
    }

    /**************************************************************************/
    /**
     * @brief Determines if this box overlaps with another box
     *
     * @param Vector2D
     *
     */
    inline bool intersects(AABox2D const& b) {

        return not(LR.x < b.UL.x or b.LR.x < UL.x or UL.y < b.LR.y or
                   b.UL.y < LR.y);
    }

    /**************************************************************************/
    /**
     * @brief  Assignment operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D& operator=(AABox2D const& o) {
        UL = o.UL;
        LR = o.LR;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  Addition (by vector) operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D operator+(Vector2D const& v) const {
        return AABox2D(UL + v, LR + v);
    }

    /**************************************************************************/
    /**
     * @brief  Addition (by vector) operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D& operator+=(Vector2D const& v) {
        UL += v;
        LR += v;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  Subtraction (by vector) operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D operator-(Vector2D const& v) const {
        return AABox2D(UL - v, LR - v);
    }

    /**************************************************************************/
    /**
     * @brief  Subtraction (by vector) operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D& operator-=(Vector2D const& v) {
        UL -= v;
        LR -= v;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  Subtraction (by vector) operator
     *
     * @param o Box to assign from
     *
     */
    AABox2D operator*(FloatType f) const { return AABox2D(UL * f, LR * f); }

    /**************************************************************************/
    /**
     * @brief  Subtraction (by vector) operator
     *
     * @param f Box to assign from
     *
     */
    AABox2D& operator*=(FloatType f) {
        UL *= f;
        LR *= f;
        return *this;
    }
};

static AABox2D operator*(FloatType f, AABox2D const& b) { return b * f; }

static Vector2D interpolate(Vector2D const& a, Vector2D b, FloatType f) {
    FloatType xDiff = (b.x - a.x) * f;
    FloatType yDiff = (b.y - a.y) * f;

    return Vector2D(a.x + xDiff, a.y + yDiff);
}

/******
 *
 * Stream operators (mostly for debugging)
 *
 */

template <size_t SIZE>
std::ostream& operator<<(std::ostream& os, Vector<SIZE> const& vec) {
    os << "(";
    bool comma = false;
    for (auto component : vec.components) {
        if (comma) {
            os << ",";
        }
        os << component;
        comma = true;
    }
    os << ")";

    return os;
}

template <size_t SIZE_M, size_t SIZE_N>
std::ostream& operator<<(std::ostream& os, Matrix<SIZE_M, SIZE_N> const& mat) {
    os << "[ \n";

    for (size_t row = 0; row < SIZE_M; ++row) {
        os << "  ";
        for (auto col : mat.columns) {
            os << col[row] << " ";
        }
        os << '\n';
    }

    os << " ]";

    return os;
}

static std::ostream& operator<<(std::ostream& os, AABox2D const& b) {
    os << "[[" << b.UL << ", " << b.LR << "]]";
    return os;
}

} // namespace MJB_3D

#endif // MJB_3DMATH
