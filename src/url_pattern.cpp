#include "ada.h"
#include <string>
#include <vector>
#include <set>
#include <variant>
#include <type_traits>
#include <regex>
#include <fmt/os.h>

namespace ada {
namespace url_pattern {

std::u32string convert_to_u32_string(std::string_view sv) {
  auto utf32_length = ada::idna::utf32_length_from_utf8(sv.data(), sv.size());
  std::u32string input_u32(utf32_length, '\0');
  ada::idna::utf8_to_utf32(sv.data(), sv.size(), input_u32.data());
  return input_u32;
};

/**
 * https://urlpattern.spec.whatwg.org/#url-pattern-create
 * */
ada::url_pattern::url_pattern create(
    ada::url_pattern::url_pattern_input input,
    std::optional<std::string_view> baseURL,
    ada::url_pattern::url_pattern_options options) {
  // TODO: implement
};

/**
 * https://urlpattern.spec.whatwg.org/#url-pattern-match
 */
ada::url_pattern::url_pattern_result match(
    ada::url_pattern::url_pattern_input input,
    std::optional<std::string_view> baseURL) {
  // TODO: implement
};

/**
 * https://urlpattern.spec.whatwg.org/#url-pattern-has-regexp-groups
 */
bool has_regexp_groups(ada::url_pattern::url_pattern url_pattern) {
  return url_pattern.protocol.has_regexp_groups ||
         url_pattern.username.has_regexp_groups ||
         url_pattern.password.has_regexp_groups ||
         url_pattern.hostname.has_regexp_groups ||
         url_pattern.port.has_regexp_groups ||
         url_pattern.pathname.has_regexp_groups ||
         url_pattern.search.has_regexp_groups ||
         url_pattern.hash.has_regexp_groups;
};

/**
 * https://urlpattern.spec.whatwg.org/#encoding-callback
 */
using encoding_callback = std::string(*)(std::string);

/**
 * https://urlpattern.spec.whatwg.org/#options
 */
struct path_to_regexp_options {
  std::string_view delimiter_code_point;
  std::string_view prefix_code_point;
  bool ignore_case;
};

/**
 * https://urlpattern.spec.whatwg.org/#compile-a-component
 */
ada::url_pattern::component compile_component(
    std::string_view input,
    ada::url_pattern::encoding_callback encoding_callback,
    path_to_regexp_options options) {
  // TODO: implement
};

/**
 * https://urlpattern.spec.whatwg.org/#create-a-component-match-result
 */
ada::url_pattern::url_pattern_component_result create_component_match_result(
    ada::url_pattern::component component, std::string_view input,
    std::vector<void> execResult) {
  // TODO: implement
};

/**
 * https://urlpattern.spec.whatwg.org/#default-options
 */
ada::url_pattern::path_to_regexp_options default_options = {
    delimiter_code_point = "",
    prefix_code_point = "",
    ignore_case = false,
};

/**
 * https://urlpattern.spec.whatwg.org/#hostname-options
 */
ada::url_pattern::path_to_regexp_options hostname_options = {
    delimiter_code_point = ".",
    prefix_code_point = "",
    ignore_case = false,
};

/**
 * https://urlpattern.spec.whatwg.org/#pathname-options
 */
ada::url_pattern::path_to_regexp_options pathname_options = {
    delimiter_code_point = "/",
    prefix_code_point = "/",
    ignore_case = false,
};

/**
 * https://urlpattern.spec.whatwg.org/#protocol-component-matches-a-special-scheme
 */
bool protocol_component_matches_special_scheme(
    ada::url_pattern::component protocol_component) {
  std::array<std::string_view, 6> special_scheme_list = {
      "ftp", "file", "http", "https", "ws", "wss"};
  for (const auto& scheme : special_scheme_list) {
    if (std::regex_match(std::string(scheme), protocol_component.reg_exp))
      return true;
  }
  return false;
}

/**
 * https://urlpattern.spec.whatwg.org/#hostname-pattern-is-an-ipv6-address
 */
bool hostname_pattern_is_ipv6_address(std::string_view input) {
  return input.length() < 2 || input[0] == '[' ||
         (input[0] == '{' && input[1] == '[') ||
         (input[0] == '\\' && input[1] == '[');
};

/**
 *
 */
enum parser_state {
  init,
  protocol,
  authority,
  username,
  password,
  hostname,
  port,
  pathname,
  search,
  hash,
  done
};

/**
 * https://urlpattern.spec.whatwg.org/#constructor-string-parser
 */
struct parser {
  std::u32string input;
  ada::url_pattern::token_list token_list;
  ada::url_pattern::url_pattern_init result;
  int component_start = 0;
  int token_index = 0;
  int token_increment = 1;
  int group_depth = 0;
  int hostname_ipv6_bracket_depth = 0;
  bool protocol_matches_special_scheme_flag = false;
  parser_state state;

