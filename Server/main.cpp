#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;


int main()
{
	http_listener listener(L"http://localhost/restdemo");

	//listener.support(methods::GET, handle_get);
	//listener.support(methods::POST, handle_post);
	//listener.support(methods::PUT, handle_put);
	//listener.support(methods::DEL, handle_del);

	//try
	//{
	//	listener
	//		.open()
	//		.then([&listener]() {TRACE(L"\nstarting to listen\n"); })
	//		.wait();

	//	while (true);
	//}
	//catch (exception const & e)
	//{
	//	wcout << e.what() << endl;
	//}

	//return 0;
}