#include "Log.h"

Log::Log(const string &_timestamp, const string &_message, int _number_of_frames, int _number_of_hops, const string &_sender_id,
         const string &_receiver_id, bool _success, ActivityType _type) {
    timestamp = _timestamp;
    message_content = _message;
    number_of_frames = _number_of_frames;
    number_of_hops = _number_of_hops;
    sender_id = _sender_id;
    receiver_id = _receiver_id;
    success_status = _success;
    activity_type = _type;
}

void Log::print(ActivityType activity_type, int entry_number) const {
    switch (activity_type) {
        case ActivityType::MESSAGE_RECEIVED:
            printMessageReceived(entry_number);
            break;
        case ActivityType::MESSAGE_FORWARDED:
            printMessageForwarded(entry_number);
            break;
        case ActivityType::MESSAGE_SENT:
            printMessageSent(entry_number);
            break;
        case ActivityType::MESSAGE_DROPPED:
            printMessageDropped(entry_number);
            break;
        default:
            std::cerr << "Unknown activity type." << std::endl;
            break;
    }
}

void Log::printMessageSent(int entry_number) const {
    std::cout << "--------------" << std::endl;
    std::cout << "Log Entry #" << entry_number << ":" << std::endl;
    std::cout << "Activity: Message Sent" << std::endl;
    printCommonInfo();
    std::cout << "Message: \"" << message_content << "\"" << std::endl;
}

void Log::printMessageForwarded(int entry_number) const {
    std::cout << "--------------" << std::endl;
    std::cout << "Log Entry #" << entry_number << ":" << std::endl;
    std::cout << "Activity: Message Forwarded" << std::endl;
    printCommonInfo();
}

void Log::printMessageDropped(int entry_number) const {
    std::cout << "--------------" << std::endl;
    std::cout << "Log Entry #" << entry_number << ":" << std::endl;
    std::cout << "Activity: Message Dropped" << std::endl;
    printCommonInfo();
}

void Log::printMessageReceived(int entry_number) const {
    std::cout << "--------------" << std::endl;
    std::cout << "Log Entry #" << entry_number << ":" << std::endl;
    std::cout << "Activity: Message Received" << std::endl;
    printCommonInfo();
    std::cout << "Message: \"" << message_content << "\"" << std::endl;
}

void Log::printCommonInfo() const {
    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "Number of frames: " << number_of_frames << std::endl;
    std::cout << "Number of hops: " << number_of_hops << std::endl;
    std::cout << "Sender ID: " << sender_id << std::endl;
    std::cout << "Receiver ID: " << receiver_id << std::endl;
    std::cout << "Success: " << (success_status ? "Yes" : "No") << std::endl;
}


Log::~Log() {
    // TODO: Free any dynamically allocated memory if necessary.
}
