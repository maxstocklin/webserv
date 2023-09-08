#include "../Includes/TestServer.hpp"

int main(int ac, char **av)
{
    if (ac != 1)
    {
        std::cerr << "Webserv requires a config file --> ./webserv [config_file]" << std::endl;
    }
    (void)av;
    TestServer test;
}
