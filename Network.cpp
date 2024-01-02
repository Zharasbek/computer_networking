#include "Network.h"


Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                      const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */

    for (const string &command : commands) {
        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "MESSAGE") {
            // Define the regular expression pattern
            std::regex pattern(R"((\w+)\s+(\w+)\s+(\w+)\s+#(.+)#)");
            
            // Create a match object
            std::smatch matches;
            std::string sender, receiver, message, message_part;

            // Try to match the pattern in the input string
            if (std::regex_match(command, matches, pattern)) {
                // Extract matched groups
                message_part = matches[1];
                sender = matches[2];
                receiver = matches[3];
                message = matches[4];
            } else {
                std::cout << "No match found." << std::endl;
                continue;
            }
            printCommandWithHyphens(command);
            // Call the method to process the MESSAGE command
            process_message(clients, sender, receiver, message, message_limit, sender_port, receiver_port);
        } else if (cmd == "SHOW_FRAME_INFO") {
            
            string client_id, direction;
            int frame_number;
            iss >> client_id >> direction >> frame_number;

            printCommandWithHyphens(command);

            // Call the method to show frame information
            show_frame_info(clients, client_id, direction, frame_number);
        } else if (cmd == "SHOW_Q_INFO") {
            
            string client_id, direction;
            iss >> client_id >> direction;

            printCommandWithHyphens(command);

            // Call the method to show queue information
            show_queue_info(clients, client_id, direction);
        } else if (cmd == "SEND") {
            printCommandWithHyphens(command);
            // Call the method to simulate sending frames
            processSendCommand(clients);
        } else if (cmd == "RECEIVE") {
            printCommandWithHyphens(command);
            // Call the method to simulate receiving frames
            processReceiveCommand(clients);
        } else if (cmd == "PRINT_LOG") {
            printCommandWithHyphens(command);
            string client_id;
            iss >> client_id;

            // Call the method to print log information
            print_log(clients, client_id);
        } else{
            printCommandWithHyphens(command);
            cout << "Invalid command." << endl;            
        }
    }

}

