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

struct url_pattern {
	ada::url_pattern::component protocol;
	ada::url_pattern::component username;
	ada::url_pattern::component password;
	ada::url_pattern::component hostname;
	ada::url_pattern::component port;
	ada::url_pattern::component pathname;
	ada::url_pattern::component search;
	ada::url_pattern::component hash;
};

struct component {
	std::string pattern_string;
	std::regex reg_exp;
	std::vector<std::string> group_name_list;
	bool has_regexp_groups;
};

using url_pattern_input = std::variant<std::string, ada::url_pattern::url_pattern_init>;

struct url_pattern_init {
	std::string protocol;
	std::string username;
	std::string password;
	std::string hostname;
	std::string port;
	std::string pathname;
	std::string search;
	std::string hash;
	std::string baseURL;
	void set_baseURL (const std::string& baseURL);
};

struct url_pattern_options {
	bool ignoreCase = false;
};

struct url_pattern_result {
	std::vector<ada::url_pattern::url_pattern_input> inputs;

	ada::url_pattern::url_pattern_component_result protocol;
	ada::url_pattern::url_pattern_component_result username;
	ada::url_pattern::url_pattern_component_result password;
	ada::url_pattern::url_pattern_component_result hostname;
	ada::url_pattern::url_pattern_component_result port;
	ada::url_pattern::url_pattern_component_result pathname;
	ada::url_pattern::url_pattern_component_result search;
	ada::url_pattern::url_pattern_component_result hash;
};

struct url_pattern_component_result {
	std::string input;
	std::map<std::string, std::string> groups;
};

}
}
#endif