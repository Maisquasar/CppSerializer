#include <iostream>
#include <string>

struct Vec2f
{
	float x;
	float y;

	std::string ToString() const
	{
		return std::to_string(x) + " " + std::to_string(y);
	}
};

#define EXTRA_CPPSERIALIZER \
Serializer& operator<<(const Vec2f& value)\
{\
	const std::string stringValue = value.ToString();\
	*this << stringValue.c_str();\
	return *this;\
}\

#include "CppSerializer.h"

using namespace std;

int main(int argc, char** argv)
{
	using namespace CppSer;

	Serializer serializer("example.txt");
	serializer << Pair::BeginMap << "Abc";
	serializer << Pair::Key << "Key" << Pair::Value << "Value";
	Vec2f vec2f{2, 3};
	serializer << Pair::Key << "Vec2f" << Pair::Value << vec2f;
    serializer.CloseFile();
	serializer << Pair::EndMap << "Abc";

	vec2f = {};

	std::cout << serializer.GetContent() << std::endl;

	Parser parser(serializer.GetContent());
	vec2f = parser["Vec2f"].As<Vec2f>();
	std::string value = parser["Value"];

    return 0;
}
