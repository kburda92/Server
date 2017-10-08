#pragma once
#include <string>
#include <cpprest/http_msg.h>
#include "Transactions.h"

namespace sql
{
	class Connection;
}
class DatabaseManager
{
public:
	DatabaseManager(DatabaseManager const&) = delete;
	DatabaseManager(DatabaseManager&&) = delete;
	DatabaseManager& operator=(DatabaseManager const&) = delete;
	DatabaseManager& operator=(DatabaseManager &&) = delete;
	
	static DatabaseManager& getInstance();

	int getTraderId(const std::wstring& userName);
	int getTraderId(const std::wstring& userName, const std::wstring& password);

	double quote(const std::wstring& userName, const std::wstring& password, unsigned int stockCode, web::http::status_code& statusCode);
	void sell(const std::wstring& userName, const std::wstring& password, unsigned int stockCode, int quantity, double price, web::http::status_code& statusCode);
	void buy(const std::wstring& userName, const std::wstring& password, unsigned int stockCode, int quantity, double price, web::http::status_code& statusCode);
	int registerTrader(const std::wstring& userName, const std::wstring& password, web::http::status_code& statusCode);
	std::unique_ptr<ISerializable> transactions(const std::wstring& userName, const std::wstring& password, web::http::status_code& statusCode);
	std::unique_ptr<ISerializable> portfolio(const std::wstring& userName, const std::wstring& password, web::http::status_code& statusCode);
protected:
	DatabaseManager();
	~DatabaseManager();
private:
	sql::Connection* conn;
};

