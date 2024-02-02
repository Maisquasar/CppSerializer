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

#define EXTRA_CPPSERIALIZER_SERIALIZER \
Serializer& operator<<(const Vec2f& value)\
{\
	const std::string stringValue = value.ToString();\
	*this << stringValue.c_str();\
	return *this;\
}\

#define EXTRA_CPPSERIALIZER_PARSER \
template<>\
Vec2f As() const\
{\
	std::istringstream ss(m_content);\
	Vec2f vec2f;\
	ss >> vec2f.x >> vec2f.y; \
	return vec2f;\
}

#include "CppSerializer.h"

using namespace std;

int main(int argc, char** argv)
{
	using namespace CppSer;

	Serializer serializer("example.txt");
	serializer.SetTabSize(10);

	Vec2f vec2f{ 2, 3 };
	serializer << Pair::BeginMap << "Map 0";
	{
		serializer << Pair::Key << "Key" << Pair::Value << "Value";
		serializer << Pair::Key << "Vec2f" << Pair::Value << vec2f;
		serializer << Pair::BeginMap << "Map 1";
		{
			serializer << Pair::Key << "Key Map 1" << Pair::Value << 2.f;
		}
		serializer << Pair::EndMap << "Map 1";
		{
			serializer << Pair::BeginTab;
			serializer << Pair::Key << "Tab key" << Pair::Value << 10;
			serializer << Pair::EndTab;
		}
	}
	serializer << Pair::EndMap << "Map 0";
	// Or will automatically close on destructor of Serializer
	serializer.CloseFile();

	vec2f = {};

	std::cout << "Serializer Content :" << std::endl;
	std::cout << serializer.GetContent() << std::endl;

	Parser parser(serializer.GetContent());
	{
		vec2f = parser["Vec2f"].As<Vec2f>();
		std::string value = parser["Value"];
		{
			// New Depth for map
			parser.PushDepth();
			float value2 = parser["Value"].As<float>();
		}
		// this will not work because of the Push depth
		int tabValue = parser["Tab key"].As<int>();
	}

	std::cout << "Parser Content :" << std::endl;
	parser.PrintData();

	return 0;
}
