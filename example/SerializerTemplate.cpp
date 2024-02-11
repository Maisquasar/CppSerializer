#include "SerializerTemplate.h"

template<>
Vec2f CppSer::StringSerializer::As() const
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

std::string Vec2f::ToString() const
{
	return std::to_string(x) + " " + std::to_string(y);
}
