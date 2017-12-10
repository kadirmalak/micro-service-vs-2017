//
//  Created by Ivan Mejia on 11/28/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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
