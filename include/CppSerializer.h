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

		// Closes the file associated with the serializer (do not clear the content)
		inline void CloseFile() const;

		// Overloaded operators to serialize various data types.
		template<typename T> Serializer& operator<<(const T& value);
		template<typename T> Serializer& operator<<(T* value);
		Serializer& operator<<(const char* value);

		/* 
		// Extra serialization functionality can be added here using the EXTRA_CPPSERIALIZER_SERIALIZER
		// and EXTRA_CPPSERIALIZER_PARSER macro, e.g:

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
		Vec2f As() const\
		{\
			std::istringstream ss(m_content);\
			Vec2f vec2f;\
			ss >> vec2f.x >> vec2f.y; \
			return vec2f;\
		}
		*/

#ifdef EXTRA_CPPSERIALIZER_SERIALIZER
		EXTRA_CPPSERIALIZER_SERIALIZER;
#endif

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

#ifdef EXTRA_CPPSERIALIZER_PARSER
		EXTRA_CPPSERIALIZER_PARSER
#endif

	private:
		std::string m_content;
	};

	class Parser
	{
	public:
		inline Parser(const std::string& content);
		inline Parser(const std::filesystem::path& path);

		inline bool IsFileOpen() const { return m_fileOpen; }
		inline void PrintData();

		inline void PushDepth();
		inline void PopDepth();

		inline StringSerializer operator[](const std::string& key);
		inline const std::vector<std::unordered_map<std::string, StringSerializer>>& GetValueMap() const { return m_valueMap; }
		inline size_t GetCurrentDepth() const { return m_currentDepth; }
	private:
		inline void ParseFile(const std::filesystem::path& path);
		inline void ParseContent(const std::string& content);
	private:
		// Parser
		std::vector<std::unordered_map<std::string, StringSerializer>> m_valueMap;

		size_t m_currentDepth = 0;

		bool m_fileOpen = false;
	};
}
#include "CppSerializer.inl" 
