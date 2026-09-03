
#ifndef CHECKPOINT_STL_PROTOS_HH
#define CHECKPOINT_STL_PROTOS_HH

#include <string>
#include <cctype>
#include "trick/memorymanager_c_intf.h"

inline std::string checkpoint_stl_name_encode(const std::string& name) {
	std::string encoded_name;
	encoded_name.reserve(name.size());

	for (const char character : name) {
		if (character == '_') {
			encoded_name += "__";
		} else if (std::ispunct(static_cast<unsigned char>(character))) {
			encoded_name += '_';
		} else {
			encoded_name += character;
		}
	}

	return encoded_name;
}

inline std::string checkpoint_stl_name_encode_legacy(const std::string& name) {
	std::string encoded_name;
	encoded_name.reserve(name.size());

	for (const char character : name) {
		if (std::ispunct(static_cast<unsigned char>(character))) {
			encoded_name += '_';
		} else {
			encoded_name += character;
		}
	}

	return encoded_name;
}

inline bool checkpoint_stl_allocation_exists(const std::string& object_name, const std::string& var_name) {
	const std::string base_name = object_name + "_" + var_name;
	return TMM_var_exists(base_name.c_str()) ||
		   TMM_var_exists((base_name + "_keys").c_str()) ||
		   TMM_var_exists((base_name + "_first").c_str());
}

// prototype of functions used in checkpoint_stl templates

std::string stl_type_name_convert(std::string in_type) ;

#endif
