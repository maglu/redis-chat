
/**
 * @file chat.h
 * @brief Create a Chat object that will connect on redis server.
 *  Same class is used for listener and writer, each one needs its own object.
 *  Redis server is hardcoded as localhost.
 * @version 0.1
 * @date 2022-06-28
 *  
 */
/*
Copyright 2022 maglu

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <sw/redis++/redis++.h>

#include <string>
#include <unordered_map>
#include <optional>

/**
 * @brief Chat Commands
 * 
 */
enum class ChatCmd {
    connect,
    disconnect,    
};

using Attrs = std::vector<std::pair<std::string, std::string>>;
using Item = std::pair<std::string, sw::redis::Optional<Attrs>>;

/**
 * @brief Class chat 
 * 
 */
class Chat {
   public:
    /**
     * @brief Construct a new Chat object for listener
     * 
     * @param chat name of the chat
     */
    Chat(std::string chat);
   
    /**
     * @brief Construct a new Chat object for writer
     * 
     * @param chat name of the chat
     * @param user user of the chat
     * 
     */
    Chat(std::string chat, std::string user);

    ~Chat();

    /**
     * @brief Send new message to the chat
     * 
     * @param msg msg string
     */
    void SendMsg(std::string msg);

    /**
     * @brief Publish new command on cmd channel
     * 
     * @param cmd command type
     */
    void SendCmd(ChatCmd cmd);

    /**
     * @brief Check for new command
     * 
     */
    void RecvCmd();

    /**
     * @brief Check if object is a listener or a writer
     * 
     * @return true listener
     * @return false writer
     */
    bool Listener();

    /**
     * @brief Print new message on stream 
     * 
     */
    void PrintMessage();

   private:
    // Redis Connection configuration
    static constexpr const char* redis_server = "127.0.0.1";
    static constexpr int redis_port = 6379;
    static constexpr int conection_timeout_ms = 10;

    sw::redis::Redis* connector;    // Redis connector
    sw::redis::Subscriber* sub;     // subscriber
    std::string chatname;           // name of chat stream on redis
    std::string cmdname;            // name of command channel on redis
    std::string user;               // user name
    std::string strmmsgid;          // last message id on redis
    
    /**
     * @brief Common initialization
     * 
     * @param channelname base name of the channel
     */
    void Init(std::string channelname);
};