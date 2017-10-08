#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cppconn/resultset.h>
#include <cpprest/json.h>

struct ISerializable
{
	virtual web::json::value serialize() = 0;
};

struct Transactions : ISerializable
{
	struct Transaction 
	{
		unsigned int stock_code;
		int quantity;
		double datetime;
		std::string status;
	};
	Transactions(sql::ResultSet* result)
	{
		while (result->next())
		{
			auto transaction = std::make_unique<Transaction>();
			transaction->stock_code = result->getUInt("stock_code");
			transaction->quantity = result->getUInt("stock_code");
			transaction->datetime = result->getDouble("stock_code");
			transaction->status = result->getString("status");
			m_transactions.push_back(std::move(transaction));
		}
	}
	web::json::value serialize()
	{
		web::json::value transactions;
		int i = 0;
		for (auto it = std::begin(m_transactions); it != std::end(m_transactions); ++it)
		{
			web::json::value transaction;
			transaction[L"stock_code"] = (*it)->stock_code;
			transaction[L"quantity"] = (*it)->quantity;
			transaction[L"datetime"] = (*it)->datetime;
			//transaction[L"status"] = web::json::value::parse((*it)->status);
			transactions[i++] = transaction;
		}
		return transactions;
	}
private:
	std::vector<std::unique_ptr<Transaction>> m_transactions;
};

struct Portfolio : ISerializable
{
	struct Stock
	{

	};
	Portfolio(sql::ResultSet* result)
	{
		while (result->next())
		{
			auto stock = std::make_unique<Stock>();
			//transaction->stock_code = result->getUInt("stock_code");
			//transaction->quantity = result->getUInt("stock_code");
			//transaction->datetime = result->getDouble("stock_code");
			//transaction->status = result->getString("status");
			m_stocks.push_back(std::move(stock));
		}
	}
	web::json::value serialize()
	{
		web::json::value stocks;
		int i = 0;
		for (auto it = std::begin(m_stocks); it != std::end(m_stocks); ++it)
		{
			//web::json::value transaction;
			//transaction[L"stock_code"] = (*it)->stock_code;
			//transaction[L"quantity"] = (*it)->quantity;
			//transaction[L"datetime"] = (*it)->datetime;
			////transaction[L"status"] = (*it)->status;
			////transaction[L"status"] = "cos";
			//transactions[i++] = transaction;
		}
		return stocks;
	}
private:
	std::vector<std::unique_ptr<Stock>> m_stocks;
};
