// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2020 - 2021 by Apex.AI Inc. All rights reserved.
// Copyright (c) 2023 by Mathias Kraus <elboberido@m-hias.de>. All rights reserved.
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
//
// SPDX-License-Identifier: Apache-2.0
#ifndef IOX_HOOFS_POSIX_WRAPPER_UNIX_DOMAIN_SOCKET_HPP
#define IOX_HOOFS_POSIX_WRAPPER_UNIX_DOMAIN_SOCKET_HPP

#include "iceoryx_hoofs/internal/posix_wrapper/ipc_channel.hpp"
#include "iceoryx_platform/fcntl.hpp"
#include "iceoryx_platform/platform_settings.hpp"
#include "iceoryx_platform/socket.hpp"
#include "iceoryx_platform/stat.hpp"
#include "iceoryx_platform/un.hpp"
#include "iox/builder.hpp"
#include "iox/duration.hpp"
#include "iox/filesystem.hpp"
#include "iox/optional.hpp"
#include <cassert>

namespace iox
{
namespace posix
{
class UnixDomainSocketBuilder;

/// @brief Wrapper class for unix domain socket
class UnixDomainSocket
{
  public:
    struct NoPathPrefix_t
    {
    };
    static constexpr NoPathPrefix_t NoPathPrefix{};
    static constexpr uint64_t NULL_TERMINATOR_SIZE = 1U;
    static constexpr uint64_t MAX_MESSAGE_SIZE = platform::IOX_UDS_SOCKET_MAX_MESSAGE_SIZE - NULL_TERMINATOR_SIZE;
    static constexpr uint64_t MAX_NUMBER_OF_MESSAGES = 10;
    /// @brief The name length is limited by the size of the sockaddr_un::sun_path buffer and the IOX_SOCKET_PATH_PREFIX
    static constexpr size_t LONGEST_VALID_NAME = sizeof(sockaddr_un::sun_path) - 1;

    using Builder_t = UnixDomainSocketBuilder;

    using UdsName_t = string<LONGEST_VALID_NAME>;
    using Message_t = string<platform::IOX_UDS_SOCKET_MAX_MESSAGE_SIZE>;

    UnixDomainSocket() noexcept = delete;
    UnixDomainSocket(const UnixDomainSocket& other) = delete;
    UnixDomainSocket(UnixDomainSocket&& other) noexcept;
    UnixDomainSocket& operator=(const UnixDomainSocket& other) = delete;
    UnixDomainSocket& operator=(UnixDomainSocket&& other) noexcept;

    ~UnixDomainSocket() noexcept;

    /// @brief unlink the provided unix domain socket
    /// @param name of the unix domain socket to unlink
    /// @return true if the unix domain socket could be unlinked, false otherwise, IpcChannelError if error occured
    static expected<bool, IpcChannelError> unlinkIfExists(const UdsName_t& name) noexcept;

    /// @brief unlink the provided unix domain socket
    /// @param NoPathPrefix signalling that this method does not add a path prefix
    /// @param name of the unix domain socket to unlink
    /// @return true if the unix domain socket could be unlinked, false otherwise, IpcChannelError if error occured
    static expected<bool, IpcChannelError> unlinkIfExists(const NoPathPrefix_t, const UdsName_t& name) noexcept;

    /// @brief send a message using std::string.
    /// @param msg to send
    /// @return IpcChannelError if error occured
    expected<void, IpcChannelError> send(const std::string& msg) const noexcept;

    /// @brief try to send a message for a given timeout duration using std::string
    /// @param msg to send
    /// @param timeout for the send operation
    /// @return IpcChannelError if error occured
    expected<void, IpcChannelError> timedSend(const std::string& msg, const units::Duration& timeout) const noexcept;

    /// @brief receive message using std::string.
    /// @return received message. In case of an error, IpcChannelError is returned and msg is empty.
    expected<std::string, IpcChannelError> receive() const noexcept;

    /// @brief try to receive message for a given timeout duration using std::string.
    /// @param timeout for the receive operation
    /// @return received message. In case of an error, IpcChannelError is returned and msg is empty.
    expected<std::string, IpcChannelError> timedReceive(const units::Duration& timeout) const noexcept;

    /// @brief send a message using a dimension template containter.
    /// @param msg to send
    /// @return IpcChannelError if error occured
    template <template <uint64_t> class C, uint64_t N>
    expected<void, IpcChannelError> send(const C<N>& buf) const noexcept;

    /// @brief try to send a message for a given timeout duration using a dimension template containter.
    /// @param msg to send
    /// @param timout for the send operation
    /// @return IpcChannelError if error occured
    template <template <uint64_t> class C, uint64_t N>
    expected<void, IpcChannelError> timedSend(const C<N>& buf, const units::Duration& timeout) const noexcept;

    /// @brief receive message using a dimension template containter.
    /// @param buf message received
    /// @return  IpcChannelError if error occured
    template <template <uint64_t> class C, uint64_t N>
    expected<void, IpcChannelError> receive(C<N>& buf) const noexcept;

    /// @brief try to receive message for a given timeout duration using a dimension template containter.
    /// @param buf message received
    /// @param timeout for the receive operation
    /// @return  IpcChannelError if error occured
    template <template <uint64_t> class C, uint64_t N>
    expected<void, IpcChannelError> timedReceive(C<N>& buf, const units::Duration& timeout) const noexcept;
  private:
    friend class UnixDomainSocketBuilderNoPathPrefix;

