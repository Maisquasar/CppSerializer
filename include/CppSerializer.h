#pragma once
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <string>

#define MAP_SEPARATOR_BEGIN " ------------- "
#define MAP_SEPARATOR_END " ============= "

namespace CppSer {
	enum class Pair
	{
		Key,
		Value,
		BeginTab,
		EndTab,
		BeginMap,
		EndMap,
		Title,
	};

	class Serializer
	{
	public:
		Serializer() = default;
		explicit Serializer(const std::filesystem::path& path);
		~Serializer();

		inline void SetVersion(const std::string& version);
		
		// Closes the file associated with the serializer (do not clear the content)
		inline void CloseFile() const;

		// Overloaded operators to serialize various data types.
		template<typename T> Serializer& operator<<(const T& value);
		template<typename T> Serializer& operator<<(T* value);

		inline Serializer& operator<<(const bool& value);
		inline Serializer& operator<<(const char& value);
		inline Serializer& operator<<(const int& value);
		inline Serializer& operator<<(const unsigned int& value);
		inline Serializer& operator<<(const long long& value);
		inline Serializer& operator<<(const unsigned long long& value);
		inline Serializer& operator<<(const float& value);
		inline Serializer& operator<<(const double& value);
		inline Serializer& operator<<(const std::string& value);
		inline Serializer& operator<<(const std::filesystem::path& value);

		inline Serializer& operator<<(const Pair& value);
		inline Serializer& operator<<(const char* value);

		//template<typename T> Serializer& operator<<(T* value);

		/*
		// Extra serialization functionality can be added here using the template method operator<<() and As(), e.g:

		struct Vec2f
		{
			float x;
			float y;

			std::string ToString() const
			{
				return std::to_string(x) + " " + std::to_string(y);
			}
		};

		// Serialization
		template<>
		CppSer::Serializer& CppSer::Serializer::operator<<(const Vec2f& value)
		{
			const std::string stringValue = value.ToString();
			*this << stringValue.c_str();
			return *this;
		}

		// Parsing
		template<>
		Vec2f CppSer::StringSerializer::As<Vec2f>() const
		{
			std::istringstream ss(m_content);
			Vec2f vec2;
			ss >> vec2.x >> vec2.y;
			return vec2;
		}
		*/

		inline std::string GetContent() const { return m_content.str(); }
		inline void SetTabSize(uint32_t size) { m_tabSize = size; }
		inline void SetShouldSaveOnDestroy(bool val) { m_saveOnDestroy = val; }
	private:
		inline void WriteLine();
		inline void AddLine(const std::string& key, const std::string& value);
		inline void AddLine(const std::string& line);

		inline void SetCurrentType(Pair val);
		inline void SetCurrentKey(const std::string& key) { m_currentPair.first = key; }
		inline void SetCurrentValue(const std::string& value) { m_currentPair.second = value; }

		inline void PushTab();
		inline void PopTab();
	private:
		std::stringstream m_content;
		std::filesystem::path m_filePath;
		uint32_t m_tabSize = 2;

		std::string m_tab;

		bool m_saveOnDestroy = true;
		bool m_hasVersion = false;

		std::pair<std::string, std::string> m_currentPair;

		Pair m_currentType = Pair::Key;
	};

	class StringSerializer
	{
	public:
		StringSerializer() = default;
		StringSerializer(std::string value) : m_content(std::move(value)) {}
		StringSerializer(const StringSerializer& value) : m_content(value) {}

		StringSerializer& operator=(const std::string& str) {
			m_content = str;
			return *this;
		}

		StringSerializer& operator=(const char* cstr) {
			m_content = cstr;
			return *this;
		}

		StringSerializer& operator=(char c) {
			m_content = c;
			return *this;
		}

		StringSerializer& operator=(const StringSerializer& other) {
			if (this != &other) {
				m_content = other.m_content;
			}
			return *this;
		}

		// Custom operator= for move assignment
		StringSerializer& operator=(StringSerializer&& other) noexcept {
			if (this != &other) {
				m_content = std::move(other.m_content);
			}
			return *this;
		}

		operator std::string() const {
			return m_content;
		}

		template <typename T> T As() const;

	private:
		std::string m_content;
	};

	template<> inline int StringSerializer::As() const
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

	template<> inline std::string StringSerializer::As() const
	{
		return m_content;
	}

	template<> inline float StringSerializer::As() const
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

	template<> inline double StringSerializer::As() const
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

	template<> inline unsigned long long StringSerializer::As() const
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

	template<> inline long long StringSerializer::As() const
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

	template<> inline unsigned long StringSerializer::As() const
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

	template<> inline bool StringSerializer::As() const
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


	class Parser
	{
	public:
		inline Parser(const std::string& content);
		inline Parser(const std::filesystem::path& path);

		inline bool IsFileOpen() const { return m_fileOpen; }
		inline void PrintData();

		inline void PushDepth();

		inline StringSerializer operator[](const std::string& key);
		inline const std::vector<std::unordered_map<std::string, StringSerializer>>& GetValueMap() const { return m_valueMap; }
		inline size_t GetCurrentDepth() const { return m_currentDepth; }
		inline std::string GetVersion() const { return version; }
	private:
		inline void ParseFile(const std::filesystem::path& path);
		inline void ParseContent(const std::string& content);
	private:
		// Parser
		std::vector<std::unordered_map<std::string, StringSerializer>> m_valueMap;

		size_t m_currentDepth = 0;

		std::string version;

		bool m_fileOpen = false;
	};
}
#include "CppSerializer.inl" 
