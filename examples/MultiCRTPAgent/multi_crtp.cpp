// Copyright 2021-present Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <uxr/agent/transport/custom/CustomAgent.hpp>
#include <uxr/agent/transport/endpoint/IPv4EndPoint.hpp>
#include <crazyflieLinkCpp/Connection.h>

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <map> 

#define CRTP_MAX_DATA_SIZE 30
#define UROS_PORT 9
#define CONSOLE_PORT 0
#define DEFAULT_CHANNEL 80
#define BUFFER_SIZE 10000

//Enable or disable debugging
//#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl;
#else
#define DEBUG_PRINT(x) do{}while(0)
#endif
#define CONSOLE_PRINT(x) DEBUG_PRINT(x)


using namespace bitcraze::crazyflieLinkCpp;

static size_t crtp_index_primary = 0;
static uint8_t * crtp_buffer; 


int main(int argc, char** argv)
{
    eprosima::uxr::Middleware::Kind mw_kind(eprosima::uxr::Middleware::Kind::FASTDDS);

    /*
    if (argc < 2){
        std::cout << "Please specify uri! Aborted." << std::endl;
        exit(1);
    } else if (argc > 2){
        std::cout << "Too many arguments! Only one uri supported (for now) Aborted." << std::endl;
        exit(1);
    }
    */

    //std::string uri = argv[1];
    
    //Incrivelmente trolha refactor later
    std::vector<std::string> uris({"radio://0/80/2M/E7E7E7E7E3", "radio://0/80/2M/E7E7E7E7E9"});
    Connection con1("radio://0/80/2M/E7E7E7E7E3");
    Connection con2("radio://0/80/2M/E7E7E7E7E9");

    std::ofstream debug_file;
    /**
     * @brief Agent's initialization behaviour description.
     */
    eprosima::uxr::CustomAgent::InitFunction init_function = [&]() -> bool
    {
        debug_file.open("debug.txt");
        crtp_buffer = static_cast<uint8_t *>(malloc(BUFFER_SIZE));
        return true;
    };

    /**
     * @brief Agent's destruction actions.
     */
    eprosima::uxr::CustomAgent::FiniFunction fini_function = [&]() -> bool
    {
        debug_file.close();
        return true;
    };

    /**
     * @brief Agent's incoming data functionality.
     */
    eprosima::uxr::CustomAgent::RecvMsgFunction recv_msg_function = [&](
            eprosima::uxr::CustomEndPoint* source_endpoint,
            uint8_t* buffer,
            size_t buffer_length,
            int timeout,
            eprosima::uxr::TransportRc& transport_rc) -> ssize_t
    {
        //std::cout << "in receive function " << std::endl;
        size_t bytes_received = 0;
        size_t * crtp_index = &crtp_index_primary;
        static uint32_t call_count = 0;
        uint8_t con_idx = call_count % uris.size();


        Packet p; 
        //std::cout << "Connecting to: " + uris[con_idx] << std::endl;
        switch(con_idx){
            case 0:
                p = con1.recv(timeout);
                source_endpoint->set_member_value<std::string>("uri", uris[con_idx]);
                source_endpoint->set_member_value<uint8_t>("id", 0);
                break;
            case 1:
                p = con2.recv(timeout);
                source_endpoint->set_member_value<std::string>("uri", uris[con_idx]);
                source_endpoint->set_member_value<uint8_t>("id", 1);
                break;
            default:
                std::cout << "Invalid Connection" << std::endl;
        }

        if (p.valid() && p.port() == UROS_PORT){ 
            DEBUG_PRINT("received uros packet " << (int )p.port() << "index: " << *crtp_index);
            std::memcpy(buffer, p.payload(), p.payloadSize());
            bytes_received = p.payloadSize();
            source_endpoint->set_member_value<std::string>("uri", uris[con_idx]);
            transport_rc =  (0 != bytes_received)
                ? eprosima::uxr::TransportRc::ok
                : eprosima::uxr::TransportRc::server_error;
        } else if (p.port() == CONSOLE_PORT){
            std::cout << "[Console]" << p.payload() << std::endl;
            bytes_received = 0;
            transport_rc = eprosima::uxr::TransportRc::timeout_error;
        } else {
            bytes_received = 0;
            transport_rc = eprosima::uxr::TransportRc::timeout_error;
        }        


        call_count++;
        return bytes_received;
    };

    /**
     * @brief Agent's outcoming data flow definition.
     */
    eprosima::uxr::CustomAgent::SendMsgFunction send_msg_function = [&](
        const eprosima::uxr::CustomEndPoint* destination_endpoint,
        uint8_t* buffer,
        size_t message_length,
        eprosima::uxr::TransportRc& transport_rc) -> ssize_t
    {
        size_t bytes_sent = 0;
        size_t len = message_length;
        size_t to_write;
        uint8_t id = destination_endpoint->get_member<uint8_t>("id");
        //std::cout << "wrinting" << len << std::endl;
        while (len > 0){
            //std::cout << "In the loop" << len << std::endl;
            to_write = (len <= CRTP_MAX_DATA_SIZE) ? len : CRTP_MAX_DATA_SIZE;
            Packet p;
            p.setPort(UROS_PORT);
            p.setPayloadSize(to_write);
            std::memcpy(p.payload(), &buffer[bytes_sent], to_write);
            switch(id){
                case 0:
                    con1.send(p);
                    break;
                case 1:
                    con2.send(p);
                    break;
                default:
                    std::cout << "Invalid id" << std::endl;
                    break;
            }
            transport_rc == eprosima::uxr::TransportRc::ok; //ToDo: Actually check if ok
            len -= to_write;
            bytes_sent += to_write;
            //std::cout << "len" << message_length << "\tto_write: " << to_write << "\tbytes sent: " << bytes_sent << std::endl;
        } 
        return bytes_sent;
    };  

    /**
     * Run the main application.
     */
    try
    {
        /**
         * EndPoint definition for this transport. We define an address and a port.
         */
        eprosima::uxr::CustomEndPoint custom_endpoint;
        custom_endpoint.add_member<std::string>("uri");
        custom_endpoint.add_member<uint8_t>("id");

        /**
         * Create a custom agent instance.
         */
        eprosima::uxr::CustomAgent custom_agent(
            "CRTP",
            &custom_endpoint,
            mw_kind,
            true,
            init_function,
            fini_function,
            send_msg_function,
            recv_msg_function);

        /**
         * Set verbosity level
         */
        custom_agent.set_verbose_level(6);

        /**
         * Run agent and wait until receiving an stop signal.
         */
        custom_agent.start();

        int n_signal = 0;
        sigset_t signals;
        sigwait(&signals, &n_signal);

        /**
         * Stop agent, and exit.
         */
        custom_agent.stop();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
