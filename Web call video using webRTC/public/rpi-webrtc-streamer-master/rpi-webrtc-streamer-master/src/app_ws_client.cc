/*
Copyright (c) 2018, rpi-webrtc-streamer Lyu,KeunChang

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "websocket_server.h"
#include "app_ws_client.h"
#include "utils.h"

static const char kKeyCmd[] = "cmd";
static const char kValueCmdRegister[] = "register";
static const char kValueCmdSend[] = "send";

static const char kKeyRegisterRoomId[] = "roomid";
static const char kKeyRegisterClientId[] = "clientid";
static const char kKeyCmdSendMessage[] = "msg";
static const char kKeyCmdSendType[] = "type";
static const char kValueCmdSendTypeBye[] = "bye";

//////////////////////////////////////////////////////////////////////////////////////////
//
// App Websocket only Channel
//
//////////////////////////////////////////////////////////////////////////////////////////
AppWsClient::AppWsClient()
    : websocket_message_(nullptr),num_chunked_frames_(0)  {
}

void AppWsClient::RegisterWebSocketMessage(WebSocketMessage *server_instance) {
    websocket_message_ = server_instance;
}

void AppWsClient::OnConnect(int sockid) {
    RTC_LOG(INFO) << "New WebSocket connnection id : " << sockid;
    // reset the chunked_frames list container
    chunked_frames_.clear();
    num_chunked_frames_=0;
}


bool AppWsClient::OnMessage(int sockid, const std::string& message) {
    RTC_LOG(INFO) << __FUNCTION__ << "(" << sockid << ")";

    Json::Reader json_reader;
    Json::Value json_value;
    std::string cmd;
    bool parsing_successful;

    // There is an issue where Chrome & Firefox WebSocket sends json messages
    // in multiple chunks, so this is the part to solve.
    // If JSON parsing fails, it will be kept in chunked_frrames up to 5 times and
    // try to parse the collected chunked_frames again until the next message succeeds.
    if((parsing_successful = json_reader.parse(message, json_value)) == true){
        rtc::GetStringFromJsonObject(json_value, kKeyCmd, &cmd);
    };

    if ( parsing_successful && !cmd.empty() ) {
        // parsing success and found the cmd keyword...
        RTC_LOG(INFO) << "JSON Parsing Success: " << message;
    }
    else {
        static const int kMaxChunkedFrames=5;

        // json_value.clear();
        chunked_frames_.append(message);
        RTC_LOG(INFO) << "Chunked Frame (" << num_chunked_frames_ << "), Message : "
            << chunked_frames_;
        if (!json_reader.parse(chunked_frames_, json_value)) {
            // parsing failed
            if( num_chunked_frames_++ > kMaxChunkedFrames )  {
                RTC_LOG(INFO) << "Failed to parse, Dropping Chunked frames: "
                    << chunked_frames_;
                num_chunked_frames_ = 0;
                chunked_frames_.clear();
            }
            return true;
        }

        rtc::GetStringFromJsonObject(json_value, kKeyCmd, &cmd);
        if( cmd.empty() ) {
            return true;
        }
        // parsing success and cmd keyword found.
        RTC_LOG(INFO) << "Chunked frames successful: " << chunked_frames_;
        num_chunked_frames_ = 0;
        chunked_frames_.clear();
        // finally successful parsing
    }

    rtc::GetStringFromJsonObject(json_value, kKeyCmd, &cmd);
    if( !cmd.empty() ) {    // found cmd id
        // command register
        if(cmd.compare(kValueCmdRegister)== 0) {
            int client_id, room_id;
            if( !rtc::GetIntFromJsonObject(json_value, kKeyRegisterRoomId, &room_id) ||
                !rtc::GetIntFromJsonObject(json_value, kKeyRegisterClientId, &client_id)) {
                RTC_LOG(LS_ERROR) << "Not found clientid/roomid :" << message;
                return true;
            }
            RTC_LOG(INFO) << "Room ID: " << room_id << ", Client ID: " << client_id;
            if( app_client_.Connected( sockid, room_id, client_id ) == false ) {
                RTC_LOG(LS_ERROR) << "Failed to set room_id/client_id :" << message;
                // Drop current connection
                return false;
            };

            if ( IsStreamSessionActive() == false ) {
                if( ActivateStreamSession(client_id,utils::IntToString(client_id))
                        == true ) {
                    RTC_LOG(INFO) << "New WebSocket Name: " << client_id;
                };
                return true;
            }
            // TODO(kclyu) need to send response of room busy error message
            RTC_LOG(INFO) << "Streamer Session already Active. Try to drop connection: "
                << client_id;
            return true;
        }
        // command send
        else if(cmd.compare(kValueCmdSend) == 0) {
            std::string msg;
            rtc::GetStringFromJsonObject(json_value, kKeyCmdSendMessage, &msg);
            if( !msg.empty() ) {
                Json::Value json_msg_value;
                std::string json_msg_type;

                // trying to parse msg value to check whether it is json type msg
                if (!json_reader.parse(msg, json_msg_value)) {
                    RTC_LOG(WARNING) << "Failed to parse send message string. "
                        << msg;
                    return false;
                }

                rtc::GetStringFromJsonObject(json_msg_value, kKeyCmdSendType,
                        &json_msg_type);
                // checking send command is type:bye
                if( !json_msg_type.empty() &&
                    json_msg_type.compare(kValueCmdSendTypeBye) == 0) {
                    // command 'send' message is type: bye
                    // reset the app_clientinfo and deactivate the streaming
                    if( app_client_.IsConnected( sockid ) == true ) {
                        app_client_.Reset();
                        if ( IsStreamSessionActive() == true ) {
                            DeactivateStreamSession();
                        };
                    };
                    return true;
                };

                MessageFromPeer(msg);
                return true;
            }
            RTC_LOG(LS_ERROR) << "Failed to pass received message :" << message;
        }
        return true;
    };
    RTC_LOG(LS_ERROR) << "Received unknown protocol message. " << message;
    return true;
}


void AppWsClient::OnDisconnect(int sockid) {
    RTC_LOG(INFO) << "WebSocket connnection id : " << sockid << " closed";
    app_client_.DisconnectWait(sockid);
    app_client_.Reset();
    if ( IsStreamSessionActive() == true ) {
        DeactivateStreamSession();
    };
}

void AppWsClient::OnError(int sockid, const std::string& message) {
}

bool AppWsClient::SendMessageToPeer(const int peer_id, const std::string &message) {
    RTC_DCHECK(websocket_message_ != nullptr ) << "WebSocket Server instance is nullptr";
    int websocket_id;

    RTC_LOG(INFO) << __FUNCTION__;
    if( app_client_.GetWebsocketId(peer_id, websocket_id ) == true ) {
        Json::StyledWriter json_writer;
        Json::Value json_message;

        json_message[kKeyCmd] = kValueCmdSend;
        json_message[kKeyCmdSendMessage] = message;
        websocket_message_->SendMessage(websocket_id, json_writer.write(json_message));
        return true;
    }
    return false;
}

AppWsClient::~AppWsClient() {
}

