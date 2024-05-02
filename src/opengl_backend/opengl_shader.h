#pragma once

#include <string>
#include <unordered_map>

class Shader
{
public:
	Shader();
	bool init(const std::string& shader_name);
	void use()const;
	template<typename T> void setUniform(const std::string& name, const T val) const;
	template<typename T> void setUniform(const std::string& name, const T val1, const T val2)const;
	template<typename T> void setUniform(const std::string& name, const T val1, const T val2, const T val3)const;

private:

	std::int32_t getLocation(const std::string& name) const;

	bool checkCompileErr();
	bool checkLinkingErr();
	bool compile();
	bool link();
	unsigned int m_vertex_id;
	unsigned int m_fragment_id;
	unsigned int m_id;
	std::string m_vertex_code;
	std::string m_fragment_code;
	mutable std::unordered_map<std::string, std::int32_t> m_location_key;
};


