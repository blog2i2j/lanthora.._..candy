// SPDX-License-Identifier: MIT
#include "peer/peer.h"
#include "utility/address.h"
#include <cstdlib>
#include <openssl/sha.h>
#include <spdlog/spdlog.h>

namespace Candy {

int PeerInfo::setTun(uint32_t tun, const std::string &password) {
    this->tun = tun;
    std::string data;
    data.append(password);
    data.append((char *)&this->tun, sizeof(this->tun));
    this->key.resize(SHA256_DIGEST_LENGTH);
    SHA256((unsigned char *)data.data(), data.size(), (unsigned char *)this->key.data());
    return 0;
}

std::string PeerInfo::getKey() const {
    return this->key;
}

uint32_t PeerInfo::getTun() const {
    return this->tun;
}

void PeerInfo::updateState(PeerState state) {
    this->count = 0;
    if (this->state == state) {
        return;
    }

    spdlog::debug("conn state: {} {} => {}", Address::ipToStr(this->tun), getStateStr(this->state), getStateStr(state));
    if (state == PeerState::INIT || state == PeerState::WAITTING || state == PeerState::FAILED) {
        this->ip = 0;
        this->port = 0;
        this->ack = 0;
        this->retry = RETRY_MIN;
        this->delay = DELAY_LIMIT;
    }
    this->state = state;
}

PeerState PeerInfo::getState() const {
    return this->state;
}

std::string PeerInfo::getStateStr() const {
    return getStateStr(this->state);
}

std::string PeerInfo::getStateStr(PeerState state) {
    switch (state) {
    case PeerState::INIT:
        return "INIT";
    case PeerState::PREPARING:
        return "PREPARING";
    case PeerState::SYNCHRONIZING:
        return "SYNCHRONIZING";
    case PeerState::CONNECTING:
        return "CONNECTING";
    case PeerState::CONNECTED:
        return "CONNECTED";
    case PeerState::WAITTING:
        return "WAITTING";
    case PeerState::FAILED:
        return "FAILED";
    default:
        return "UNKNOWN";
    }
}

void UdpHolder::setBindPort(uint16_t port) {
    this->port = port;
}

} // namespace Candy
