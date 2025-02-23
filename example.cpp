#include <iostream>
#include <chrono>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Target Windows 10 and above
#endif
#define ASIO_STANDALONE //not using boost
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// Creating a large buffer for receiving network data
std::vector<char> vBuffer(20 * 1024);

// Recursive function to continuously read data from the socket
void GrabData(asio::ip::tcp::socket& socket)
{
    // Set up an asynchronous read operation
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";
                std::cout.write(vBuffer.data(), length);
                // Recursively call GrabData to continue reading
                GrabData(socket);
            }
            else
            {
                std::cout << "\nRead error: " << ec.message() << "\n";
            }
        }
    );
}

int main()
{
    try
    {
        // Create a "Context" - essentially the platform specific interface
        // Unique instance of asio, also hides platform specifics
        asio::io_context context;

        // Create a resolver to turn DNS names into IP addresses
        asio::ip::tcp::resolver resolver(context);
        auto endpoints = resolver.resolve("example.com", "80");

        // Create a socket, the context will deliver the implementation
        // This hooks into OS network drivers
        asio::ip::tcp::socket socket(context);

        // Attempt to connect to the resolved endpoints
        asio::connect(socket, endpoints);

        if (socket.is_open())
        {
            // Set up asynchronous read operation before sending request
            GrabData(socket);

            // Prepare and send HTTP request
            // Using proper HTTP/1.1 format with required headers
            std::string sRequest =
                "GET /index.html HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Connection: close\r\n\r\n";

            // Send data using ASIO buffer as a container
            socket.write_some(asio::buffer(sRequest));

            // Run the IO context - this is required for async operations
            // This call will block until all async operations are complete
            context.run();
        }
    }
    catch (const std::exception& e)
    {
        // Handle any exceptions that might occur during execution
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}