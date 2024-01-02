//
// Created by alperen on 27.09.2023.
//

#include "Client.h"

Client::Client(string const& _id, string const& _ip, string const& _mac) {
    client_id = _id;
    client_ip = _ip;
    client_mac = _mac;
}

ostream &operator<<(ostream &os, const Client &client) {
    os << "client_id: " << client.client_id << " client_ip: " << client.client_ip << " client_mac: "
       << client.client_mac << endl;
    return os;
}

void Client::createFrame(vector<Client>& clients, const Client &clientSender, const Client &clientReceiver, const std::string& message, int max_msg_size,
 const string &sender_port, const string &receiver_port) {
    // Calculate the number of frames needed based on the message size and max_msg_size
    int num_frames = message.length() / max_msg_size + 1;
    if (message.length() % max_msg_size == 0) {
        num_frames--;
    }
    
    cout<<"Message to be sent: \""<< message << "\""<<endl;
    cout<<endl;
    // Iterate for each frame
    for (int i = 0; i < num_frames; ++i) {
        // Calculate the start and end index for the current message chunk
        int start_index = i * max_msg_size;
        int end_index = std::min((i + 1) * max_msg_size, static_cast<int>(message.length()));
        // Extract the current message chunk
        std::string message_chunk = message.substr(start_index, end_index - start_index);
        Client* nextHopClient = getNextHopClient(clients, clientSender, clientReceiver);
        // Create packets for each layer

        Packet* application_packet = new ApplicationLayerPacket(0, clientSender.client_id, clientReceiver.client_id, message_chunk);
        Packet* transport_packet = new TransportLayerPacket(1, sender_port, receiver_port);
        Packet* network_packet = new NetworkLayerPacket(2, clientSender.client_ip, clientReceiver.client_ip);
        Packet* physical_packet = new PhysicalLayerPacket(3, clientSender.client_mac, nextHopClient->client_mac);
        
        // Set the frame number in each packet
        application_packet->frame_number = i+1;
        transport_packet->frame_number = i+1;
        network_packet->frame_number = i+1;
        physical_packet->frame_number = i+1;

        // Set the frame number in each packet
        application_packet->hops_count = 0;
        transport_packet->hops_count = 0;
        network_packet->hops_count = 0;
        physical_packet->hops_count = 0;

        // Add the packets as a stack to represent the frame
        std::stack<Packet*> frame_stack;
        frame_stack.push(application_packet);
        frame_stack.push(transport_packet);
        frame_stack.push(network_packet);
        frame_stack.push(physical_packet);



        // Add the frame stack to the outgoing_queue
        outgoing_queue.push(frame_stack);
        cout<<"Frame #"<< i+1 << endl;
        // Iterate through frame_stack and output information using packet->print()
        while (!frame_stack.empty()) {
            Packet* packet = frame_stack.top();
            frame_stack.pop();

            // Assuming packet has a print method
            packet->print();
        }
        cout<< "Number of hops so far: 0"<<endl;

        cout<<"--------"<<endl;
    }
    //printOutgoingQueue(clientSender);
}

void Client::printOutgoingQueue(const Client &client) const {
    queue<stack<Packet*>> copy;

    // Create a deep copy of each frame stack in the outgoing_queue
    queue<stack<Packet*>> tempQueue = client.outgoing_queue;
    std::cout << "Outgoing Queue for Client " << client_id << ":" << std::endl;

    while (!tempQueue.empty()) {
        

        // Access the frame stack
        const std::stack<Packet*>& frame_stack = tempQueue.front();
        
        std::cout << "Frame " << frame_stack.top()->frame_number << ":" << std::endl;
        frame_stack.top()->print();
        // Pop the frame stack from the outgoing_queue
        tempQueue.pop();
        
    }
}

Client* Client::getNextHopClient(vector<Client>& clients, const Client &clientSender, const Client &clientReceiver) {
    // Iterate through the routing table
    for (const auto& entry : clientSender.routing_table) {
        // Check if the entry matches the clientReceiver's ID


        if (entry.first == clientReceiver.client_id) {
            // Get the next hop client ID from the entry
            const string& nextHopClientID = entry.second;

            // Find the next hop client by its ID
            for (Client& client : clients) {  // Make sure 'clients' is accessible
                if (client.client_id == nextHopClientID) {
                    return &client; // Return a pointer to the next hop client
                }
            }
            return nullptr; // Return nullptr if the next hop client is not found
        }
    }
    return nullptr; // Return nullptr if the entry is not found
}


Client::~Client() {
    // TODO: Free any dynamically allocated memory if necessary.
    // Free dynamically allocated memory for packets in the outgoing_queue
    while (!outgoing_queue.empty()) {
        std::stack<Packet*>& frame_stack = outgoing_queue.front();

        // Pop and delete each packet in the frame stack
        while (!frame_stack.empty()) {
            Packet* packet = frame_stack.top();
            frame_stack.pop();
            delete packet;
        }

        // Pop the frame stack from the outgoing_queue
        outgoing_queue.pop();
    }

    while (!incoming_queue.empty()) {
        std::stack<Packet*>& frame_stack1 = incoming_queue.front();

        // Pop and delete each packet in the frame stack
        while (!frame_stack1.empty()) {
            Packet* packet = frame_stack1.top();
            frame_stack1.pop();
            delete packet;
        }

        // Pop the frame stack from the incoming_queue
        incoming_queue.pop();
    }
}
