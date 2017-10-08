#include "DatabaseManager.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <cpprest/asyncrt_utils.h>

DatabaseManager & DatabaseManager::getInstance()
{
	static DatabaseManager instance;
	return instance;
}

int DatabaseManager::getTraderId(const std::wstring & userName)
{
	auto query = conn->prepareStatement("SELECT id FROM traders WHERE username = ?");
	query->setString(1, utility::conversions::utf16_to_utf8(userName).c_str());
	auto result = query->executeQuery();

	int ret = 0;
	if (result->next())
		ret = result->getInt("id");

	delete query;
	delete result;
	return ret;
}

int DatabaseManager::getTraderId(const std::wstring & userName, const std::wstring & password)
{
	int id = getTraderId(userName);
	if (!id)
		return 0;
	auto query = conn->prepareStatement("SELECT count(id) as \"exists\" FROM traders WHERE id = ? AND password = ?;");
	query->setInt(1, id);
	query->setString(2, utility::conversions::utf16_to_utf8(password).c_str());
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

double DatabaseManager::quote(const std::wstring & userName, const std::wstring & password, unsigned int stockCode, web::http::status_code& statusCode)
{
	if (!getTraderId(userName, password))
	{
		statusCode = web::http::status_codes::Unauthorized;
		return 0.0;
	}
	auto query = conn->prepareStatement("SELECT last_sale_price FROM stocks WHERE code = ?");
	query->setUInt64(1, stockCode);
	auto result = query->executeQuery();
	
	double ret = 0.0;
	if (result->next())
	{
		statusCode = web::http::status_codes::OK;
		ret = result->getDouble("last_sale_price");
	}
	else
		statusCode = web::http::status_codes::NotFound;

	delete query;
	delete result;
	return ret;
}

void DatabaseManager::sell(const std::wstring & userName, const std::wstring & password, unsigned int stockCode, int quantity, double price, web::http::status_code& statusCode)
{
	if (!getTraderId(userName, password))
		return;
	//auto query = conn->prepareStatement("INSERT INTO test(id) VALUES (?)");
	//query->setUInt64(1, stockCode);
	//auto result = query->executeQuery();
}

void DatabaseManager::buy(const std::wstring & userName, const std::wstring & password, unsigned int stockCode, int quantity, double price, web::http::status_code& statusCode)
{
	if (!getTraderId(userName, password))
		return;
}

int DatabaseManager::registerTrader(const std::wstring & userName, const std::wstring & password, web::http::status_code& statusCode)
{
	if (getTraderId(userName))
		return 0;
	auto query = conn->prepareStatement("INSERT INTO traders(username, password) VALUES (?,?)");
	query->setString(1, utility::conversions::utf16_to_utf8(userName).c_str());
	query->setString(2, utility::conversions::utf16_to_utf8(password).c_str());
	auto result = query->executeUpdate();

	delete query;
	return result;
}

std::unique_ptr<ISerializable> DatabaseManager::transactions(const std::wstring & userName, const std::wstring & password, web::http::status_code& statusCode)
{
	int id = getTraderId(userName, password);
	if (!id)
	{
		statusCode = web::http::status_codes::Unauthorized;
		return nullptr;
	}
	auto query = conn->prepareStatement("SELECT stock_code, quantity, datetime, name as status FROM transactions t INNER JOIN transactionstatuses s ON t.status_id = s.id WHERE trader_id = ?");
	query->setUInt64(1, id);
	auto result = query->executeQuery();

	std::unique_ptr<ISerializable> transactions = std::make_unique<Transactions>(result);
	delete query;
	delete result;
	statusCode = web::http::status_codes::OK;
	return transactions;
}

std::unique_ptr<ISerializable> DatabaseManager::portfolio(const std::wstring & userName, const std::wstring & password, web::http::status_code& statusCode)
{
	int id = getTraderId(userName, password);
	if (!id)
	{
		statusCode = web::http::status_codes::Unauthorized;
		return nullptr;
	}
	auto query = conn->prepareStatement("SELECT stock_code, quantity, total_cost, last_sale_price FROM portfolios p INNER JOIN stocks s ON p.stock_code = s.code WHERE trader_id = ?");
	query->setUInt64(1, id);
	auto result = query->executeQuery();

	std::unique_ptr<ISerializable> portfolio = std::make_unique<Portfolio>(result);
	delete query;
	delete result;
	statusCode = web::http::status_codes::OK;
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