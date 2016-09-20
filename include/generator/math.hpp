// Copyright 2015 Markus Ilmola
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#ifndef GENERATOR_MATH_HPP
#define GENERATOR_MATH_HPP

#ifdef GENERATOR_USE_GLM

#include <stdexcept>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace gml {

// Vectors
using glm::bvec2;
using glm::dvec2;
using glm::ivec2;
using glm::uvec2;
using glm::vec2;

using glm::bvec3;
using glm::dvec3;
using glm::ivec3;
using glm::uvec3;
using glm::vec3;

using glm::bvec4;
using glm::dvec4;
using glm::ivec4;
using glm::uvec4;
using glm::vec4;

// Matrices
using glm::mat2;
using glm::mat2x2;
using glm::mat2x3;
using glm::mat2x4;
using glm::mat3;
using glm::mat3x2;
using glm::mat3x3;
using glm::mat3x4;
using glm::mat4;
using glm::mat4x2;
using glm::mat4x3;
using glm::mat4x4;

using glm::dmat2;
using glm::dmat2x2;
using glm::dmat2x3;
using glm::dmat2x4;
using glm::dmat3;
using glm::dmat3x2;
using glm::dmat3x3;
using glm::dmat3x4;
using glm::dmat4;
using glm::dmat4x2;
using glm::dmat4x3;
using glm::dmat4x4;

// Quaternions
using glm::quat;
using glm::dquat;

// Functions
using glm::clamp;
using glm::cross;
using glm::degrees;
using glm::dot;
using glm::max;
using glm::min;
using glm::mix;
using glm::normalize;
using glm::ortho;
using glm::perspective;
using glm::radians;
using glm::rotate;
using glm::translate;

// Function substitutes
template <typename T>
T angle(const glm::tvec2<T>& v1, const glm::tvec2<T>& v2) {
	using std::sqrt;
	using std::acos;
	using std::numeric_limits;

	const T len = sqrt(dot(v1, v1) * dot(v2, v2));
	if (len <= std::numeric_limits<T>::epsilon()) return T{0};
	return acos(clamp(dot(v1, v2) / len, T{-1}, T{1}));
}

template <typename T>
T angle(const glm::tvec3<T>& v1, const glm::tvec3<T>& v2) {
	using std::sqrt;
	using std::acos;
	using std::numeric_limits;

	const T len = sqrt(dot(v1, v1) * dot(v2, v2));
	if (len <= std::numeric_limits<T>::epsilon()) return T{0};
	return acos(clamp(dot(v1, v2) / len, T{-1}, T{1}));
}

template <typename T>
glm::tvec2<T> cross(const glm::tvec2<T>& v) {
	return glm::tvec2<T>(-v.y, v.x);
}

template <typename T>
glm::tvec3<T> transform(const glm::tquat<T>& q, const glm::tvec3<T>& v) {
	const glm::tvec3<T> temp = T{2.0} * cross(glm::tvec3<T>(q.x, q.y, q.z), v);
	return v + q.w * temp + cross(glm::tvec3<T>(q.x, q.y, q.z), temp);
}

template <typename T>
glm::tquat<T> qrotate(const T& angle, const glm::tvec3<T>& axis) {
	using std::sin;
	using std::cos;

	const T a = angle / T{2.0};
	return glm::tquat<T>{cos(a), sin(a) * axis};
}

template <typename T>
glm::tvec3<T> normal(const glm::tvec3<T>& p1, const glm::tvec3<T>& p2, const glm::tvec3<T>& p3) {
	return normalize(cross(p2 - p1, p3 - p1));
}

template <typename T>
glm::tvec3<T> project(
	const glm::tvec3<T>& v, const glm::tmat4x4<T>& modelViewNroj,
	const ivec4& viewport
) {
	glm::tvec4<T> in = modelViewNroj * glm::tvec4<T>{v, T{1.0}};

	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];

	in[0] = in[0] * T{0.5} + T{0.5};
	in[1] = in[1] * T{0.5} + T{0.5};
	in[2] = in[2] * T{0.5} + T{0.5};

	in[0] = in[0] * viewport[2] + viewport[0];
	in[1] = in[1] * viewport[3] + viewport[1];

	return glm::tvec3<T>{in};
}

template <typename T>
glm::tmat4x4<T> ortho2D(
	const T& left, const T& right, const T& bottom, const T& top
) {
	return ortho(left, right, bottom, top, T{-1}, T{1});
}

template <typename T>
glm::tvec3<T> slerp(const glm::tvec3<T>& v1, const glm::tvec3<T>& v2, const T& a) {
	using std::sin;
	const T theta = angle(v1, v2);
	const T sine = sin(theta);
	return sin((T{1} - a) * theta) / sine * v1 + sin(a * theta) / sine * v2;
}

template <typename T>
glm::tmat4x4<T> rotate(const glm::tvec3<T>& angle) {
	using std::sin;
	using std::cos;

	const T sy = sin(angle[2]);
	const T cy = cos(angle[2]);
	const T sp = sin(angle[1]);
	const T cp = cos(angle[1]);
	const T sr = sin(angle[0]);
	const T cr = cos(angle[0]);

	const T data[16] = {
		cp * cy, sr * sp * cy + cr * -sy, cr * sp * cy + -sr * -sy, T{0},
		cp * sy, sr * sp * sy + cr * cy,  cr * sp * sy + -sr * cy,  T{0},
		-sp,     sr * cp,                 cr * cp,                  T{0},
		T{0},    T{0},                    T{0},                     T{1}
	};
	return glm::rowMajor4(glm::make_mat4(data));
}

