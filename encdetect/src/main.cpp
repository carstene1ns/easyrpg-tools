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
#endif
#ifdef HAVE_ENCA
#include <enca.h>
#endif
#ifdef HAVE_GUESS
#include <libguess.h>
#endif

#include "ldb_reader.h"
#include "lmt_reader.h"
#include "lmu_reader.h"
#include "lsd_reader.h"
#include "reader_util.h"

int main(int argc, const char* argv[]) {

	/* parse command line arguments */
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if ((arg == "--help") || (arg == "-h")) {
			std::cout << "encdetect - " << std::endl << std::endl;
			std::cout << "Usage: encdetect [ Options ] RPG_RT.ldb" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "  -h, --help             This usage message" << std::endl;
			return 0;
		}
	}

	return 0;
}
