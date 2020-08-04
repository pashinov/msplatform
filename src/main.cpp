// system includes
#include <csignal>
#include <iostream>
#include <thread>

// 3rd party includes
#include <args.hxx>

// project includes
#include <phoenix_service/microsvc_controller.hpp>
#include <rest_service/microsvc_controller.hpp>
#include <utils/interrupt_handler.hpp>

void run_service()
{
    rest_service::microservice_controller server;
    server.set_endpoint("http://localhost:9001/api/v1");

    phoenix_service::microsvc_controller phoenix_controller;

    try
    {
        server.accept().wait();
        phoenix_controller.start();

        utils::interrupt_handler::hook_signal(SIGINT);
        utils::interrupt_handler::hook_signal(SIGTERM);
        utils::interrupt_handler::wait_for_signal_interrupt();

        phoenix_controller.stop();
        server.shutdown().wait();
    }
    catch(std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // Parse arguments
    args::ArgumentParser parser("Dragon");
    args::HelpFlag help(parser, "help", "Display help menu", {'h', "help"});
    args::ValueFlag<std::string> config(parser, "config", "Configuration path", {'c'});
    args::Flag daemon(parser, "daemon", "Run as daemon", {'d'});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help&)
    {
        std::cout << parser;
        return EXIT_SUCCESS;
    }
    catch (args::ParseError &ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << parser;
        return EXIT_FAILURE;
    }
    catch (args::ValidationError &ex)
    {
        std::cerr << ex.what() << std::endl;
        std::cerr << parser;
        return EXIT_FAILURE;
    }

    // Load configuration
    std::string config_file;
    if(config)
    {
        config_file = config.Get();
    }
    else
    {
        config_file = "/etc/dragon/dragon.json";
    }

    // Start service
    run_service();

    return EXIT_SUCCESS;
}
