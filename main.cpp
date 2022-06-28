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

#include <iostream>
#include <optional>
#include <string>
#include <future>
#include <thread>
#include <chrono>

#include "chat.h"

/**
 * @brief Get the user msg asynchronously
 * 
 * @return std::string user message
 */
static std::string GetMsg()
{    
    std::string msg;
    std::getline(std::cin, msg);
    return msg;
}

/**
 * @brief writer thread
 * 
 * @param chat Chat object
 */
static void WriterFunc(Chat& chat) {
    std::string usrinput;
    printf("Send empty line to exit\n");

    std::future<std::string> future = std::async(GetMsg);
    while (true) {
        if (future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
            usrinput = future.get();
            if (!usrinput.empty()) {
                chat.SendMsg(usrinput);
                future = std::async(GetMsg);
            } else {
                break;
            }
        }
    }
}

/**
 * @brief Listener thread
 * 
 * @param chat Chat object
 */
static void ListenerFunc(Chat& chat) {
    while (true) {
        chat.PrintMessage();
        chat.RecvCmd();
    }
}

int main(int argc, char* argv[]) {
    Chat* chatmgr;
    std::thread* chat_thread;
    if (argc > 2) {
        // writer
        chatmgr = new Chat(argv[1], argv[2]);
    } else if (argc > 1) {
        // reader
        chatmgr = new Chat(argv[1]);
    } else {
        // Error
        printf("Error\n Use: chat chatname| chat chatname username\n");
        return 1;
    }
    
    if (chatmgr->Listener()) {
        // Create listener thread
        chat_thread = new std::thread(ListenerFunc, std::ref(*chatmgr));
    } else {
        // Create writer thread
        chat_thread = new std::thread(WriterFunc, std::ref(*chatmgr));
    }
    chat_thread->join();

    delete chat_thread;
    delete chatmgr;
    return 0;
}