template <typename T>
glm::tmat3x3<T>  rotate(const T& angle) {
	using std::sin;
	using std::cos;

	const T s = sin(angle);
	const T c = cos(angle);

	const T data[9] = {
		c,    -s,    T{0},
		s,     c,    T{0},
		T{0},  T{0}, T{1}
	};

	return glm::rowMajor3(glm::make_mat3(data));
}

template <typename T>
glm::tvec2<T> transform(const glm::tmat3x3<T>& m, const glm::tvec2<T>& v) {
	return glm::tvec2<T>(m * glm::tvec3<T>(v, 1.0));
}


namespace detail {

template <typename VecT, typename T>
VecT bezierImpl(
	const VecT* p, std::size_t n, T t1, T t2, std::size_t stride = 1
) {
	if (n == 1) return *p;
	if (n == 2) return t1 * p[0] + t2 * p[stride];
	return
		t1 * bezierImpl(p, n - 1, t1, t2, stride) +
		t2 * bezierImpl(p + stride, n - 1, t1, t2, stride);
}

} // detail


template <std::size_t D, typename T>
glm::tvec2<T> bezier(const glm::tvec2<T> (&p)[D], T t)
{
	static_assert(D > 0, "At least one control point needed.");
	return detail::bezierImpl(&p[0], D, static_cast<T>(1) - t, t);
}

template <std::size_t D, typename T>
glm::tvec3<T> bezier(const glm::tvec3<T> (&p)[D], T t)
{
	static_assert(D > 0, "At least one control point needed.");
	return detail::bezierImpl(&p[0], D, static_cast<T>(1) - t, t);
}

template <std::size_t D0, std::size_t D1, typename T>
glm::tvec3<T> bezier2(
	const glm::tvec3<T> (&p)[D1][D0], const glm::tvec2<T>& t
) {
	static_assert(D0 > 0, "At least one control point needed.");
	static_assert(D1 > 0, "At least one control point needed.");

	glm::tvec3<T> temp[D1];
	for (std::size_t i = 0; i < D1; ++i) {
		temp[i] = bezier(p[i], t[0]);
	}
	return bezier(temp, t[1]);
}

namespace detail {

template <std::size_t O, std::size_t D, typename VecT, typename T>
struct bezierDerivativeImpl {
	static VecT calc(const VecT (&p)[D], T t)
	{
		VecT temp[D - 1];
		for (std::size_t i = 0; i < D - 1; ++i) {
			temp[i] = static_cast<T>(D - 1) * (p[i + 1] - p[i]);
		}
		return bezierDerivativeImpl<O - 1, D - 1, VecT, T>::calc(temp, t);
	}
};

template <std::size_t D, typename VecT, typename T>
struct bezierDerivativeImpl<0, D, VecT, T> {
	static VecT calc(const VecT (&p)[D], T t) {
		return bezier(p, t);
	}
};

template <typename VecT, typename T>
struct bezierDerivativeImpl<0, 1, VecT, T> {
	static VecT calc(const VecT (&p)[1], T t) {
		return bezier(p, t);
	}
};

template <std::size_t O, typename VecT, typename T>
struct bezierDerivativeImpl<O, 1, VecT, T> {
	static VecT calc(const VecT (&)[1], T) {
		return VecT{static_cast<T>(0)};
	}
};

} // detail

template <std::size_t O, std::size_t D, typename T>
glm::tvec2<T> bezierDerivative(const glm::tvec2<T> (&p)[D], T t)
{
	static_assert(O > 0, "The derivative order must be at least one.");
	static_assert(D > 0, "At least one control point needed.");
	return detail::bezierDerivativeImpl<O, D, glm::tvec2<T>, T>::calc(p, t);
}

template <std::size_t O, std::size_t D, typename T>
glm::tvec3<T> bezierDerivative(const glm::tvec3<T> (&p)[D], T t)
{
	static_assert(O > 0, "The derivative order must be at least one.");
	static_assert(D > 0, "At least one control point needed.");
	return detail::bezierDerivativeImpl<O, D, glm::tvec3<T>, T>::calc(p, t);
}

template <std::size_t O, std::size_t D0, std::size_t D1, typename T>
glm::tmat2x3<T> bezier2Jacobian(
	const glm::tvec3<T> (&p)[D1][D0], const glm::tvec2<T>& t
) {
	static_assert(O > 0, "Order of the Jacobian must be at least one.");
	static_assert(D0 > 0, "At least one control point needed.");
	static_assert(D1 > 0, "At least one control point needed.");

	glm::tvec3<T> temp0[D0];
	for (std::size_t i = 0; i < D0; ++i) {
		temp0[i] = detail::bezierImpl(&p[0][i], D1, static_cast<T>(1) - t[1], t[1], D0);
	}

	glm::tvec3<T> temp1[D1];
	for (std::size_t i = 0; i < D1; ++i) {
		temp1[i] = bezier(p[i], t[0]);
	}

	return glm::tmat2x3<T>{
		bezierDerivative<O>(temp0, t[0]),
		bezierDerivative<O>(temp1, t[1])
	};
}

}

#else

#include <gml/gml.hpp>

#endif


#endif
