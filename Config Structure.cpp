Aserver.hpp
    private:
        --> add "ConfigServer * config[];
        or
        --> modify "std::vector<ListeningSocket*> sockets;
;


Combine ConfigServer and ListeningSocket:

    Instead of having separate lists for ConfigServer and ListeningSocket, consider combining them into a single class or struct. This way, for each server, you have a unified object that contains both its configuration and its listening socket. It provides more clarity and reduces the chances of misalignment between configurations and sockets.

    cpp

class ServerInstance {
public:
    ConfigServer* config;
    ListeningSocket* socket;
    // ... other members and methods ...
};
std::vector<ServerInstance> servers;


Initialization:

    Instead of hardcoding values in the constructor, you can add methods to the AServer class to add servers and their configurations dynamically as you parse the configuration file.

    cpp

void addServer(const ConfigServer& config);