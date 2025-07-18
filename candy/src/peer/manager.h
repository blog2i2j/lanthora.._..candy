// SPDX-License-Identifier: MIT
#ifndef CANDY_PEER_MANAGER_H
#define CANDY_PEER_MANAGER_H

#include "core/message.h"
#include "core/net.h"
#include "peer/message.h"
#include "peer/peer.h"
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/URI.h>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace candy {

using Poco::Net::SocketAddress;

class Client;

struct Stun {
    std::string uri;
    SocketAddress address;
    bool needed = false;
    IP4 ip;
    uint16_t port;

    bool enabled() {
        return !this->address.host().isWildcard();
    }

    int update() {
        try {
            if (!this->uri.empty()) {
                Poco::URI uri(this->uri);
                if (!uri.getPort()) {
                    uri.setPort(3478);
                }
                this->address = Poco::Net::SocketAddress(uri.getHost(), uri.getPort());
            }
            return 0;
        } catch (std::exception &e) {
            spdlog::warn("set stun server address failed: {}", e.what());
            return -1;
        }
    }
};

struct PeerRouteEntry {
    IP4 dst;
    IP4 next;
    int32_t rtt;

    PeerRouteEntry(IP4 dst = IP4(), IP4 next = IP4(), int32_t rtt = RTT_LIMIT) : dst(dst), next(next), rtt(rtt) {}
};

class PeerManager {
public:
    int setPassword(const std::string &password);
    int setStun(const std::string &stun);
    int setDiscoveryInterval(int interval);
    int setRouteCost(int cost);
    int setPort(int port);
    int setLocalhost(const std::string &ip);

    int run(Client *client);
    int wait();

    std::string getPassword();

private:
    std::string password;
    IP4 localhost;

public:
    int sendPubInfo(CoreMsg::PubInfo info);
    IP4 getTunIp();
    int updateRtTable(PeerRouteEntry entry);

private:
    // 处理来自消息队列的数据
    int handlePeerQueue();
    int handlePacket(Msg msg);
    int handleTunAddr(Msg msg);
    int handleTryP2P(Msg msg);
    int handlePubInfo(Msg msg);

    std::thread msgThread;

    int sendPacket(IP4 dst, const Msg &msg);
    int sendPacketDirect(IP4 dst, const Msg &msg);
    int sendPacketRelay(IP4 dst, const Msg &msg);

    Address tunAddr;

    int startTickThread();
    int tick();
    std::thread tickThread;
    uint64_t tickTick = randomUint32();

    std::shared_mutex ipPeerMutex;
    std::unordered_map<IP4, Peer> ipPeerMap;

    void showRtChange(const PeerRouteEntry &entry);
    int sendRtMessage(IP4 dst, int32_t rtt);

    std::shared_mutex rtTableMutex;
    std::unordered_map<IP4, PeerRouteEntry> rtTableMap;

public:
    Stun stun;
    std::atomic<bool> localP2PDisabled;

private:
    int initSocket();
    void sendStunRequest();
    void handleStunResponse(std::string buffer);
    void handleMessage(std::string buffer, const SocketAddress &address);
    void handleHeartbeatMessage(std::string buffer, const SocketAddress &address);
    void handleForwardMessage(std::string buffer, const SocketAddress &address);
    void handleDelayMessage(std::string buffer, const SocketAddress &address);
    void handleRouteMessage(std::string buffer, const SocketAddress &address);
    void poll();

    std::optional<std::string> decrypt(const std::string &ciphertext);
    std::shared_ptr<EVP_CIPHER_CTX> decryptCtx;
    std::mutex decryptCtxMutex;
    std::string key;

    // 默认监听端口,如果不配置,随机监听
    uint16_t listenPort = 0;

public:
    std::mutex socketMutex;
    Poco::Net::DatagramSocket socket;
    int sendTo(const void *buffer, int length, const SocketAddress &address);
    int getDiscoveryInterval() const;
    bool clientRelayEnabled() const;

private:
    std::thread pollThread;

    int discoveryInterval = 0;
    int routeCost = 0;

    Client &getClient();
    Client *client;
};

} // namespace candy

#endif
