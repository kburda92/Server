#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cppconn/resultset.h>

struct Stock
{
	void loadFromResultSet(sql::ResultSet* result)
	{

	}
};

typedef std::vector<std::unique_ptr<Stock>> Portfolio;

struct PortfolioFactory
{
	static Portfolio get(sql::ResultSet* result)
	{
		Portfolio portfolio;
		while (result->next())
		{
			auto stock = std::make_unique<Stock>();
			stock->loadFromResultSet(result);
			portfolio.push_back(std::move(stock));
		}
		return portfolio;
	}
};