  void change_state(parser_state new_state, int skip) {
		if (this->state != parser_state::init || this->state != parser_state::authority || this->state != parser_state::done) {
			switch (this->state)
			{
				#define V(state) \
					case parser_state::state: \
						this->result.state = this->make_component_string();
						break;
				URL_PATTERN_COMPONENTS(V)
				#undef V
			default:
				break;
			}
		}

		if (this->state != parser_state::init && new_state != parser_state::done) {
			if (
				(this->state == parser_state::protocol || this->state == parser_state::authority || this->state == parser_state::username || this->state == parser_state::password) &&
				(new_state == parser_state::port || new_state == parser_state::pathname || new_state == parser_state::search || new_state == parser_state::hash) &&
				(this->result.hostname.empty())
			) {
				this->result.hostname = U"";
			}

			if (
				(this->state == parser_state::protocol || this->state == parser_state::authority || this->state == parser_state::username || this->state == parser_state::password || this->state == parser_state::hostname || this->state == parser_state::port) &&
				(new_state == parser_state::search || new_state == parser_state::hash) &&
				(this->result.pathname.empty())
			) {
				if (this->protocol_matches_special_scheme_flag) {
					this->result.pathname = U"/";
				} else {
					this->result.pathname = U"";
				}
			}

			if (
				(this->state == parser_state::protocol || this->state == parser_state::authority || this->state == parser_state::username || this->state == parser_state::password || this->state == parser_state::hostname || this->state == parser_state::port || this->state == parser_state::pathname) &&
				(new_state == parser_state::hash) &&
				(this->result.search.empty())
			) {
				this->result.search = U"";
			}
		}

		this->state = new_state;
		this->token_index += skip;
		this->component_start = this->token_index;
		this->token_increment = 0;
  };

  void rewind() {
    this->token_index = this->component_start;
    this->token_increment = 0;
  };

  void rewind_and_set_state(ada::url_pattern::parser_state state) {
    this->rewind();
    this->state = state;
  };

  ada::url_pattern::token get_safe_token(int index) {
    if (index < this->token_list.size()) return this->token_list[index];
    int last_index = this->token_list.size() - 1;
    ada::url_pattern::token token = this->token_list[last_index];
    return token;
  };

  bool is_non_special_pattern_char(int index, std::u32string_view value) {
    ada::url_pattern::token token = get_safe_token(index);
    if (token.value.compare(value) == 0) return false;
    return token.type == ada::url_pattern::token_type::char_ ||
           token.type == ada::url_pattern::token_type::escaped_char ||
           token.type == ada::url_pattern::token_type::invalid_char;
  };

  bool is_protocol_suffix() {
    return this->is_non_special_pattern_char(this->token_index, U":");
  };

  bool next_is_authority_slashes() {
    return this->is_non_special_pattern_char(this->token_index + 1, U"/") &&
           this->is_non_special_pattern_char(this->token_index + 2, U"/");
  };

  bool is_identity_terminator() {
    return this->is_non_special_pattern_char(this->token_index, U"@");
  };

  bool is_password_prefix() {
    return this->is_non_special_pattern_char(this->token_index, U":");
  };

  bool is_port_prefix() {
    return this->is_non_special_pattern_char(this->token_index, U":");
  };

