#include<iostream>
#include<chrono>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif


#define ASIO_STANDALONE //not using boost


#include<asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


int main()
{

	asio::error_code ec; //create a asio error code which could be reused later

	//asio needs a space to perform stuff
	//create a "Context" - essentially the platform specific interface
	asio::io_context context; //unique instance of asio, also hides platform specifics

	//Get address of somehwere we wish to connect to
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	//create a resolver to turn dns names into IP addreesss
	asio::ip::tcp::resolver resolver(context);

	auto endpoints = resolver.resolve("amazon.in", "80");

	//create a socket, the context will deliver the implementation
	asio::ip::tcp::socket socket(context); //hook into OS network drivers

	//attempt to connect
	asio::connect(socket, endpoints, ec);

	if (!ec)
	{
		std::cout << "COnnected" << std::endl;

	}
	else
	{
		std::cout << "Failed" << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: amazon.in\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
		//send data, asio buffer is a container


		//bruteforce delay
		/*using namespace std::chrono_literals;
		std::this_thread::sleep_for(800ms);*/

		socket.wait(socket.wait_read);

		size_t bytes = socket.available();
		std::cout << "bytes avaiblable" << bytes << std::endl;

		//read those from the socket
		if (bytes >= 0)
		{
			std::vector<char> vBuffer(bytes);
			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);


			for (auto c : vBuffer)
			{
				std::cout << c;
			}
		}
	}

	return 0;
}

