/*
 * Copyright (c) 2016-2020 gencache authors
 * This file is released under the ISC License
 * https://opensource.org/licenses/ISC
 */

#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/locid.h>
#ifdef _WIN32
#  include <Windows.h>
#  include "dirent_win.h"
#else
#  include <dirent.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const icu::Normalizer2* icu_normalizer;
const icu::Locale* icu_loc_invariant;

std::string strip_ext(const std::string& in_file) {
	return in_file.substr(0, in_file.find_last_of("."));
}

std::string basename(const std::string& in_file) {
	size_t pos = in_file.find_last_of("/");
	if (pos == std::string::npos) {
		return in_file;
	}
	return in_file.substr(pos + 1);
}

// utf16 woes
#ifdef _WIN32
#  define MYOPENDIR(dir) _wopendir(reinterpret_cast<const wchar_t*>( \
			icu::UnicodeString::fromUTF8(dir).getTerminatedBuffer()))
#  define MYDIRENT struct _wdirent
#  define MYREADDIR _wreaddir
#  define MYCLOSEDIR _wclosedir
#else
#  define MYOPENDIR(dir) opendir(dir.c_str())
#  define MYDIRENT struct dirent
#  define MYREADDIR readdir
#  define MYCLOSEDIR closedir
#endif

json parse_dir_recursive(const std::string& path, const int depth, const bool first = false) {
	json r;
	UErrorCode icu_error = U_ZERO_ERROR;

	/* do not recurse any further */
	if (depth == 0)
		return r;

	auto dir = MYOPENDIR(path);
	if (dir != nullptr) {
		if (!first) {
			r["_dirname"] = basename(path);
		}

		MYDIRENT* dent;
		while ((dent = MYREADDIR(dir)) != nullptr) {
			std::string dirname;
			std::string lower_dirname;
			icu::UnicodeString uni_lower_dirname;

			/* unicode aware lowercase conversion */
#ifdef _WIN32
			icu::UnicodeString(dent->d_name).toUTF8String(dirname);
			uni_lower_dirname = icu::UnicodeString(dent->d_name).toLower(*icu_loc_invariant);
#else
			dirname = std::string(dent->d_name);
			uni_lower_dirname = icu::UnicodeString(dent->d_name, "utf-8").toLower(*icu_loc_invariant);
#endif
			/* normalization */
			icu::UnicodeString normalized_dirname =
				icu_normalizer->normalize(uni_lower_dirname, icu_error);
			if (U_FAILURE(icu_error)) {
				uni_lower_dirname.toUTF8String(lower_dirname);
				std::cerr << "Failed to normalize \"" << lower_dirname << "\"! Using lowercase conversion." << std::endl;
			} else {
				normalized_dirname.toUTF8String(lower_dirname);
			}

			if (dirname == "_dirname") {
				std::cerr << "Skipping _dirname: File conflicts with reserved keyword!" << std::endl;
				continue;
			}

			/* dig deeper, but skip upper and current directory */
			if (dent->d_type == DT_DIR && dirname != ".." && dirname != ".") {
				json temp = parse_dir_recursive(path + "/" + dirname, depth - 1);
				if (!temp.empty()) {
					r[lower_dirname] = temp;
				}
			}

			auto keep_extension = [](const std::string& src) {
				for (const auto& s: {".ini", ".po"}) {
					std::string k = s;
					if (src.length() >= k.size()) {
						if (0 == src.compare(src.length() - k.length(), k.length(), k)) {
							return true;
						}
					}
				}
				return false;
			};

			/* add files */
			if (dent->d_type == DT_REG || dent->d_type == DT_LNK) {
				if (first || keep_extension(lower_dirname)) {
					/* ExFont is a special file in the main directory, needs to be renamed */
					if (strip_ext(lower_dirname) == "exfont") {
						lower_dirname = "exfont";
					}

					r[lower_dirname] = dirname;
				} else {
					r[strip_ext(lower_dirname)] = dirname;
				}
			}
		}
	}
	MYCLOSEDIR(dir);

	return r;
}

int main(int argc, const char* argv[]) {
	struct stat path_info;
	UErrorCode icu_error = U_ZERO_ERROR;

	/* defaults */
	int recursion_depth = 4;
	bool pretty_print = false;
	std::string path = ".";
	std::string output = "index.json";

	/* parse command line arguments */
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if ((arg == "--help") || (arg == "-h")) {
			std::cout << "gencache " PACKAGE_VERSION " - JSON cache generator for EasyRPG Player ports" << std::endl << std::endl;
			std::cout << "Usage: gencache [ Options ] [ Directory ]" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "  -h, --help             This usage message" << std::endl;
			std::cout << "  -p, --pretty           Pretty print the JSON contents" << std::endl;
			std::cout << "  -o, --output <file>    Output file name (default: \"" << output << "\")" << std::endl;
			std::cout << "  -r, --recurse <depth>  Recursion depth (default: " << std::to_string(recursion_depth) << ")" << std::endl << std::endl;
			std::cout << "It uses the current directory if not given as argument." << std::endl;
			return 0;
		} else if ((arg == "--pretty") || (arg == "-p")) {
			pretty_print = true;
		} else if ((arg == "--output") || (arg == "-o")) {
			if (i + 1 < argc) {
				output = argv[++i];
			} else {
				std::cerr << "--output without file name argument." << std::endl;
				return 1;
			}
		} else if ((arg == "--recurse") || (arg == "-r")) {
			if (i + 1 < argc) {
				std::istringstream iss(argv[++i]);
				if (!(iss >> recursion_depth)) {
					std::cerr << "--recurse option needs a number argument." << std::endl;
					return 1;
				}
			} else {
				std::cerr << "--recurse without depth argument." << std::endl;
				return 1;
			}
		} else {
			if (path == ".") {
				if (stat(arg.c_str(), &path_info) != 0) {
					std::cerr << "Cannot access directory: \"" << arg << "\"" << std::endl;
					return 1;
				} else if (!(path_info.st_mode & S_IFDIR)) {
					std::cerr << "Not a directory: \"" << arg << "\"" << std::endl;
					return 1;
				} else {
					path = arg;
				}
			} else {
				std::cerr << "Skipping additional argument: \"" << arg << "\"..." << std::endl;
			}
		}
	}

	icu_normalizer = icu::Normalizer2::getNFKCInstance(icu_error);
	if (U_FAILURE(icu_error)) {
		std::cerr << "Failed to initialize ICU NFKC Normalizer!" << std::endl;
		return 1;
	}

	icu_loc_invariant = &icu::Locale::getRoot();

	/* get directory contents */
	json cache = parse_dir_recursive(path, recursion_depth, true);

	std::time_t t = std::time(nullptr);
	// trigraph ?-escapes
	std::string date = R"(????-??-??)";
	char datebuf[11];
	if (std::strftime(datebuf, sizeof(datebuf), "%F", std::localtime(&t)))
		date = std::string(datebuf);

	/* add metadata */
	json out = {
		{
			"metadata", {
				{ "version", 2 },
				{ "date", date }
			}
		},
		{
			"cache", cache
		}
	};

	/* write to cache file */
	std::ofstream cache_file;
	cache_file.open(output);
	cache_file << out.dump(pretty_print ? 2 : -1);
	cache_file.close();
	std::cout << "JSON cache has been written to \"" << output << "\"." << std::endl;

	return 0;
}
