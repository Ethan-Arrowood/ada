#include "ada.h"
#include <string>
#include <variant>
#include <type_traits>

namespace ada {
	url_pattern::url_pattern(ada::url_pattern_input input, std::string baseURL, std::optional<ada::url_pattern_options> options) {

	};

	url_pattern::~url_pattern() = default;


	void create(ada::url_pattern_input input, std::optional<std::string> baseURL, std::optional<ada::url_pattern_options> options) {
		ada::url_pattern_init init;
		if (const std::string_view* input_string = std::get_if<std::string>(&input)) {
			init = parse_constructor_string(*input_string);

			if (!baseURL.has_value() && init.protocol.empty()) {
				// throw type error
				return;
			}

			if (baseURL.has_value()) {
				init.set_baseURL(*baseURL);
			}
		} else {
			if (baseURL.has_value()) {
				// throw type error
				return;
			}
			init = std::get<ada::url_pattern_init>(input);
		}
	};

	ada::url_pattern_init parse_constructor_string (std::string_view input) {}

	void url_pattern_init::set_baseURL(const std::string& baseURL) {
		this->baseURL = baseURL;
	}
}