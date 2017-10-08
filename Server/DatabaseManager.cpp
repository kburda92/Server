#include "DatabaseManager.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

DatabaseManager & DatabaseManager::getInstance()
{
	static DatabaseManager instance;
	return instance;
}

int DatabaseManager::getTraderId(const std::string & userName)
{
	auto query = conn->prepareStatement("SELECT id FROM traders WHERE username = ?");
	query->setString(1, userName);
	auto result = query->executeQuery();

	int ret = 0;
	if (result->next())
		ret = result->getInt("id");

	delete query;
	delete result;
	return ret;
}

int DatabaseManager::getTraderId(const std::string & userName, const std::string & password)
{
	int id = getTraderId(userName);
	if (!id)
		return 0;
	auto query = conn->prepareStatement("SELECT count(id) as exists FROM traders WHERE id = ? AND password = ?");
	query->setInt(1, id);
	query->setString(2, password);
	auto result = query->executeQuery();
	
	int ret = 0;
	if (result->next())
	{
		if (result->getInt("exists"))
			ret = id;
	}

	delete query;
	delete result;
	return ret;
}

double DatabaseManager::quote(const std::string & userName, const std::string & password, unsigned int stockCode)
{
	if (!getTraderId(userName, password))
		return 0.0;
	auto query = conn->prepareStatement("SELECT last_sale_price FROM stocks WHERE code = ?");
	query->setUInt64(1, stockCode);
	auto result = query->executeQuery();
	
	double ret = 0.0;
	if (result->next())
		ret = result->getDouble("last_sale_price");

	delete query;
	delete result;
	return ret;
}

void DatabaseManager::sell(const std::string & userName, const std::string & password, unsigned int stockCode, int quantity, double price)
{
	if (!getTraderId(userName, password))
		return;
	//auto query = conn->prepareStatement("INSERT INTO test(id) VALUES (?)");
	//query->setUInt64(1, stockCode);
	//auto result = query->executeQuery();
}

void DatabaseManager::buy(const std::string & userName, const std::string & password, unsigned int stockCode, int quantity, double price)
{
	if (!getTraderId(userName, password))
		return;
}

int DatabaseManager::registerTrader(const std::string & userName, const std::string & password)
{
	if (getTraderId(userName))
		return 0;
	auto query = conn->prepareStatement("INSERT INTO traders(username, password) VALUES (?,?)");
	query->setString(1, userName);
	query->setString(2, password);
	auto result = query->executeUpdate();

	delete query;
	return result;
}

Transactions DatabaseManager::transactions(const std::string & userName, const std::string & password)
{
	int id = getTraderId(userName, password);
	if (!id)
		return Transactions();
	auto query = conn->prepareStatement("SELECT stock_code, quantity, datetime, name as status FROM transactions t INNER JOIN transactionstatuses s ON t.status_id = s.id WHERE trader_id = ?");
	query->setUInt64(1, id);
	auto result = query->executeQuery();

	auto transactions = TransactionsFactory::get(result);
	delete query;
	delete result;
	return transactions;
}

Portfolio DatabaseManager::portfolioList(const std::string & userName, const std::string & password)
{
	int id = getTraderId(userName, password);
	if (!id)
		return Portfolio();
	auto query = conn->prepareStatement("SELECT stock_code, quantity, total_cost, last_sale_price FROM portfolios p INNER JOIN stocks s ON p.stock_code = s.code WHERE trader_id = ?");
	query->setUInt64(1, id);
	auto result = query->executeQuery();

	auto portfolio = PortfolioFactory::get(result);
	delete query;
	delete result;
	return portfolio;
}

DatabaseManager::DatabaseManager()
{
	auto driver = get_driver_instance();
	if (!driver)
		throw std::string("MySql driver was not found!");

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("config.ini", pt);

	auto host = pt.get<std::string>("database.host");
	auto user = pt.get<std::string>("database.name");
	auto pass = pt.get<std::string>("database.password");

	conn = driver->connect(host.c_str(), user.c_str(), pass.c_str());
	if(!conn->isValid())
		throw std::string("MySql connection problem");
	
	conn->setSchema("stockexchange");
}

DatabaseManager::~DatabaseManager()
{
	delete conn;
}