void Network::printHyphens(int length) {
    for (int i = 0; i < length; ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;
}

void Network::printCommandWithHyphens(const std::string& command) {
    int numberOfHyphens = 9 + command.length();

    // Print hyphens before the command
    printHyphens(numberOfHyphens);

    // Print the command
    std::cout << "Command: " << command << std::endl;

    // Print hyphens after the command
    printHyphens(numberOfHyphens);
}

void Network::process_message(vector<Client> &clients, const string &sender, const string &receiver,
                              const string &message, int message_limit, const string &sender_port,
                              const string &receiver_port) {
    // Find the sender client in the vector
    Client* senderClient = get_client_by_id(clients, sender);
    // Find the receiver client in the vector
    Client* receiverClient = get_client_by_id(clients, receiver);
    senderClient->createFrame( clients, *senderClient, *receiverClient, message, message_limit, sender_port, receiver_port);
}

// Helper function to find a client by ID
Client* Network::get_client_by_id(vector<Client>& clients, const string& client_id) {
    for (Client& client : clients) {
        if (client.client_id == client_id) {
            return &client; // Return a pointer to the client if found
        }
        
    }
    return nullptr; // Return nullptr if the client is not found
}


void Network::processSendCommand(vector<Client> &clients) {
    
    for (Client &sender : clients) {
        std::string messageFull = "";
        // Iterate over all clients in the network
        while (!sender.outgoing_queue.empty()) {
            bool found = false;
            // Dequeue a frame from the outgoing queue
            stack<Packet*> frame = sender.outgoing_queue.front();
            stack<Packet*> second_frame = sender.outgoing_queue.front();
            
            string receiverMac;
            // Extract necessary information from the frame
            Packet *physicalPacket = frame.top();  // The Physical Layer Packet
            // Check if the dynamic type is PhysicalLayerPacket
            PhysicalLayerPacket* physicalLayerPacket = dynamic_cast<PhysicalLayerPacket*>(physicalPacket);
            ApplicationLayerPacket* applicationLayerPacket = dynamic_cast<ApplicationLayerPacket*>(getBottomPacket(frame));

            if (physicalLayerPacket) {
                // Successfully casted to PhysicalLayerPacket, now you can access its members
                receiverMac = physicalLayerPacket->receiver_MAC_address;
                // Use receiverID as needed
            } else {
                // Handle the case where the dynamic cast failed
                cerr << "Error: Unable to cast to PhysicalLayerPacket." << endl;
            }
            int frameNumber = physicalPacket->frame_number;
            int hopsNumber = second_frame.top()->hops_count;

            Client* nextHopClient = getClientByMacAddress(clients, receiverMac);
            if (nextHopClient==nullptr) {
                cerr << "Error: Unable to find the next hop client." << endl;
                
            }

            increaseHopCount(frame);
            // Log the transmission information
            messageFull = messageFull + applicationLayerPacket->message_data;
            cout << "Client " << sender.client_id << " sending frame #" << frameNumber << " to client " << nextHopClient->client_id << endl;
            second_frame.top()->print();
            second_frame.pop();
            second_frame.top()->print();
            second_frame.pop();
            second_frame.top()->print();
            second_frame.pop();
            second_frame.top()->print();
            
            cout << "Number of hops so far: " << second_frame.top()->hops_count << endl;
            second_frame.pop();

            cout << "--------" << endl;

            // Add the frame to the incoming queue of the next hop
            nextHopClient->incoming_queue.push(frame);
            sender.outgoing_queue.pop();

            // receiverClient->log_entries.emplace_back(getCurrentTimeAsString(), messageFull, frameNumber, hopsNumber,
            // applicationLayerPacket->sender_ID, applicationLayerPacket->receiver_ID, true, ActivityType::MESSAGE_RECEIVED);

            Packet* tempPacket;
            if (!sender.outgoing_queue.empty()){
                tempPacket = sender.outgoing_queue.front().top();                        
            }
            if (sender.outgoing_queue.empty() || tempPacket->frame_number==1){
                // cout << "SIZE OF THE LOGS FROM THE SEND FUNCTION" << sender.log_entries.size() << endl;
                for (const Log& log : sender.log_entries) {
                    if (log.activity_type == ActivityType::MESSAGE_FORWARDED && log.number_of_frames==frameNumber && log.number_of_hops==hopsNumber && log.sender_id==applicationLayerPacket->sender_ID && log.receiver_id==applicationLayerPacket->receiver_ID) {
                        messageFull = "";
                        found = true;
                    }
                }
                if (found==false){
                    sender.log_entries.emplace_back(getCurrentTimeAsString(), messageFull, frameNumber, hopsNumber, applicationLayerPacket->sender_ID, applicationLayerPacket->receiver_ID, true, ActivityType::MESSAGE_SENT);

                }
                
                messageFull = "";
            }
            
            deleteStack(second_frame);
        }
    }
}

void Network::processReceiveCommand(vector<Client> &clients) {
    for (Client& receivingClient : clients) {
        std::string fullMessage = "";
        while (!receivingClient.incoming_queue.empty()) {
            
            stack<Packet*>& frameStack = receivingClient.incoming_queue.front();
            ApplicationLayerPacket* appPacket = dynamic_cast<ApplicationLayerPacket*>(getBottomPacket(frameStack));
            PhysicalLayerPacket* physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frameStack.top());
            Client* previousSender = getClientByMacAddress(clients, physicalPacket->sender_MAC_address);
            int hops_count = frameStack.top()->hops_count;
            int frame_number = frameStack.top()->frame_number;
            std::string sender_ID = appPacket->sender_ID;
            std::string receiver_ID = appPacket->receiver_ID;
            
            if (appPacket) {
                if (appPacket->receiver_ID == receivingClient.client_id) {
                    fullMessage = fullMessage + appPacket->message_data;
                    assembleAndOutputMessage(clients, receivingClient, frameStack);
                    
                    receivingClient.incoming_queue.pop();
                    Packet* tempPacket;
                    if (!receivingClient.incoming_queue.empty()){
                        tempPacket = receivingClient.incoming_queue.front().top();                        
                    }
                    if (receivingClient.incoming_queue.empty() || tempPacket->frame_number==1){
                        if (fullMessage.length()>0){
                            cout << "Client " << receivingClient.client_id << " received the message " <<"\""<< fullMessage <<"\"" << " from client "<< sender_ID << "." << endl;
                        }
                        cout << "--------" << endl;
                        // cout << "MESSAGE RECEIVED AND LOG HAS BEEN CREATED" << endl;
                        
                        receivingClient.log_entries.emplace_back(getCurrentTimeAsString(), fullMessage, frame_number, hops_count, sender_ID, receiver_ID, true, ActivityType::MESSAGE_RECEIVED);
                        // cout << "SIZE OF THE LOGS" << receivingClient.log_entries.size() << endl;
                        // cout << "MESSAGE HAS BEEN NULLIFIED" << endl;
                        fullMessage = "";
                    }                  
                    
                } else {
                    
                    
                    bool forwardStatus = forwardFrame(receivingClient, frameStack, clients);
                
                    receivingClient.incoming_queue.pop();
                    Packet* tempPacket;
                    if (!receivingClient.incoming_queue.empty()){
                        tempPacket = receivingClient.incoming_queue.front().top();                        
                    }
                    if (receivingClient.incoming_queue.empty() || tempPacket->frame_number==1){
                        cout << "--------" << endl;
                        if (forwardStatus){
                            receivingClient.log_entries.emplace_back(getCurrentTimeAsString(), fullMessage, frame_number, hops_count, sender_ID, receiver_ID, true, ActivityType::MESSAGE_FORWARDED);

                        } else {
                            receivingClient.log_entries.emplace_back(getCurrentTimeAsString(), fullMessage, frame_number, hops_count, sender_ID, receiver_ID, false, ActivityType::MESSAGE_DROPPED);
                        }
                    }
                    fullMessage = "";
                }
            } else {
                cout << "Error: Invalid frame format. Application layer packet not found." << endl;
            }

            
        }
        //receivingClient.printOutgoingQueue(receivingClient);
    }
    
}


