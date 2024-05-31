#include <iostream>
#include <string>

#include "SerializerTemplate.h"

using namespace std;

#define UNUSED(x) (void)(x)

int main(int argc, char** argv)
{
	using namespace CppSer;

	Serializer serializer("example.txt");
	serializer.SetVersion("1.0.1");
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
	serializer.CloseFile();
	// Or will automatically close on destructor of Serializer

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
			UNUSED(value2); //unused
		}
		// this will not work because of the Push depth
		int tabValue = parser["Tab key"].As<int>();
		UNUSED(tabValue);
	}

	std::cout << "Parser Content :" << std::endl;
	std::cout << parser.GetVersion() << std::endl;
	parser.PrintData();

	return 0;
}
