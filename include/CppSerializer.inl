#pragma once
#include "CppSerializer.h"

#pragma region Serializer
inline CppSer::Serializer::Serializer(const std::filesystem::path& path)
{
	m_filePath = path;
}

inline CppSer::Serializer::~Serializer()
{
	if (!m_filePath.empty() && m_saveOnDestroy)
		CloseFile();
}

inline void CppSer::Serializer::CloseFile() const
{
	std::ofstream file = std::ofstream(m_filePath);
	if (!file.is_open()) {
		std::cerr << ("File %s could'nt be create", m_filePath.string().c_str()) << std::endl;
		file.close();
	}
	file << m_content.str();
	file.close();
}

template<typename T> inline CppSer::Serializer& CppSer::Serializer::operator<<(const T& value)
{
	std::cerr << "Error while serialize type : " << typeid(T).name() << std::endl;
	return *this;
}

template<typename T> inline CppSer::Serializer& CppSer::Serializer::operator<<(T* value)
{
	std::cout << "Error while serialize type : " << typeid(*value).name() << std::endl;
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const std::string& value)
{
	*this << value.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const char& value)
{
	const std::string stringValue(1, value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const std::filesystem::path& value)
{
	*this << value.generic_string().c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const bool& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const float& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const int& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const unsigned long& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}
template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const long long& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const unsigned long long& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const double& value)
{
	const std::string stringValue = std::to_string(value);
	*this << stringValue.c_str();
	return *this;
}

template<> inline CppSer::Serializer& CppSer::Serializer::operator<<(const Pair& value)
{
	SetCurrentType(value);
	return *this;
}

inline CppSer::Serializer& CppSer::Serializer::operator<<(const char* value)
{
	switch (m_currentType)
	{
	case Pair::Key:
	{
		SetCurrentKey(value);
	}
	break;
	case Pair::Value:
	{
		SetCurrentValue(value);
		WriteLine();
	}
	break;
	case Pair::Title:
	{
		AddLine(value);
	}
	break;
	case Pair::BeginMap:
	{
		m_content << value << MAP_SEPARATOR_BEGIN << '\n';
	}
	break;
	case Pair::EndMap:
	{
		m_content << value << MAP_SEPARATOR_END << '\n';
	}
	break;
	default:
		break;
	}
	return *this;
}

// Private
inline void CppSer::Serializer::SetCurrentType(const Pair val)
{
	switch (val)
	{
	case Pair::BeginTab:
		PushTab();
		break;
	case Pair::EndTab:
		PopTab();
		break;
	case Pair::BeginMap:
		m_currentType = val;
		m_content << m_tab << MAP_SEPARATOR_BEGIN;
		break;
	case Pair::EndMap:
		m_currentType = val;
		m_content << m_tab << MAP_SEPARATOR_END;
		break;
	default:
		m_currentType = val;
		break;
	}
}

inline void CppSer::Serializer::WriteLine()
{
	AddLine(m_currentPair.first, m_currentPair.second);
}

inline void CppSer::Serializer::AddLine(const std::string& key, const std::string& value)
{
	m_content << m_tab << "[" << key << "] : " << value << '\n';
}

inline void CppSer::Serializer::AddLine(const std::string& line)
{
	m_content << m_tab << line << '\n';
}

inline void CppSer::Serializer::PushTab()
{
	for (int i = 0; i < m_tabSize; i++)
		m_tab.push_back(' ');
}

inline void CppSer::Serializer::PopTab()
{
	for (int i = 0; i < m_tabSize; i++)
		m_tab.pop_back();
}
#pragma endregion

#pragma region Parser
inline CppSer::Parser::Parser(const std::filesystem::path& path)
{
	ParseFile(path);
}

inline CppSer::Parser::Parser(const std::string& content)
{
	ParseContent(content);
}

inline void CppSer::Parser::ParseFile(const std::filesystem::path& path)
{
	auto file = std::fstream(path, std::ios::in);
	if (!file.is_open()) {
		file.close();
		m_fileOpen = false;
		return;
	}
	// Get file content
	const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	ParseContent(content);

	file.close();
	m_fileOpen = true;
}

inline void CppSer::Parser::ParseContent(const std::string& content)
{
	std::stringstream ss(content);

	std::string line;
	std::unordered_map<std::string, StringSerializer>* currentMap = nullptr;

	size_t currMapIndex = -1;
	while (std::getline(ss, line)) {
		if (line.find(MAP_SEPARATOR_BEGIN) != std::string::npos)
		{
			currMapIndex++;
			m_valueMap.emplace_back();
			currentMap = &m_valueMap[currMapIndex];
		}

		if (const size_t pos = line.find("["); pos != std::string::npos)
		{
			std::string currentKey = line.substr(pos + 1);
			currentKey = currentKey.substr(0, currentKey.find_first_of(']'));
			std::string currentValue = line.substr(line.find_first_of(':') + 2);

			currentMap->insert({ currentKey, currentValue });
		}
	}
}

inline void CppSer::Parser::PrintData()
{
	for (auto& maps : m_valueMap)
	{
		for (auto& [key, value] : maps)
		{
			std::cout << key << " : " << (std::string)value << std::endl;
		}
		std::cout << "---------------------------" << std::endl;
	}
}

inline void CppSer::Parser::PushDepth()
{
	m_currentDepth++;
}

inline CppSer::StringSerializer CppSer::Parser::operator[](const std::string& key)
{
	//ASSERT(m_valueMap.size() > m_currentDepth);
	//ASSERT(m_valueMap[m_currentDepth].contains(key));
	if (m_valueMap.size() <= m_currentDepth || !m_valueMap[m_currentDepth].contains(key))
		return {};

	return m_valueMap[m_currentDepth][key];
}

#pragma endregion

#pragma region StringSerializer
template <typename T> T CppSer::StringSerializer::As() const
{
	std::cerr << "Error with parsing type as with type " << typeid(T).name() << std::endl;
	return T();
}

template <>
inline std::string CppSer::StringSerializer::As() const
{
	return m_content;
}

template <>
inline float CppSer::StringSerializer::As() const
{
	try
	{
		return std::stof(m_content);
	}
	catch (...)
	{
		return 0.f;
	}
}

template <>
inline double CppSer::StringSerializer::As() const
{
	try
	{
		return std::stod(m_content);
	}
	catch (...)
	{
		return 0.0;
	}
}

template <>
inline unsigned long long CppSer::StringSerializer::As() const
{
	try
	{
		return std::stoull(m_content);
	}
	catch (...)
	{
		return -1;
	}
}

template <>
inline long long CppSer::StringSerializer::As() const
{
	try
	{
		return std::stoll(m_content);
	}
	catch (...)
	{
		return 0;
	}
}

template <>
inline unsigned long CppSer::StringSerializer::As() const
{
	try
	{
		return std::stoul(m_content);
	}
	catch (...)
	{
		return -1;
	}
}

template <>
inline int CppSer::StringSerializer::As() const
{
	try
	{
		return std::stoi(m_content);
	}
	catch (...)
	{
		return 0;
	}
}

template <>
inline bool CppSer::StringSerializer::As() const
{
	try
	{
		return std::stoi(m_content);
	}
	catch (...)
	{
		return 0;
	}
}
#pragma endregion