#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cppconn/resultset.h>

struct Transaction
{
	unsigned int stock_code;
	int quantity;
	double datetime;
	std::string status;

	void loadFromResultSet(sql::ResultSet* result)
	{

	}
};

typedef std::vector<std::unique_ptr<Transaction>> Transactions;

struct TransactionsFactory
{
	static Transactions get(sql::ResultSet* result)
	{
		Transactions transactions;
		while (result->next())
		{
			auto transaction = std::make_unique<Transaction>();
			transaction->loadFromResultSet(result);
			transactions.push_back(std::move(transaction));
		}
		return transactions;
	}
};