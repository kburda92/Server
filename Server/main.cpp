#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using namespace std;

int main(int argc, const char **argv)
{
	cout << endl;
	cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "pass1");
		con->setSchema("stockexchange");

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
		while (res->next()) {
			cout << "\t... MySQL replies: ";
			cout << res->getString("_message").c_str() << endl;
			cout << "\t... MySQL says it again: ";
			cout << res->getString(1).c_str() << endl;
		}
		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;

	return EXIT_SUCCESS;
}