/**
 * @file urlpattern.h
 * @brief Declaration for URL Pattern
 */
#ifndef ADA_URL_PATTERN_H
#define ADA_URL_PATTERN_H

#include <optional>
#include <string>
#include <map>
#include <variant>

namespace ada {

struct url_pattern {
	public:
	url_pattern(ada::url_pattern_input input, std::string baseURL, std::optional<ada::url_pattern_options> options);
	url_pattern(std::optional<ada::url_pattern_input> input, std::optional<ada::url_pattern_options> options);
	~url_pattern();

	bool test (std::optional<ada::url_pattern_input> input, std::optional<std::string> baseURL);
	std::optional<ada::url_pattern_result> exec (std::optional<ada::url_pattern_input> input, std::optional<std::string> baseURL);

	// std::string protocol;
	// std::string username;
	// std::string password;
	// std::string hostname;
	// std::string port;
	// std::string pathname;
	// std::string search;
	// std::string hash;

	[[nodiscard]] std::string get_protocol() const noexcept;
	[[nodiscard]] std::string get_username() const noexcept;
	[[nodiscard]] std::string get_password() const noexcept;
	[[nodiscard]] std::string get_hostname() const noexcept;
	[[nodiscard]] std::string get_port() const noexcept;
	[[nodiscard]] std::string get_pathname() const noexcept;
	[[nodiscard]] std::string get_search() const noexcept;
	[[nodiscard]] std::string get_hash() const noexcept;

	bool hasRegExpGroups;

	[[nodiscard]] bool get_hasRegExpGroups() const noexcept;

	private:

		struct inner {
			ada::url_pattern::inner_component protocol;
			ada::url_pattern::inner_component username;
			ada::url_pattern::inner_component password;
			ada::url_pattern::inner_component hostname;
			ada::url_pattern::inner_component port;
			ada::url_pattern::inner_component pathname;
			ada::url_pattern::inner_component search;
			ada::url_pattern::inner_component hash;
		};

		struct inner_component {
			std::string pattern_string;
			std::string reg_exp; // TODO (@Ethan-Arrowood): Replace with actual RegExp type?
			std::vector<std::string> group_name_list;
			bool has_regexp_groups;
		};

};
using url_pattern_input = std::variant<std::string, ada::url_pattern_init>;

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
	std::vector<ada::url_pattern_input> inputs;

	ada::url_pattern_component_result protocol;
	ada::url_pattern_component_result username;
	ada::url_pattern_component_result password;
	ada::url_pattern_component_result hostname;
	ada::url_pattern_component_result port;
	ada::url_pattern_component_result pathname;
	ada::url_pattern_component_result search;
	ada::url_pattern_component_result hash;
};

struct url_pattern_component_result {
	std::string input;
	std::map<std::string, std::string> groups;
};
}
#endif