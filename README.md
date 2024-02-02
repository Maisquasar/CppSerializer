# CppSerializer Project

CppSerializer is a simple C++ library that provides serialization and parsing capabilities. It allows you to easily serialize and deserialize data structures, including custom types, to and from various formats, such as text files.

## Usage

### Including CppSerializer

```cpp
#include "CppSerializer.h"
```

### Defining a Custom Type for Serialization

To add support for a custom type (e.g., `Vec2f`), use the provided macros to extend the functionality of the serializer and parser.

```cpp
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
```

### Using CppSerializer in Your Code
You can see this example in the example folder

```cpp
#include "CppSerializer.h"

using namespace std;

int main(int argc, char** argv)
{
    using namespace CppSer;

    // Create a serializer with a specified file name
    Serializer serializer("example.txt");
    serializer.SetTabSize(10);

    Vec2f vec2f{ 2, 3 };

    // Serialize data using a fluent interface
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

    // Close the file explicitly or let the destructor do it

    // Get the serialized content
    std::cout << "Serializer Content :" << std::endl;
    std::cout << serializer.GetContent() << std::endl;

    // Create a parser with the serialized content
    Parser parser(serializer.GetContent());
    {
        // Parse data using a fluent interface
        vec2f = parser["Vec2f"].As<Vec2f>();
        std::string value = parser["Value"];
        {
            // New Depth for map
            parser.PushDepth();
            float value2 = parser["Value"].As<float>();
        }
        // This will not work because of the Push depth
        int tabValue = parser["Tab key"].As<int>();
    }

    // Print the parsed data
    std::cout << "Parser Content :" << std::endl;
    parser.PrintData();

    return 0;
}
```


### Output
example.txt
```
 ------------- Map 0 ------------- 
[Key] : Value
[Vec2f] : 2.000000 3.000000
 ------------- Map 1 ------------- 
[Key Map 1] : 2.000000
 ============= Map 1 ============= 
          [Tab key] : 10
 ============= Map 0 ============= 

```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE,txt) file for details.

Feel free to contribute and improve the CppSerializer library!
