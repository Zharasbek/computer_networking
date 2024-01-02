# HUBBMNET - Basic Network Communication Implementation

This repository contains the implementation of a simplified computer networking protocol suite, inspired by the Internet protocol suite. The primary goal is to establish network communication between peers within a computer network. The network protocol stack consists of four layers: Application, Transport, Network, and Data Link/Physical.

## Mission Objectives

The primary objectives of this project are as follows:

- Implement a basic version of network communication between peers.
- Develop a simplified computer networking protocol suite with four layers.
- Enable communication among clients based on the defined protocols.
- Provide simple routing of data packets using clients' routing tables.
- Implement logging of all network activities and printing log reports.

## Basic Layer Services

The implemented network provides the following basic layer services:

- Application Layer: Responsible for dividing messages into chunks and identifying sender and receiver IDs.
- Transport Layer: Appends port numbers to frames based on the application and type of socket.
- Network Layer: Appends sender and receiver IP addresses to frames for routing.
- Data Link/Physical Layer: Appends physical MAC addresses of clients for the next hop in the network.

## Summary of Network Services

The key network services implemented include:

- Communication among clients based on protocols.
- Routing of data packets using clients' routing tables.
- Logging of all network activity.
- Printing log reports as necessary.

## Network Structure and Protocols

### Network Packets

The network packets are defined for each layer as follows:

- **Application Layer Packet:**
  - Layer ID
  - Sender ID
  - Receiver ID
  - Message Chunk

- **Transport Layer Packet:**
  - Layer ID
  - Sender port number
  - Receiver port number

- **Network Layer Packet:**
  - Layer ID
  - Sender IP address
  - Receiver IP address

- **Physical Layer Packet:**
  - Layer ID
  - Sender MAC address
  - Receiver MAC address

### Frames as Stacks of Network Packets

Frames are implemented as stacks of network packets in a Last In First Out (LIFO) structure. The encapsulation process involves preparing the data for transmission across the physical network medium.

### Outgoing and Incoming Queues

Each client has outgoing and incoming queues for managing messages. The queues are implemented as First In First Out (FIFO) structures.

### Routing

Routing is determined based on the client's routing table, specifying the next hop for frames en route to their final destination.

### Logs

All network activities are logged, including details such as timestamp, number of frames, number of hops, sender and receiver IDs, activity type, and success status.

## Input Files and Parameters

The program takes three input files and three command-line arguments:

1. **Client Info File:** Contains details about the network clients.
2. **Routing Tables File:** Contains information about the network topology and routing tables.
3. **Commands File:** Specifies network commands for simulation.

The command-line arguments include the maximum supported message size, outgoing port, and incoming port.


## Getting Started

These instructions will guide you through cloning the project, installing any necessary dependencies, and running the network communication simulation.

### Prerequisites

Ensure that you have the following prerequisites installed on your system:

- [Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- [C++](https://visualstudio.microsoft.com/vs/features/cplusplus/)

### Clone the Repository

Open your terminal or command prompt and run the following command to clone the repository:

```bash
git clone https://github.com/Zharasbek/computer_networking.git
```

### Program Execution

To execute the program, use the following command:

```bash
./HUBBMNET clients.dat routing.dat commands.dat max_msg_size outgoing_port incoming_port
```

## Contributing

Thank you for considering contributing to this project! Your contributions are highly valued.

### How to Contribute

1. Fork the repository to your own GitHub account.
2. Clone the project to your local machine.
  ```bash
   git clone https://github.com/Zharasbek/computer_networking.git
  ```
3. Create a new branch:
  ```bash
   git checkout -b feature/your-feature-name
  ```
4. Make your changes and commit them:
  ```bash
    git add .
    git commit -m "Add your commit message here"
  ```
5. Push to the branch:
  ```bash
    git push origin feature/your-feature-name
  ```
6. Open a Pull Request (PR) to the main branch with a clear title and description.
7. Your PR will be reviewed, and once approved, it will be merged.

## Issues
If you find any issues or have suggestions for improvements, please open an issue on the GitHub repository.
Provide a clear and detailed description of the problem or enhancement you're proposing.

## Feature Requests
Feel free to propose new features or improvements. Open an issue to discuss your ideas and gather feedback from the community.