void Network::assembleAndOutputMessage(vector<Client>& clients, const Client& receiverClient, stack<Packet*>& frameStack) {
    ApplicationLayerPacket* appPacket = dynamic_cast<ApplicationLayerPacket*>(getBottomPacket(frameStack));
    PhysicalLayerPacket* physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frameStack.top());
    Client* firstSender = get_client_by_id(clients, appPacket->sender_ID);
    Client* previousSender = getClientByMacAddress(clients, physicalPacket->sender_MAC_address);
    cout << "Client " << receiverClient.client_id << " receiving frame #" << frameStack.top()->frame_number<< " from client " << previousSender->client_id<<", originating from client "<< firstSender->client_id << endl;
    int hops_count = frameStack.top()->hops_count;
    while (!frameStack.empty()) {
        Packet* packet = frameStack.top();
        frameStack.pop();
        packet->print();
        delete packet;
    }
    cout << "Number of hops so far: " << hops_count << endl;
    cout << "--------" << endl;
    //deleteStack(frameStack);
    
}

bool Network::forwardFrame(Client& forwardingClient, stack<Packet*>& frameStack, vector<Client>& clients) {
    
    ApplicationLayerPacket* appPacket = dynamic_cast<ApplicationLayerPacket*>(getBottomPacket(frameStack));
    PhysicalLayerPacket* physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frameStack.top());
    Client* nextHopClient = forwardingClient.getNextHopClient(clients, forwardingClient, *get_client_by_id(clients, appPacket->receiver_ID));
    Client* previousSender = getClientByMacAddress(clients, physicalPacket->sender_MAC_address);
    if (nextHopClient) {
        physicalPacket->receiver_MAC_address = nextHopClient->client_mac;
        physicalPacket->sender_MAC_address = forwardingClient.client_mac;
        forwardingClient.outgoing_queue.push(frameStack);
        
        if (frameStack.top()->frame_number==1){
                        cout << "Client " << forwardingClient.client_id << " receiving a message from client "
                        << previousSender->client_id << ", but intended for client " << appPacket->receiver_ID
                        << ". Forwarding..." << endl;
                    }
        cout << "Frame #" << frameStack.top()->frame_number << " MAC address change: New sender MAC " << forwardingClient.client_mac << ", new receiver MAC "<< nextHopClient->client_mac << endl;
        return true;
    } else {
        cout << "Client " << forwardingClient.client_id << " receiving frame #"<< frameStack.top()->frame_number <<" from client "
                        << previousSender->client_id << ", but intended for client " << appPacket->receiver_ID
                        << ". Forwarding..." << endl;
        cout << "Error: Unreachable destination. Packets are dropped after " << frameStack.top()->hops_count << " hops!" << endl;

        deleteStack(frameStack);
        return false;
    }
}