    UnixDomainSocket(const UdsName_t& name,
                     const IpcChannelSide channelSide,
                     const int32_t sockfd,
                     const sockaddr_un sockAddr,
                     const uint64_t maxMsgSize) noexcept;

    expected<void, IpcChannelError> destroy() noexcept;

    expected<void, IpcChannelError> initalizeSocket() noexcept;

    IpcChannelError errnoToEnum(const int32_t errnum) const noexcept;
    static IpcChannelError errnoToEnum(const UdsName_t& name, const int32_t errnum) noexcept;

    expected<void, IpcChannelError> closeFileDescriptor() noexcept;
    static expected<void, IpcChannelError> closeFileDescriptor(const UdsName_t& name,
                                                               const int sockfd,
                                                               const sockaddr_un& sockAddr,
                                                               IpcChannelSide channelSide) noexcept;

    expected<void, IpcChannelError> send(const char* msg, uint64_t msgSize, const units::Duration& timeout) const noexcept;
    expected<void, IpcChannelError> receive(char* msg, uint64_t msgSize, const units::Duration& timeout) const noexcept;

  private:
    static constexpr int32_t ERROR_CODE = -1;
    static constexpr int32_t INVALID_FD = -1;

    UdsName_t m_name;
    IpcChannelSide m_channelSide = IpcChannelSide::CLIENT;
    int32_t m_sockfd{INVALID_FD};
    sockaddr_un m_sockAddr{};
    uint64_t m_maxMessageSize{MAX_MESSAGE_SIZE};
};

template <template <uint64_t> class C, uint64_t N>
expected<void, IpcChannelError> UnixDomainSocket::send(const C<N>& buf) const noexcept{
  // we also support timedSend. The setsockopt call sets the timeout for all further sendto calls, so we must set
  // it to 0 to turn the timeout off
  return send(buf.c_str(), buf.size(), units::Duration::fromSeconds(0ULL)); 
}

template <template <uint64_t> class C, uint64_t N>
expected<void, IpcChannelError> UnixDomainSocket::timedSend(const C<N>& buf, const units::Duration& timeout) const noexcept{
  return send(buf.c_str(), buf.size(), timeout);
}

template <template <uint64_t> class C, uint64_t N>
expected<void, IpcChannelError> UnixDomainSocket::receive(C<N>& buf) const noexcept{
  static_assert(N <= platform::IOX_UDS_SOCKET_MAX_MESSAGE_SIZE, "Size exceeds transmission limit!");
  // we also support timedSend. The setsockopt call sets the timeout for all further sendto calls, so we must set
  // it to 0 to turn the timeout off
  return receive(buf.data(), N, units::Duration::fromSeconds(0ULL)); 
}

template <template <uint64_t> class C, uint64_t N>
expected<void, IpcChannelError> UnixDomainSocket::timedReceive(C<N>& buf, const units::Duration& timeout) const noexcept{
  static_assert(N <= platform::IOX_UDS_SOCKET_MAX_MESSAGE_SIZE, "Size exceeds transmission limit!");
  return receive(buf.data(), N, timeout);
}

class UnixDomainSocketBuilder
{
    /// @brief Defines the socket name
    IOX_BUILDER_PARAMETER(IpcChannelName_t, name, "")

    /// @brief Defines how the socket is opened, i.e. as client or server
    IOX_BUILDER_PARAMETER(IpcChannelSide, channelSide, IpcChannelSide::CLIENT)

    /// @brief Defines the max message size of the socket
    IOX_BUILDER_PARAMETER(uint64_t, maxMsgSize, UnixDomainSocket::MAX_MESSAGE_SIZE)

    /// @brief Defines the max number of messages for the socket.
    IOX_BUILDER_PARAMETER(uint64_t, maxMsgNumber, UnixDomainSocket::MAX_NUMBER_OF_MESSAGES)

  public:
    /// @brief create a unix domain socket
    /// @return On success a 'UnixDomainSocket' is returned and on failure an 'IpcChannelError'.
    expected<UnixDomainSocket, IpcChannelError> create() const noexcept;
};

class UnixDomainSocketBuilderNoPathPrefix
{
    /// @brief Defines the socket name
    IOX_BUILDER_PARAMETER(UnixDomainSocket::UdsName_t, name, "")

    /// @brief Defines how the socket is opened, i.e. as client or server
    IOX_BUILDER_PARAMETER(IpcChannelSide, channelSide, IpcChannelSide::CLIENT)

    /// @brief Defines the max message size of the socket
    IOX_BUILDER_PARAMETER(uint64_t, maxMsgSize, UnixDomainSocket::MAX_MESSAGE_SIZE)

    /// @brief Defines the max number of messages for the socket.
    IOX_BUILDER_PARAMETER(uint64_t, maxMsgNumber, UnixDomainSocket::MAX_NUMBER_OF_MESSAGES)

  public:
    /// @brief create a unix domain socket
    /// @return On success a 'UnixDomainSocket' is returned and on failure an 'IpcChannelError'.
    expected<UnixDomainSocket, IpcChannelError> create() const noexcept;
};

} // namespace posix
} // namespace iox

#endif // IOX_HOOFS_POSIX_WRAPPER_UNIX_DOMAIN_SOCKET_HPP
