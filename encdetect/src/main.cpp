/*
 * Copyright (c) 2016 encdetect authors
 * This file is released under the MIT License
 * http://opensource.org/licenses/MIT
 */

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include <dirent.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_ICU
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucsdet.h>
#endif
#ifdef HAVE_UCHARDET
#include <uchardet.h>
#endif

#include <reader_struct.h>
#include <data.h>

#ifdef HAVE_ICU
bool detect_icu(const char *text, unsigned int size) {
	UErrorCode status = U_ZERO_ERROR;
	const UCharsetMatch** matches = NULL;
	int32_t match_count = 0;
	bool is_ok = false;

	std::cout << "ICU\n---" << std::endl;

	UCharsetDetector* detector = ucsdet_open(&status);
	if (status == U_ZERO_ERROR)
		ucsdet_setText(detector, text, size, &status);
	if (status == U_ZERO_ERROR)
		matches = ucsdet_detectAll(detector, &match_count, &status);
	if (match_count > 0) {
		int max_matches = (match_count > 3 ? 3 : match_count);
		for (int i = 0; i < max_matches; i++) {
			const char *enc = ucsdet_getName(matches[i], &status);
			const char *lang = ucsdet_getLanguage(matches[i], &status);
			int32_t conf = ucsdet_getConfidence(matches[i], &status);

			std::cout << (conf > 20 ? " " : "(") << "ENC: " << std::string(enc) <<
				" LANG: " << std::string(lang) << " CONF: " << std::to_string(conf) << "%" <<
				(conf > 20 ? " " : ")") << std::endl;
		}
		is_ok = true;
	}
	std::cout << std::endl;
	ucsdet_close(detector);

	return is_ok;
}
#endif

#ifdef HAVE_UCHARDET
bool detect_uchardet(const char *text, unsigned int size) {
	const char *detected = NULL;
	bool is_ok = false;

	std::cout << "uchardet\n--------" << std::endl;
	uchardet_t det = uchardet_new();
	if (det && (uchardet_handle_data(det, text, size) == 0)) {
		uchardet_data_end(det);
		detected = uchardet_get_charset(det);
		if (detected && !detected[0])
			detected = NULL;
		if (detected) {
			std::cout << "ENC: " << std::string(detected) << std::endl;
			is_ok = true;
		}
	}
	std::cout << std::endl;
	uchardet_delete(det);

	return is_ok;
}
#endif


int main(int argc, const char* argv[]) {
	std::ostringstream text;
	bool force = false;
	bool extended = false;

	/* parse command line arguments */
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if ((arg == "--help") || (arg == "-h")) {
			std::cout << "encdetect - " << std::endl << std::endl;
			std::cout << "Usage: encdetect [ Options ] RPG_RT.ldb" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "  -h, --help             This usage message" << std::endl;
			std::cout << "  -f, --force            Force detection, even if the Database header is different (dangerous!)" << std::endl;
			std::cout << "  -e, --extended         Use additional strings (variables, switches, chipset names) for detection" << std::endl;

			return 0;
		}
		if ((arg == "--force") || (arg == "-f")) {
			force = true;
		}
		if ((arg == "--extended") || (arg == "-e")) {
			extended = true;
		}
	}

	// XXX
	std::string filename = argv[1];

	// load database (FIXME: not using ldb_reader to have our own validation)
	LcfReader reader(filename, "");
	if (!reader.IsOk()) {
		std::cout << "Couldn't find " << filename << " database file." << std::endl;
		return 1;
	}

	std::string header;
	reader.ReadString(header, reader.ReadInt());
	if (header.length() != 11) {
		std::cout << filename << " is not a valid RPG2000 database." << std::endl;
		return 1;
	}

	if (header != "LcfDataBase") {
		std::cout << "Warning: " << filename << " header is not valid and might not be a RPG2000/2003 database." << std::endl;
		if (!force) {
			std::cout << "To ignore this, you need to use the force flag (-f)." << std::endl;
			return 1;
		}
	}
	TypeReader<RPG::Database>::ReadLcf(Data::data, reader, 0);

	text <<
	Data::terms.menu_save <<
	Data::terms.menu_quit <<
	Data::terms.new_game <<
	Data::terms.load_game <<
	Data::terms.exit_game <<
	Data::terms.status <<
	Data::terms.row <<
	Data::terms.order <<
	Data::terms.wait_on <<
	Data::terms.wait_off <<
	Data::terms.level <<
	Data::terms.health_points <<
	Data::terms.spirit_points <<
	Data::terms.normal_status <<
	Data::terms.exp_short <<
	Data::terms.lvl_short <<
	Data::terms.hp_short <<
	Data::terms.sp_short <<
	Data::terms.sp_cost <<
	Data::terms.attack <<
	Data::terms.defense <<
	Data::terms.spirit <<
	Data::terms.agility <<
	Data::terms.weapon <<
	Data::terms.shield <<
	Data::terms.armor <<
	Data::terms.helmet <<
	Data::terms.accessory <<
	Data::terms.save_game_message <<
	Data::terms.load_game_message <<
	Data::terms.file <<
	Data::terms.exit_game_message <<
	Data::terms.yes <<
	Data::terms.no <<
	Data::system.boat_name <<
	Data::system.ship_name <<
	Data::system.airship_name <<
	Data::system.title_name <<
	Data::system.gameover_name <<
	Data::system.system_name <<
	Data::system.system2_name <<
	Data::system.battletest_background <<
	Data::system.frame_name;

	if (text.str().empty()) {
		std::cout << "Database terms are empty, no encoding detection possible!" << std::endl;
		return 1;
	}

	if (extended) {
		for (int i = 0; i < Data::chipsets.size(); i++) {
			if (Data::chipsets[i].name.size() + Data::chipsets[i].chipset_name.size() > 0)
				text <<	Data::chipsets[i].name << Data::chipsets[i].chipset_name;
		}

		for (int i = 0; i < Data::variables.size(); i++) {
			if (!Data::variables[i].name.empty())
				text << Data::variables[i].name;
		}

		for (int i = 0; i < Data::switches.size(); i++) {
			if (!Data::switches[i].name.empty())
				text << Data::switches[i].name;
		}
	}

#ifdef HAVE_ICU
	detect_icu(text.str().c_str(), text.str().size());
#endif
#ifdef HAVE_UCHARDET
	detect_uchardet(text.str().c_str(), text.str().size());
#endif

	return 0;
}