void Network::deleteStack(stack<Packet*>& frameStack) {
    // Iterate through the frame stack and delete each packet
    while (!frameStack.empty()) {
        Packet* packet = frameStack.top();
        frameStack.pop();
        delete packet;
    }
}

// Function to get the bottom Packet object of a stack of Packet pointers
Packet* Network::getBottomPacket(std::stack<Packet*>& inputStack) {
    if (inputStack.empty()) {
        std::cerr << "Error: Stack is empty." << std::endl;
        return nullptr;
    }

    std::stack<Packet*> tempStack;

    // Transfer elements from the input stack to the temporary stack
    while (!inputStack.empty()) {
        tempStack.push(inputStack.top());
        inputStack.pop();
    }

    // The bottom Packet object is now at the top of the temporary stack
    Packet* bottomPacket = tempStack.top();

    // Restore the elements back to the original stack
    while (!tempStack.empty()) {
        inputStack.push(tempStack.top());
        tempStack.pop();
    }

    return bottomPacket;
}

void Network::print_log(vector<Client>& clients, const string& client_id) {
    // Find the client by ID
    Client* client = get_client_by_id(clients, client_id);

    // Check if the client is found
    if (client == nullptr) {
        cerr << "Error: Client " << client_id << " not found." << endl;
        return;
    }

    // Check if the client has any log entries
    if (client->log_entries.empty()) {
        // No log entries for the client
        return;
    }

    // Print the log entries for the client
    cout << "Client " << client_id << " Logs:" << endl;

    // Iterate over the log entries of the client
    for (int i = 0; i < client->log_entries.size(); ++i) {
        const Log& logEntry = client->log_entries[i];

        // Call the generic print method
        logEntry.print(logEntry.activity_type, i + 1);
    }
}


std::string Network::getCurrentTimeAsString() {
    std::string formattedTime;

    // Use std::time and std::localtime for cross-platform compatibility
    std::time_t now = std::time(nullptr);
    struct tm* timeInfo = std::localtime(&now);
    
    if (timeInfo != nullptr) {
        char buffer[80];
        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo) > 0) {
            formattedTime = buffer;
        }
    }

    return formattedTime;
}