  bool is_pathname_prefix() {
    return this->is_non_special_pattern_char(this->token_index, U"/");
  };

  bool is_search_prefix() {
    if (this->is_non_special_pattern_char(this->token_index, U"?")) return true;

    if (this->token_list[this->token_index].value != U"?") return false;

    int previous_index = this->token_index - 1;
    if (previous_index < 0) return true;
    token previous_token =
        this->get_safe_token(previous_index);

    return previous_token.type != token_type::name &&
           previous_token.type != token_type::regexp &&
           previous_token.type != token_type::close &&
           previous_token.type != token_type::asterisk;
  }

  bool is_hash_prefix() {
    return this->is_non_special_pattern_char(this->token_index, U"#");
  }

  bool is_group_open() {
    return this->token_list[this->token_index].type ==
           ada::url_pattern::token_type::open;
  }

  bool is_group_close() {
    return this->token_list[this->token_index].type ==
           ada::url_pattern::token_type::close;
  }

  bool is_ipv6_open() {
    return this->is_non_special_pattern_char(this->token_index, U"[");
  }

  bool is_ipv6_close() {
    return this->is_non_special_pattern_char(this->token_index, U"]");
  }

  std::u32string make_component_string() {
    return this->input.substr(this->get_safe_token(this->component_start).index,
                              this->token_list[this->token_index].index);
  };

  void compute_protocol_matches_special_scheme_flag() {
    auto protocol_string = make_component_string();
    auto protocol_component = compile_component(
        protocol_string, canonicalize_protocol(protocol_string),
        default_options);
    if (protocol_component_matches_special_scheme(protocol_component)) {
      this->protocol_matches_special_scheme_flag = true;
    }
  }
};

std::string canonicalize_protocol(std::u32string value) {
  if (value.empty()) return value;
	auto parseResult = ada::parse<ada::url>(value + "://dummy.test", nullptr);
	return parseResult->get_protocol();
}

/**
 * https://urlpattern.spec.whatwg.org/#parse-a-constructor-string
 */
void parse_constructor_string(std::string_view input) {
  auto u32string_input = convert_to_u32_string(input);
  ada::url_pattern::parser parser = {
    input : u32string_input,
    token_list : ada::url_pattern::tokenize(
        u32string_input, ada::url_pattern::tokenize_policy::lenient),
  };

  while (parser.token_index < parser.token_list.size()) {
    parser.token_increment = 1;
    if (parser.token_list[parser.token_index].type ==
        ada::url_pattern::token_type::end) {
      if (parser.state == ada::url_pattern::parser_state::init) {
        // rewind
      }
    }
  }
};

/**
 * https://urlpattern.spec.whatwg.org/#token-list
 */
typedef std::vector<ada::url_pattern::token> token_list;

/**
 * https://urlpattern.spec.whatwg.org/#token-type
 */
enum token_type {
  invalid_char,
  open,
  close,
  regexp,
  name,
  char_,
  escaped_char,
  other_modifier,
  asterisk,
  end
};

/**
 * https://urlpattern.spec.whatwg.org/#token
 */
struct token {
  token_type type;
  int index = 0;
  std::u32string value;

  token(ada::url_pattern::token_type type, int index, std::u32string value)
      : type{type}, index{index}, value{value} {};

  // static token open_token(int index) {
  // 	return token(ada::url_pattern::token_type::open, index, U"(");
  // }

  // static token close_token(int index){
  // 	return token(ada::url_pattern::token_type::close, index, U")");
  // }

  // static token regexp_token(int index, std::u32string value) {
  // 	return token(ada::url_pattern::token_type::regexp, index, value);
  // }
};

/**
 * https://urlpattern.spec.whatwg.org/#tokenize-policy
 */
enum tokenize_policy { strict, lenient };

/**
 * https://urlpattern.spec.whatwg.org/#tokenizer
 */
struct tokenizer {
  std::u32string input;
  ada::url_pattern::tokenize_policy policy =
      ada::url_pattern::tokenize_policy::strict;
  ada::url_pattern::token_list token_list = {};
  int index = 0;
  int next_index = 0;
  char32_t code_point = NULL;

