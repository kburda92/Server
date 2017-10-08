#pragma once
#include <string>
#include "Transactions.h"
#include "Portfolio.h"

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

	int getTraderId(const std::string& userName);
	int getTraderId(const std::string& userName, const std::string& password);

	double quote(const std::string& userName, const std::string& password, unsigned int stockCode);
	void sell(const std::string& userName, const std::string& password, unsigned int stockCode, int quantity, double price);
	void buy(const std::string& userName, const std::string& password, unsigned int stockCode, int quantity, double price);
	int registerTrader(const std::string& userName, const std::string& password);
	Transactions transactions(const std::string& userName, const std::string& password);
	Portfolio portfolioList(const std::string& userName, const std::string& password);
protected:
	DatabaseManager();
	~DatabaseManager();
private:
	sql::Connection* conn;
};