void Network::show_frame_info(vector<Client> &clients, const string& client_id, const string& direction, int frame_number) {
    // Display frame information
    

    Client* client = get_client_by_id(clients, client_id);
    queue<stack<Packet*>> tempQueue;
    if (direction == "out"){
        int totalFrameNumber = client->outgoing_queue.size();
        tempQueue = client->outgoing_queue;
    } else if (direction == "in"){
        int totalFrameNumber = client->incoming_queue.size();
        tempQueue = client->incoming_queue;
    } else{
        cout << "Invalid direction: " << direction << endl;
    }
    int hops_count;
    // Display the frame information
    bool frameFound = false;
    // Iterate through the frameQueue using indexes
    int current_frame_number = 0;
    stack<Packet*> frame;
    while (!tempQueue.empty()) {
        current_frame_number++;
        if (current_frame_number==frame_number){
            frameFound = true;
            frame = tempQueue.front();
            break;
        } else{
            tempQueue.pop();
            continue;
        }
    }
    if (frameFound){
        if (direction == "out"){
            cout << "Current Frame #" << frame_number << " on the " << direction << "going queue of client " << client_id << endl;
        } else if (direction == "in"){
            cout << "Current Frame #" << frame_number << " on the " << direction << "coming queue of client " << client_id << endl;
        }
        
        // Display information from each layer
        stack<Packet*> tempStack;  // Temporary stack to reverse the order
        hops_count = frame.top()->hops_count;
        while (!frame.empty()) {
            Packet* packet = frame.top();
            frame.pop();
            tempStack.push(packet);  // Store the packet in the temporary stack
        }
        
        PhysicalLayerPacket* physicalLayerPacket;
        NetworkLayerPacket* networkLayerPacket;
        TransportLayerPacket* transportLayerPacket;
        ApplicationLayerPacket* applicationLayerPacket;

        extractPackets(tempStack, physicalLayerPacket, networkLayerPacket, transportLayerPacket, applicationLayerPacket);

        cout << "Carried Message: " << "\"" << applicationLayerPacket->message_data <<"\"" << endl; 
        cout << "Layer 0 info: ";
        cout << "Sender ID: " << applicationLayerPacket->sender_ID << ", Receiver ID: " << applicationLayerPacket->receiver_ID << endl;
        cout << "Layer 1 info: ";
        cout << "Sender port number: " << transportLayerPacket->sender_port_number << ", Receiver port number: " << transportLayerPacket->receiver_port_number << endl;
        cout << "Layer 2 info: ";
        cout << "Sender IP address: " << networkLayerPacket->sender_IP_address << ", Receiver IP address: " << networkLayerPacket->receiver_IP_address << endl;
        cout << "Layer 3 info: ";
        cout << "Sender MAC address: " << physicalLayerPacket->sender_MAC_address << ", Receiver MAC address: " << physicalLayerPacket->receiver_MAC_address << endl;

        // // Print the information in the correct order
        // while (!tempStack.empty()) {
        //     Packet* packet = tempStack.top();
        //     tempStack.pop();

        //     // Display information for the selected frame
        //     cout << "Layer " << packet->layer_ID << " info: ";
        //     packet->print(); // Assuming you have a print method in the Packet class
        // }
        cout<< "Number of hops so far: " << hops_count << endl;

    } else{
        cout << "No such frame." << endl;
    }
    
}

void Network::extractPackets(std::stack<Packet*>& frameStack,
                    PhysicalLayerPacket*& physicalLayerPacket,
                    NetworkLayerPacket*& networkLayerPacket,
                    TransportLayerPacket*& transportLayerPacket,
                    ApplicationLayerPacket*& applicationLayerPacket) {
// Extract necessary information from the frame
    Packet* applicationPacket = frameStack.top();
    // Check if the dynamic type is ApplicationLayerPacket
    applicationLayerPacket = dynamic_cast<ApplicationLayerPacket*>(applicationPacket);
    frameStack.pop();
    // Extract necessary information from the frame
    Packet* transportPacket = frameStack.top();
    // Check if the dynamic type is TransportLayerPacket
    transportLayerPacket = dynamic_cast<TransportLayerPacket*>(transportPacket);
    frameStack.pop();
    // Extract necessary information from the frame
    Packet* networkPacket = frameStack.top();
    // Check if the dynamic type is NetworkLayerPacket
    networkLayerPacket = dynamic_cast<NetworkLayerPacket*>(networkPacket);
    frameStack.pop();
    // Extract necessary information from the frame
    Packet* physicalPacket = frameStack.top();
    // Check if the dynamic type is PhysicalLayerPacket
    physicalLayerPacket = dynamic_cast<PhysicalLayerPacket*>(physicalPacket);
    frameStack.pop();

    

    

    
}


