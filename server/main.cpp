#include <boost/asio.hpp>
#include <iostream>

#include "server/server.h"

int main(int argc, char* argv[]) {
    try {
        auto const address = boost::asio::ip::make_address("0.0.0.0");
        unsigned short port = 8080;
        if (argc > 1) {
            port = static_cast<unsigned short>(std::stoi(argv[1]));
        }

        boost::asio::io_context ioc{1};
        seka::server::Server server(ioc, {address, port});
        server.run();
        std::cout << "Seka server listening on " << port << std::endl;
        ioc.run();
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
