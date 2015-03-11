#include <iostream>
#include <iomanip>
using namespace std;

#include <ew/ew.hpp>
#include <ew/Ew_program.hpp>

using namespace ew;
using namespace ew::core::types;

using namespace ew::maths;
using namespace ew::maths::Constants;

void test_vector()
{
	// vec2
	{
		f32 vec[ 2 ] = { 25, 26 };

		vec2 v0;
		vec2 v1(vec);

		v0.x = 45;
		v0.y = 85;
		vec2 v2(v0);

		vec2 v3(6, 5);

		v3[ 0 ] = v3[ 1 ];
		if (v0 == v1) {}

		if (v0 != v1) {}

		vec2 v4 = v1 + v2;
		vec2 v5 = v2 - v1;
		v5 += 5.0f;
		v5 -= 2.0f;
		v5 = -v5;
	}

	{
		//  vec3;
	}

	{
		//  Vec4f;
	}


	//   float vec[3] = { 4, 5, 6 };

	//   vec3 v1(1,0);
	//   vec3 v2(0,1);
	//   vec3 v3;
	//   vec3 v4;

	//   float dot = Dot(v1, v2);
	//   v3 = Cross(v1, v2);
	//   v4 = vec;

	//   cout << "Dot(v1, v2) = " << dot << "\n";
	//   cout << "v1 = " << v1 << "\n";
	//   cout << "v2 = " << v2 << "\n";
	//   cout << "v3 = v1 Cross v2 = " << v3 << "\n";
	//   cout << "v4 = " << v4 << "\n\n";
}

void test_matrix()
{
	mat4 m0;

	m0.LoadIdentity();

	vec3 x(1, 0, 0);
	vec3 y(0, 2, 0);
	vec3 z(0, 0, 3);

	m0(0, 0) = x[ 0 ];
	m0(0, 1) = x[ 1 ];
	m0(0, 2) = x[ 2 ];
	m0(0, 3) = 0.0;
	m0(1, 0) = y[ 0 ];
	m0(1, 1) = y[ 1 ];
	m0(1, 2) = y[ 2 ];
	m0(1, 3) = 0.0;
	m0(2, 0) = z[ 0 ];
	m0(2, 1) = z[ 1 ];
	m0(2, 2) = z[ 2 ];
	m0(2, 3) = 0.0;
	m0(3, 0) = 0.0;
	m0(3, 1) = 1.5;
	m0(3, 2) = 0.0;
	m0(3, 3) = 4.0;

	Matrix44f m1 = m0.Inverse();

	cout << m0 << "\n";
	cout << m1 << "\n";


	cout << "sizeof(mat44f) == " << sizeof(m1) << "\n";
}

void test_vector_and_matrix()
{
	vec3 v0(1, 2, 3);
	mat4 m0;
	mat4 m1;

	m0.LoadIdentity();

	vec3 v1 = m0 * v0;

	m1 = m0.Inverse();
	cout << "m0 = \n" << m0 << "\n";
	cout << "m1 = \n" << m1 << "\n";
	cout << "v1 = " << v1 << "\n";
}


void test_constant()
{
	cout << setprecision(16) << "Pi = " << F32::Pi << "\n";
	cout << "2 * Pi = " << F64::Two_Pi << "\n\n";
}

int main(int ac, char * av[])
{
	test_constant();
	test_vector();
	test_matrix();
	test_vector_and_matrix();

	while (1)
		pause();

	return (0);
}
