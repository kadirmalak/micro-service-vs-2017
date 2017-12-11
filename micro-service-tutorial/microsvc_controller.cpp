//
//  Created by Ivan Mejia on 12/24/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs.
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

#include "stdafx.h"
#include "microsvc_controller.hpp"
#include "user_manager.hpp"


using namespace web;
using namespace http;

void MicroserviceController::initRestOpHandlers() {
	_listener.support(methods::GET, std::bind(&MicroserviceController::handleGet, this, std::placeholders::_1));
	_listener.support(methods::PUT, std::bind(&MicroserviceController::handlePut, this, std::placeholders::_1));
	_listener.support(methods::POST, std::bind(&MicroserviceController::handlePost, this, std::placeholders::_1));
	_listener.support(methods::DEL, std::bind(&MicroserviceController::handleDelete, this, std::placeholders::_1));
	_listener.support(methods::PATCH, std::bind(&MicroserviceController::handlePatch, this, std::placeholders::_1));
}

void MicroserviceController::handleGet(http_request message) {
	auto path = requestPath(message);
	auto params = requestQueryParams(message);
	if (path.size() > 1) {
		if (path[0] == U("service") && path[1] == U("test")) {
			auto response = json::value::object();
			response[U("version")] = json::value::string(U("0.1.1"));
			response[U("status")] = json::value::string(U("ready!"));
			message.reply(status_codes::OK, response);
		}
		else if (path[0] == U("users") && path[1] == U("signon")) {
			
			pplx::create_task([=]() {

				auto headers = message.headers();
				if (headers.find(U("Authorization")) == headers.end()) {
					throw std::exception("could not find Authorization header");
				}

				auto authHeader = headers[U("Authorization")];
				auto credsPos = authHeader.find(U("Basic"));
				if (credsPos == std::string::npos) {
					throw std::exception("could not find Basic Authentication header pos");
				}

				auto base64 = authHeader.substr(credsPos + std::string{ "Basic" }.length() + 1);
				if (base64.empty()) {
					throw std::exception("base64 empty");
				}

				auto chars = utility::conversions::from_base64(base64);
				utility::string_t creds(chars.begin(), chars.end() - 1);
				auto colonPos = creds.find(':');
				if (colonPos == std::string::npos) {
					throw std::exception("could not find ':' in credentials string");
				}

				auto userEmail = creds.substr(0, colonPos);
				auto password = creds.substr(colonPos + 1, chars.size() - colonPos - 1);

				UserManager manager;

				UserInformation userInfo;
				
				if (manager.signOn(userEmail, password, userInfo)) {
					return std::make_tuple(true, userInfo);
				}
				else {
					return std::make_tuple(false, UserInformation{});
				}
				
			}).then([=](pplx::task<std::tuple<bool, UserInformation>> task) {
				
				try {
					auto result = task.get();
					if (std::get<0>(result) == true) {
						auto info = std::get<1>(result);
						auto response = json::value::object();
						response[U("success")] = json::value::string(U("welcome ") + info.name);
						message.reply(status_codes::OK, response);
					}
					else {
						throw std::exception("authorization failed");
					}
				}
				catch (const std::exception & ex) {
					message.reply(status_codes::Unauthorized);
				}
			});
		}
	}
	else {
		message.reply(status_codes::NotFound);
	}
}

void MicroserviceController::handlePatch(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void MicroserviceController::handlePut(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
}

void MicroserviceController::handlePost(http_request message) {
	auto path = requestPath(message);
	if (path.size() > 1) {
		if (path[0] == U("users") && path[1] == U("signup")) {

			message.extract_json().then([=](json::value request) { // value based continuation
				try {

					UserInformation userInfo;
					userInfo.email = request[U("email")].as_string();
					userInfo.password = request[U("password")].as_string();
					userInfo.name = request[U("name")].as_string();
					userInfo.lastName = request[U("lastName")].as_string();

					UserManager manager;
					manager.signUp(userInfo);

					auto response = json::value::object();
					response[U("message")] = json::value::string(U("register success!"));
					message.reply(status_codes::OK, response);
				}
				catch (const UserManagerException & e) {
					message.reply(status_codes::BadRequest, e.what());
				}
				catch (const json::json_exception & e) {
					message.reply(status_codes::BadRequest);
				}
				catch (const std::exception & ex) {
					message.reply(status_codes::BadRequest);
				}
				catch (...) {
					message.reply(status_codes::BadRequest);
				}
			});
		}
	}
	else {
		message.reply(status_codes::NotFound);
	}
	//message.reply(status_codes::NotImplemented, responseNotImpl(methods::POST));
}

void MicroserviceController::handleDelete(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void MicroserviceController::handleHead(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void MicroserviceController::handleOptions(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void MicroserviceController::handleTrace(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void MicroserviceController::handleConnect(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void MicroserviceController::handleMerge(http_request message) {
	message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value MicroserviceController::responseNotImpl(const http::method & method) {

	using namespace json;

	auto response = value::object();
	response[U("serviceName")] = value::string(U("C++ Mircroservice Sample"));
	response[U("http_method")] = value::string(method);

	auto subResponse = value::object();
	subResponse[U("field1")] = value::string(U("some string"));
	subResponse[U("field2")] = value::number(123123);

	response[U("sub")] = subResponse;

	return response;
}