  tokenizer(std::u32string_view input_arg) {
    input = std::u32string(input_arg);
  }
  tokenizer(std::u32string_view input_arg,
            ada::url_pattern::tokenize_policy policy)
      : policy{policy} {
    input = std::u32string(input_arg);
  }

  tokenizer(std::string_view input_arg) {
    input = convert_to_u32_string(input_arg);
  }

  tokenizer(std::string_view input_arg,
            ada::url_pattern::tokenize_policy policy)
      : policy{policy} {
    input = convert_to_u32_string(input_arg);
  }

  /**
   * https://urlpattern.spec.whatwg.org/#get-the-next-code-point
   */
  int get_next_code_point() {
    this->code_point = this->input[this->next_index];
    this->next_index += 1;
    return 0;
  }

  /**
   * https://urlpattern.spec.whatwg.org/#seek-and-get-the-next-code-point
   */
  int seek_and_get_next_code_point(int index) {
    this->next_index = index;
    return get_next_code_point();
  }

  /**
   * https://urlpattern.spec.whatwg.org/#add-a-token
   */
  int add_token(ada::url_pattern::token_type type, int next_position,
                int value_position, int value_length) {
    this->token_list.push_back(token{
        type, this->index, this->input.substr(value_position, value_length)});
    this->index = next_position;
    return 0;
  }

  /**
   * https://urlpattern.spec.whatwg.org/#add-a-token-with-default-length
   */
  int add_token_with_default_length(ada::url_pattern::token_type type,
                                    int next_position, int value_position) {
    return add_token(type, next_position, value_position,
                     next_position - value_position);
  }

  /**
   * https://urlpattern.spec.whatwg.org/#add-a-token-with-default-position-and-length
   */
  int add_token_with_default_position_and_length(
      ada::url_pattern::token_type type) {
    return add_token_with_default_length(type, this->next_index, this->index);
  }

  /**
   * https://urlpattern.spec.whatwg.org/#process-a-tokenizing-error
   */
  int process_tokenizing_error(int next_position, int value_position) {
    if (this->policy == ada::url_pattern::tokenize_policy::strict) {
      fmt::print(stderr,
                 "TypeError: Tokenizer received invalid character while in "
                 "strict mode.\n");
      return EXIT_FAILURE;
    }
    return add_token_with_default_length(
        ada::url_pattern::token_type::invalid_char, next_position,
        value_position);
  }

  /**
   * https://urlpattern.spec.whatwg.org/#is-a-valid-name-code-point
   *
   * TODO: Are the sets valid here?
   */
  bool is_valid_name_code_point(char32_t code_point, bool first) {
    std::set<char32_t> identifier_start_set = {U'$', U'_'};
    std::set<char32_t> identifier_part_set = {U'$'};
    if (first) {
      return identifier_start_set.find(code_point) !=
             identifier_start_set.end();
    }

    return identifier_part_set.find(code_point) != identifier_part_set.end();
  }

