/**
 * @file url_pattern.h
 * @brief Declaration for URL Pattern
 */
#ifndef ADA_URL_PATTERN_H
#define ADA_URL_PATTERN_H

#include <optional>
#include <string>
#include <map>
#include <variant>
#include <regex>

namespace ada {
namespace url_pattern {

#define URL_PATTERN_COMPONENTS(V) \
	V(protocol) \
	V(username) \
	V(password) \
	V(hostname) \
	V(port)     \
	V(pathname) \
	V(search)   \
	V(hash)     

struct url_pattern {
	#define V(name) ada::url_pattern::component name;
	  URL_PATTERN_COMPONENTS(V)
	#undef V
};

struct component {
	std::string pattern_string;
	std::regex reg_exp;
	std::vector<std::string> group_name_list;
	bool has_regexp_groups;
};

using url_pattern_input = std::variant<std::string, ada::url_pattern::url_pattern_init>;

struct url_pattern_init {
	#define V(name) std::u32string name;
	  URL_PATTERN_COMPONENTS(V)
	#undef V

	std::u32string baseURL;
};

struct url_pattern_options {
	bool ignoreCase = false;
};

struct url_pattern_result {
	std::vector<ada::url_pattern::url_pattern_input> inputs;

	#define V(name) ada::url_pattern::url_pattern_component_result name;
	  URL_PATTERN_COMPONENTS(V)
	#undef V
};

struct url_pattern_component_result {
	std::string input;
	std::map<std::string, std::string> groups;
};

}
}
#endif