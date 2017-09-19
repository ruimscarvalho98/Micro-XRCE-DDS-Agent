// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/*!
* @file MessageHeader.cpp
* This source file contains the definition of the described types in the IDL file.
*
* This file was generated by the tool gen.
*/

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace { char dummy; }
#endif

#include "agent/MessageHeader.h"

#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

namespace eprosima {
namespace micrortps {

MessageHeader::MessageHeader()
{
    client_key_ = 0;
    session_id_ = 0;
    stream_id_ = 0;
    sequence_nr_ = 0;
}

MessageHeader::~MessageHeader()
{
}

MessageHeader::MessageHeader(const MessageHeader &x)
{
    client_key_ = x.client_key_;
    session_id_ = x.session_id_;
    stream_id_ = x.stream_id_;
    sequence_nr_ = x.sequence_nr_;
}

MessageHeader::MessageHeader(MessageHeader &&x)
{
    client_key_ = x.client_key_;
    session_id_ = x.session_id_;
    stream_id_ = x.stream_id_;
    sequence_nr_ = x.sequence_nr_;
}

MessageHeader& MessageHeader::operator=(const MessageHeader &x)
{
    client_key_ = x.client_key_;
    session_id_ = x.session_id_;
    stream_id_ = x.stream_id_;
    sequence_nr_ = x.sequence_nr_;

    return *this;
}

MessageHeader& MessageHeader::operator=(MessageHeader &&x)
{
    client_key_ = x.client_key_;
    session_id_ = x.session_id_;
    stream_id_ = x.stream_id_;
    sequence_nr_ = x.sequence_nr_;

    return *this;
}

size_t MessageHeader::getMaxCdrSerializedSize(size_t current_alignment)
{
    size_t initial_alignment = current_alignment;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);

    current_alignment += 1 + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);

    current_alignment += 1 + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);

    current_alignment += 2 + eprosima::fastcdr::Cdr::alignment(current_alignment, 2);


    return current_alignment - initial_alignment;
}

size_t MessageHeader::getCdrSerializedSize(const MessageHeader& data, size_t current_alignment)
{
    size_t initial_alignment = current_alignment;

    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);

    current_alignment += 1 + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);

    current_alignment += 1 + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);

    current_alignment += 2 + eprosima::fastcdr::Cdr::alignment(current_alignment, 2);


    return current_alignment - initial_alignment;
}

void MessageHeader::serialize(eprosima::fastcdr::Cdr &scdr) const
{
    scdr << client_key_;
    scdr << session_id_;
    scdr << stream_id_;
    scdr.serialize(sequence_nr_, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
}

void MessageHeader::deserialize(eprosima::fastcdr::Cdr &dcdr)
{
    dcdr >> client_key_;
    dcdr >> session_id_;
    dcdr >> stream_id_;
    dcdr.deserialize(sequence_nr_, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
}

} /* namespace micrortps */
} /* namespace eprosima */
