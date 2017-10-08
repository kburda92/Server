#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include "DatabaseManager.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using namespace std;

int main(int argc, const char **argv)
{
	auto& db = DatabaseManager::getInstance();
}