void Network::show_queue_info(vector<Client>& clients, const string& client_id, const string& direction) {
    // Find the client in the vector
    Client* selectedClient = get_client_by_id(clients, client_id);

    if (selectedClient == nullptr) {
        cerr << "Error: Client not found with ID " << client_id << endl;
        return;
    }

    // Determine whether to show incoming or outgoing queue based on the direction
    const queue<stack<Packet*>>& selectedQueue = (direction == "in") ? selectedClient->incoming_queue : selectedClient->outgoing_queue;

    cout << "Client " << client_id << " " << ((direction == "in") ? "Incoming" : "Outgoing") << " Queue Status" << endl;
    
    int totalFrames = selectedQueue.size();

    if (totalFrames == 0) {
        cout << "Current total number of frames: 0" << endl;
    } else {
        cout << "Current total number of frames: " << totalFrames << endl;
    }
}


// Function to increase hop_count of each packet in the stack by 1
void Network::increaseHopCount(std::stack<Packet*>& packetStack) {
    // Create a temporary stack to hold the modified packets
    std::stack<Packet*> tempStack;

    // Iterate through the original stack
    while (!packetStack.empty()) {
        // Get the top packet
        Packet* currentPacket = packetStack.top();
        
        // Increase hop_count by 1
        currentPacket->hops_count++;

        // Move the packet to the temporary stack
        tempStack.push(currentPacket);

        // Pop the packet from the original stack
        packetStack.pop();
    }

    // Restore the modified packets back to the original stack
    while (!tempStack.empty()) {
        packetStack.push(tempStack.top());
        tempStack.pop();
    }
}

Client* Network::getClientByMacAddress(vector<Client>& clients, const string& macAddress) {
    for (Client& client : clients) {
        if (client.client_mac == macAddress) {
            return &client; // Return a pointer to the client if found
        }
    }
    return nullptr; // Return nullptr if the client is not found
}


vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return clients;
    }

    int number_of_clients;
    file >> number_of_clients;

    for (int i = 0; i < number_of_clients; ++i) {
        string ID, IP_Address, MAC_Address;
        file >> ID >> IP_Address >> MAC_Address;
        clients.emplace_back(ID, IP_Address, MAC_Address);
    }

    file.close();
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    ifstream file(filename);
    std::vector<Client> clientsCopy(clients);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    int count = 0;
    string line;
    unordered_map<string, string> routingTableData; // temporary storage for a client's routing table

    while (getline(file, line)) {
        // Check for the delimiter "-" to indicate the end of a client's routing table
        if (line == "-") {
            // Find the corresponding client and update its routing table
            if (!clients.empty()) {
                clients[count].routing_table = routingTableData;
            }
            count++;
            // Clear the temporary storage for the next client
            routingTableData.clear();

        } else {
            // Parse the line and add the routing table entry to the unordered map
            istringstream iss(line);
            string destination, nextHop;
            iss >> destination >> nextHop;
            routingTableData[destination] = nextHop;
        }
    }

    // Update the routing table for the last client (if any)
    if (!clients.empty()) {
        clients.back().routing_table = routingTableData;
    }

    file.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return commands;  // Return an empty vector if the file couldn't be opened
    }

    // Skip the first line (number of commands)
    string firstLine;
    getline(file, firstLine);

    int numCommands;
    if (!(istringstream(firstLine) >> numCommands)) {
        cerr << "Error parsing the number of commands." << endl;
        file.close();
        return commands;
    }

    string command;
    for (int i = 0; i < numCommands && getline(file, command); ++i) {
        // Add the command to the commands vector
        commands.push_back(command);
    }

    file.close();

    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}