  /**
   * https://urlpattern.spec.whatwg.org/#is-ascii
   */
  bool is_ascii(char32_t code_point) {
    return code_point >= U'\u0000' && code_point <= U'\u007F';
  }
};

/**
 * https://urlpattern.spec.whatwg.org/#tokenize
 */
ada::url_pattern::token_list tokenize(
    std::u32string_view input, ada::url_pattern::tokenize_policy policy) {
  ada::url_pattern::tokenizer tokenizer = ada::url_pattern::tokenizer{
      input,
      policy,
  };
  while (tokenizer.index < tokenizer.input.length()) {
    tokenizer.seek_and_get_next_code_point(tokenizer.index);
    if (tokenizer.code_point == U'\u002A' /* `*` */) {
      tokenizer.add_token_with_default_position_and_length(
          ada::url_pattern::token_type::asterisk);
      continue;
    }

    if (tokenizer.code_point == U'\u002B' /* `+` */ ||
        tokenizer.code_point == U'\u003F' /* `?` */) {
      tokenizer.add_token_with_default_position_and_length(
          ada::url_pattern::token_type::other_modifier);
      continue;
    }

    if (tokenizer.code_point == U'\u005C' /* `\` */) {
      if (tokenizer.index == tokenizer.input.length() - 1) {
        tokenizer.process_tokenizing_error(tokenizer.next_index,
                                           tokenizer.index);
        continue;
      }
      int escaped_index = tokenizer.next_index;
      tokenizer.get_next_code_point();
      tokenizer.add_token_with_default_length(
          ada::url_pattern::token_type::escaped_char, tokenizer.next_index,
          escaped_index);
      continue;
    }

    if (tokenizer.code_point == U'\u007B' /* `{` */) {
      tokenizer.add_token_with_default_position_and_length(
          ada::url_pattern::token_type::open);
      continue;
    }

    if (tokenizer.code_point == U'\u007D' /* `}` */) {
      tokenizer.add_token_with_default_position_and_length(
          ada::url_pattern::token_type::close);
      continue;
    }

    if (tokenizer.code_point == U'\u003A' /* `:` */) {
      int name_position = tokenizer.next_index;
      int name_start = name_position;
      while (name_position < tokenizer.input.length()) {
        tokenizer.seek_and_get_next_code_point(name_position);
        bool first_code_point = name_position == name_start;
        bool valid_code_point = tokenizer.is_valid_name_code_point(
            tokenizer.code_point, first_code_point);
        if (!valid_code_point) break;
        name_position = tokenizer.next_index;
      }
      if (name_position <= name_start) {
        tokenizer.process_tokenizing_error(name_start, tokenizer.index);
        continue;
      }
      tokenizer.add_token_with_default_length(
          ada::url_pattern::token_type::name, name_position, name_start);
      continue;
    }

    if (tokenizer.code_point == U'\u0028' /* `(` */) {
      int depth = 1;
      int regexp_position = tokenizer.next_index;
      int regexp_start = regexp_position;
      bool error = false;
      while (regexp_position < tokenizer.input.length()) {
        tokenizer.seek_and_get_next_code_point(regexp_position);

        if (!tokenizer.is_ascii(tokenizer.code_point)) {
          tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
          error = true;
          break;
        }

        if (regexp_position == regexp_start &&
            tokenizer.code_point == U'\u003F' /* `?` */) {
          tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
          error = true;
          break;
        }

        if (tokenizer.code_point == U'\u005C' /* `\` */) {
          if (regexp_position == tokenizer.input.length() - 1) {
            tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
            error = true;
            break;
          }
          tokenizer.get_next_code_point();
          if (!tokenizer.is_ascii(tokenizer.code_point)) {
            tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
            error = true;
            break;
          }
          regexp_position = tokenizer.next_index;
          continue;
        }

        if (tokenizer.code_point == U'\u0029' /* `)` */) {
          depth--;
          if (depth == 0) {
            regexp_position = tokenizer.next_index;
            break;
          }
        } else if (tokenizer.code_point == U'\u0028' /* `(` */) {
          depth++;
          if (regexp_position == tokenizer.input.length() - 1) {
            tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
            error = true;
            break;
          }
          int temporary_position = tokenizer.next_index;
          tokenizer.get_next_code_point();
          if (tokenizer.code_point != U'\u003F' /* `?` */) {
            tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
            error = true;
            break;
          }
          tokenizer.next_index = temporary_position;
        }
        regexp_position = tokenizer.next_index;
      }
      if (error) continue;
      if (depth != 0) {
        tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
        continue;
      }
      int regexp_length = regexp_position - regexp_start - 1;
      if (regexp_position == 0) {
        tokenizer.process_tokenizing_error(regexp_start, tokenizer.index);
        continue;
      }
      tokenizer.add_token(ada::url_pattern::token_type::regexp, regexp_position,
                          regexp_start, regexp_length);
      continue;
    };

    tokenizer.add_token_with_default_position_and_length(
        ada::url_pattern::token_type::char_);
  };

  tokenizer.add_token_with_default_length(ada::url_pattern::token_type::end,
                                          tokenizer.index, tokenizer.index);

  return tokenizer.token_list;
}
}