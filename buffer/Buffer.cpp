#include "Buffer.h"

#include <Logger.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/uio.h>
#include <unistd.h>

const static std::string TAG{"Buffer"};

Buffer::Buffer(int initSize) {
    if (initSize <= 0) {
        initSize = INIT_SIZE;
    }
    m_buffer.resize(initSize);
    m_readIndex = 0;
    m_writeIndex = 0;
}

char* Buffer::read(int len) {
    if (readableBytes() < len) {
        throw std::runtime_error("readableBytes() < len");
    }
    char* data = new char[len];
    std::copy(m_buffer.begin() + m_readIndex, m_buffer.begin() + m_readIndex + len, data);
    updateReadIndex(len);
    return data;
}

bool Buffer::write(char* data, int len) {
    if (data == nullptr) {
        throw std::runtime_error("data is nullptr");
    }
    ensureSpace(len);
    std::copy(data, data + len, m_buffer.begin() + m_writeIndex);
    return updateWriteIndex(len);
}

void Buffer::reset() {
    ACSDK_INFO(LX(__func__));
    m_readIndex = 0;
    m_writeIndex = 0;
}

bool Buffer::isEmpty() {
    return readableBytes() == 0;
}

int Buffer::readableBytes() {
    return m_writeIndex - m_readIndex;
}

int Buffer::writableBytes() {
    return m_buffer.size() - m_writeIndex;
}

int Buffer::unusedBytes() {
    return m_readIndex;
}

void Buffer::ensureSpace(int len) {
    if (writableBytes() < len) {
        makeSpace(len);
    }
}

void Buffer::makeSpace(int len) {
    if (writableBytes() + unusedBytes() < len) {
        m_buffer.resize(m_writeIndex + len);
    } else {
        if (writableBytes() > len) {
            return;
        }
        int readableSize = readableBytes();
        std::copy(m_buffer.begin() + m_readIndex, m_buffer.begin() + m_writeIndex, m_buffer.begin());
        m_readIndex = 0;
        m_writeIndex = m_readIndex + readableSize;
    }
}

bool Buffer::updateWriteIndex(int len) {
    if (len > writableBytes()) {
        throw std::runtime_error("len > writableBytes()");
    }
    m_writeIndex += len;
    return true;
}

bool Buffer::updateReadIndex(int len) {
    if (len > readableBytes())
        throw std::runtime_error("len > readableBytes()");

    if (len < readableBytes()) {
        m_readIndex += len;
    } else {
        m_readIndex = 0;
        m_writeIndex = 0;
    }
    return true;
}

size_t Buffer::readFd(int fd, int* savedErrno) {
    char data[1024];
    memset(data, 0, sizeof(data));
    int n;
    int total = 0;
    while ((n = ::read(fd, data, sizeof(data))) > 0) {
        write(data, n);
        total += n;
    }
    return total;
}

std::vector<char>* Buffer::getBuffer() {
    return &m_buffer;
}

void Buffer::retrieveAll() {
    m_readIndex = 0;
    m_writeIndex = 0;
}

void Buffer::retrieve(size_t n) {
    if (n > readableBytes()) {
        throw std::runtime_error(std::to_string(n) + "n > readableBytes()");
    }
    if (n < readableBytes()) {
        m_readIndex += n;
    } else {
        retrieveAll();
    }
}
