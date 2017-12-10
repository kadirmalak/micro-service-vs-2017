#pragma once

#include <map>
#include "std_micro_service.hpp"
#include <cpprest/details/basic_types.h>

typedef struct {
	utility::string_t email;
	utility::string_t password;
	utility::string_t name;
	utility::string_t lastName;
} UserInformation;

class UserManagerException : public std::exception {
	std::string _message;
public:
	UserManagerException(const std::string & message) :
		_message(message) { }
	const char * what() const throw() {
		return _message.c_str();
	}
};

// alias declaration of our In Memory database...
using UserDatabase = std::map<utility::string_t, UserInformation>;

class UserManager {
public:
	void signUp(const UserInformation & userInfo) throw(UserManagerException);
	bool signOn(const utility::string_t email, const utility::string_t password, UserInformation & userInfo);
};
