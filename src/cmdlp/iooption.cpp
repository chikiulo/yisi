/**
   This file is part of the command-line option library, which was cloned from:
   https://github.com/masaers/cmdlp (v0.4.2 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#include "iooption.h"
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
// Posix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
// C
#include <climits>
#include <cstdlib>

static bool is_file(const std::string& path) {
	bool result = false;
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0) {
		result = S_ISREG(statbuf.st_mode);
	}
	return result;
}

static bool prefix_match(const std::string& prefix, const char* filename) {
	bool result = true;
	auto it = prefix.begin();
	auto jt = filename;
	while (result && it != prefix.end() && *jt != '\0') {
		result = *it == *jt;
		++it;
		++jt;
	}
	return result && it == prefix.end();
}

static std::string normalize_path(const std::string& path) {
	static thread_local char norm_buffer[PATH_MAX];
	if (realpath(path.c_str(), norm_buffer) == nullptr) {
		throw std::runtime_error("Failed to call realpath.");
	}
	return norm_buffer;
}

bool com::masaers::cmdlp::expand_prefix(const std::string& prefix_path, std::vector<std::string>& filenames) {
	bool result = true;
	std::string dirname;
	std::string fileprefix;
	{
		std::size_t i = prefix_path.find_last_of('/');
		if (i != std::string::npos) {
			// Found a last slash to split on
			dirname    = prefix_path.substr(0, i);
			fileprefix = prefix_path.substr(i + 1);
		} else {
		   // No slashes found
			dirname    = ".";
			fileprefix = prefix_path;
		}
	}
	std::string path = dirname;
	path.push_back('/');
	std::size_t fpos = path.size();
	struct dirent* entry;
	DIR* dir = opendir(dirname.c_str());
	if (dir != nullptr) {
		for (entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
			const char* filename = entry->d_name;
			if (prefix_match(fileprefix, filename)) {
				path.replace(fpos, path.size() - fpos, filename);
				if (is_file(path)) {
					filenames.push_back(normalize_path(path));
				}
			}
		}
		closedir(dir);
	} else {
		result = false;
	}
	return result;
}

#else

bool com::masaers::cmdlp::expand_prefix(const std::string& prefix_path, std::vector<std::string>& filenames) {
#error "Cannot use ifile_prefix on non-POSIX systems.";
	return false;
}

#endif


