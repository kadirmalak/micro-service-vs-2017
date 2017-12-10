#include "stdafx.h"
#include <mutex>
#include "user_manager.hpp"
#include <cpprest/details/basic_types.h>

UserDatabase usersDB;
std::mutex usersDBMutex;

void UserManager::signUp(const UserInformation & userInfo) throw(UserManagerException) {

	std::unique_lock<std::mutex> lock{ usersDBMutex };

	if (usersDB.find(userInfo.email) != usersDB.end()) {
		throw UserManagerException("user already exists!");
	}
	usersDB.insert(
		std::pair<utility::string_t, UserInformation>(userInfo.email,
			userInfo));
}

bool UserManager::signOn(
	const utility::string_t email,
	const utility::string_t password,
	UserInformation & userInfo) {

	if (usersDB.find(email) != usersDB.end()) {
		auto ui = usersDB[email];
		if (ui.password == password) {
			userInfo = ui;
			return true;
		}
	}
	return false;
}