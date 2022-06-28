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
#include "chat.h"

#include <iostream>
#include <string>
using namespace sw::redis;

Chat::Chat(std::string chat) {
    printf("Listener\n");
    user = std::string("listener");
    Init(chat);
}

Chat::Chat(std::string chat, std::string user) : user(user) {
    printf("Writer\n");
    Init(chat);
}

void Chat::Init(std::string channelname) {
    strmmsgid = "$";
    // Initialize redis vars
    chatname = channelname + ".chat";
    cmdname = channelname + ".cmd";
    try {
        // Open redis Connection
        ConnectionOptions opts;
        opts.host = redis_server;
        opts.port = redis_port;
        opts.socket_timeout = std::chrono::milliseconds(conection_timeout_ms);
        connector = new Redis(opts);

        // Create redis subscriber
        sub = new sw::redis::Subscriber(connector->subscriber());

        // subscribe to cmd channel
        sub->subscribe(cmdname);

        // assign lambda function to channel
        sub->on_message([](std::string channel, std::string msg) {
            (void)channel;
            printf("Event: %s\n", msg.c_str());
        });

        // Register new connection
        SendCmd(ChatCmd::connect);
    } catch (const Error &e) {
        std::cerr << e.what();
    }
}

Chat::~Chat() {
    SendCmd(ChatCmd::disconnect);
    delete sub;
    delete connector;
}

void Chat::SendMsg(std::string msg) {
    Attrs attrs = {{"user", user}, {"msg", msg}};
    connector->xadd(chatname, "*", attrs.begin(), attrs.end());
}

void Chat::SendCmd(ChatCmd cmd) {
    std::string cmddata;
    switch (cmd) {
        case ChatCmd::connect:
            cmddata = user + " joined";
            break;

        case ChatCmd::disconnect:
            cmddata = user + " left";
            break;
    }
    connector->publish(cmdname, cmddata);
}

void Chat::RecvCmd() {
    try {
        sub->consume();
    } catch (const TimeoutError &e) {
        // Try again.
        return;
    } catch (const Error &e) {
        std::cerr << e.what();
    }
}

bool Chat::Listener() {
    if (user == "listener") {
        return true;
    }
    return false;
}

void Chat::PrintMessage() {
    std::unordered_map<std::string,  std::vector<Item> > result;
    try {
        connector->xread(chatname,
                         strmmsgid,
                         std::chrono::seconds(0),
                         1,
                         std::inserter(result, result.end()));
    } catch (const TimeoutError &e) {
        // Try again.
        return;
    }
    if (!result.empty()) {
        strmmsgid = result[chatname][0].first;
        auto data = result[chatname][0].second;
        if (data) {
            // has new data
            if (data.value()[1].first == "msg") {
                std::string user = data.value()[0].second;
                std::string msg = data.value()[1].second;
                printf("%s: %s\n", user.c_str(), msg.c_str());
            }
        }
        result.clear();
    }
}
