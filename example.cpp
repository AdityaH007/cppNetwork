#include<iostream>
using namespace std;
#ifdef _WIN32
#define WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include<asio.hpp>
#include<asio/ts/buffer.hpp>
#include<asio/ts/internet.hpp>

int main()
{
    try
    {
        // Create IO context - the core ASIO functionality
        asio::io_context context;

        // Create a resolver to turn DNS names into IP addresses
        asio::ip::tcp::resolver resolver(context);

        // Attempt to resolve "example.com"
        auto endpoints = resolver.resolve("example.com", "80");

        // Create a socket
        asio::ip::tcp::socket socket(context);

        // Print connection attempt message
        cout << "Attempting connection to example.com..." << endl;

        // Connect to the first endpoint in the list
        asio::error_code ec;
        asio::connect(socket, endpoints, ec);

        if (!ec)
        {
            cout << "CONNECTED to " << socket.remote_endpoint().address().to_string() << endl;

            if (socket.is_open())
            {
                // Prepare HTTP request
                std::string sRequest =
                    "GET /index.html HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Connection: close\r\n\r\n";

                // Send request
                cout << "Sending request..." << endl;
                socket.write_some(asio::buffer(sRequest), ec);

                if (ec)
                {
                    cout << "Failed to send request: " << ec.message() << endl;
                    return 1;
                }

                // Read response
                cout << "Reading response..." << endl;

                // Simple wait for data
                socket.wait(socket.wait_read, ec);

                if (ec)
                {
                    cout << "Error waiting for response: " << ec.message() << endl;
                    return 1;
                }

                // Check available bytes
                size_t bytes = socket.available();
                cout << "Bytes Available: " << bytes << endl;

                if (bytes > 0)
                {
                    // Create buffer and read data
                    std::vector<char> vBuffer(4096); // Larger buffer for safety
                    size_t bytesRead = socket.read_some(asio::buffer(vBuffer), ec);

                    if (ec && ec != asio::error::eof)
                    {
                        cout << "Read error: " << ec.message() << endl;
                    }
                    else
                    {
                        // Display the response
                        cout << "\n---RESPONSE START---\n";
                        cout.write(vBuffer.data(), bytesRead);
                        cout << "\n---RESPONSE END---\n";

                        // Continue reading if there's more data
                        while (socket.available() > 0)
                        {
                            bytesRead = socket.read_some(asio::buffer(vBuffer), ec);
                            if (!ec)
                                cout.write(vBuffer.data(), bytesRead);
                            else
                                break;
                        }
                    }
                }
                else
                {
                    cout << "No data received" << endl;
                }

                // Close socket gracefully
                socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
                socket.close();
            }
        }
        else
        {
            cout << "Failed to connect: " << ec.message() << endl;
            return 1;
        }
    }
    catch (std::exception& e)
    {
        // Catch and display any exceptions
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    cout << "\nPress Enter to exit..." << endl;
    cin.get();
    return 0;
}