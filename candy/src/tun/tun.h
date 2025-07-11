// SPDX-License-Identifier: MIT
#ifndef CANDY_TUN_TUN_H
#define CANDY_TUN_TUN_H

#include "core/message.h"
#include "core/net.h"
#include <any>
#include <list>
#include <shared_mutex>
#include <string>
#include <thread>

namespace candy {

class Client;

class Tun {
public:
    Tun();
    ~Tun();

    int setName(const std::string &name);
    int setMTU(int mtu);

    int run(Client *client);
    int wait();

    IP4 getIP();

private:
    int setAddress(const std::string &cidr);

    // 处理来自 TUN 设备的数据
    int handleTunDevice();

    // 处理来自消息队列的数据
    int handleTunQueue();
    int handlePacket(Msg msg);
    int handleTunAddr(Msg msg);
    int handleSysRt(Msg msg);

    std::string tunAddress;
    std::thread tunThread;
    std::thread msgThread;

private:
    int up();
    int down();

    int read(std::string &buffer);
    int write(const std::string &buffer);

    int setSysRtTable(const SysRouteEntry &entry);
    int setSysRtTable(IP4 dst, IP4 mask, IP4 nexthop);

    std::shared_mutex sysRtMutex;
    std::list<SysRouteEntry> sysRtTable;

private:
    std::any impl;

private:
    Client &getClient();
    Client *client;
};

} // namespace candy

#endif
