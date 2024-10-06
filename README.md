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
struct Vec2f
{
	Vec2f(float x = 0, float y = 0) : x(x), y(y) {}

	float x;
	float y;

	std::string ToString() const
	{
		return std::to_string(x) + " " + std::to_string(y);
	}
};

template<>
Vec2f CppSer::StringSerializer::As<Vec2f>() const
{
	std::istringstream ss(m_content);
	Vec2f vec2;
	ss >> vec2.x >> vec2.y;
	return vec2;
}

template<>
CppSer::Serializer& CppSer::Serializer::operator<<(const Vec2f& value)
{
	const std::string stringValue = value.ToString();
	*this << stringValue.c_str();
	return *this;
}
```

### Using CppSerializer in Your Code
You can see an example in the [example folder](https://github.com/Maisquasar/CppSerializer/tree/master/example).

### Running the Example
To run the example, simply compile the code with [xmake](https://github.com/xmake-io/xmake) and run it.

Compile :
```bash
$ xmake f -p [windows|linux|macosx|android|iphoneos ..] -a [x86|arm64 ..] -m [debug|release]
$ xmake
```

Run :
```bash
$ xmake run
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE.txt) file for details.

Feel free to contribute and improve the CppSerializer library!
