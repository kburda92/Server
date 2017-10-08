#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <functional>
#include "DatabaseManager.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using namespace std;

typedef map<const http::method, const std::function<void(http_request)>> methodMap;
typedef std::vector<std::pair<wstring, json::value>> fieldMap;

std::wstring getFromHeader(http_headers& headers, const std::wstring& valueName)
{
	if (headers.has(valueName))
		auto s = headers[valueName];
	return L"";
}

void quoteRequest(http_request request)
{
	status_code statusCode;
	fieldMap answer;

	auto parameters = uri::split_path(request.absolute_uri().to_string());
	if (parameters.size() < 3)
	{
		request.reply(web::http::status_codes::BadRequest);
		return;
	}

	unsigned long stockCode;
	try
	{
		stockCode = std::stoul(parameters[2]);
	}
	catch (std::exception&)
	{
		request.reply(web::http::status_codes::BadRequest);
		return;
	}

	auto userName = getFromHeader(request.headers(), L"Username");
	auto password = getFromHeader(request.headers(), L"Password");
		
	auto price = DatabaseManager::getInstance().quote(userName, password, stockCode, statusCode);
	if (price)
		answer.push_back(std::make_pair(L"LastSalePrice", price));

	if(answer.empty())
		request.reply(statusCode);
	else
		request.reply(statusCode, json::value::object(answer));
}
void sellRequest(http_request request)
{

}
void buyRequest(http_request request)
{

}
void registerTradeRequest(http_request request)
{

}

void getUsernamePassword(http_request request, status_code& statusCode, std::wstring& username, std::wstring& password)
{
	request.extract_json().then([&statusCode,&username,&password](pplx::task<json::value> task)
	{
		try
		{
			auto & json = task.get();
			if (!json.is_null())
			{
				if (!json.has_field(L"Username") || json.has_field(L"Password"))
					statusCode = status_codes::BadRequest;
				else
				{
					username = json[L"Username"].as_string();
					password = json[L"Password"].as_string();
				}
			}
		}
		catch (http_exception const & e)
		{
			wcout << e.what() << endl;
		}
	}).wait();
}

void transactionsRequest(http_request request)
{
	status_code statusCode;
	std::wstring username, password;

	getUsernamePassword(request, statusCode, username, password);
	if (username.empty() || password.empty())
	{
		request.reply(statusCode); 
		return;
	}

	auto transactions = DatabaseManager::getInstance().transactions(username, password, statusCode);
	if (!transactions)
	{
		request.reply(statusCode);
		return;
	}

	auto answer = transactions->serialize();
	if (answer.is_null())
		request.reply(statusCode);
	else
		request.reply(statusCode, answer);
}
void portfolioRequest(http_request request)
{
	status_code statusCode;
	std::wstring username, password;

	getUsernamePassword(request, statusCode, username, password);
	if (username.empty() || password.empty())
	{
		request.reply(statusCode);
		return;
	}

	auto portfolio = DatabaseManager::getInstance().portfolio(username, password, statusCode);
	if (!portfolio)
	{
		request.reply(statusCode);
		return;
	}

	auto answer = portfolio->serialize();
	if (answer.is_null())
		request.reply(statusCode);
	else
		request.reply(statusCode, answer);
}

int main(int argc, const char **argv)
{
	vector<thread> listenerThreads;
	map<shared_ptr<http_listener>, methodMap> listeners
	{
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/quote/" }),{ { methods::GET, quoteRequest } } },
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/sell/" }),{ { methods::PUT, sellRequest } } },
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/buy/" }),{ { methods::PUT, buyRequest } } },
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/registertrader/" }),{ { methods::POST, registerTradeRequest } } },
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/transactions/" }),{ { methods::GET, transactionsRequest } } },
		{ make_shared<http_listener>(http_listener{ L"http://localhost/stockexchange/portfolio/" }),{ { methods::GET, portfolioRequest } } }
	};

	for (auto& listener : listeners)
	{
		//Before we run listening, we must add methods and handlers we declared earlier
		for (auto& supported_m : listener.second)
		{
			const auto& method = supported_m.first;
			const auto& handler = supported_m.second;
			listener.first->support(method, handler);
		}

		//Start thread for every listener
		listenerThreads.push_back(thread([httpListener = listener.first]()
		{
			httpListener->open()
				.then([&httpListener]() {std::wcout << "Listening at " << httpListener->uri().to_string() << '\n'; })
				.wait();
			while (1);
		}));
	}

	for (auto& thread : listenerThreads)
	{
		thread.join();
	}
	return 0;
}