#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"
#include <regex>
#include <string>

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename); 

    // Additional methods
    void process_message(vector<Client> &clients, const string &sender, const string &receiver,
                              const string &message, int message_limit, const string &sender_port,
                              const string &receiver_port);
    
    Client* get_client_by_id(vector<Client>& clients, const string& client_id);

    void processSendCommand(vector<Client> &clients);

    Client* getClientByMacAddress(vector<Client>& clients, const string& macAddress);

    void increaseHopCount(std::stack<Packet*>& packetStack);

    void processReceiveCommand(vector<Client> &clients);

    PhysicalLayerPacket* extractPhysicalPacket(std::stack<Packet*>& frame);

    void show_frame_info(vector<Client> &clients, const string& client_id, const string& direction, int frame_number);

    void show_queue_info(vector<Client>& clients, const string& client_id, const string& direction);

    std::string getCurrentTimeAsString();

    void print_log(vector<Client>& clients, const string& client_id);
    void printCommandWithHyphens(const std::string& command);
    void printHyphens(int length);

    void assembleAndOutputMessage(vector<Client>& clients, const Client& receiverClient, stack<Packet*>& frameStack);

    bool forwardFrame(Client& forwardingClient, stack<Packet*>& frameStack, vector<Client>& clients);

    Packet* getBottomPacket(std::stack<Packet*>& inputStack);

    void deleteStack(stack<Packet*>& frameStack);

    void extractPackets(std::stack<Packet*>& frameStack,
                    PhysicalLayerPacket*& physicalLayerPacket,
                    NetworkLayerPacket*& networkLayerPacket,
                    TransportLayerPacket*& transportLayerPacket,
                    ApplicationLayerPacket*& applicationLayerPacket);


};


#endif  // NETWORK_H
