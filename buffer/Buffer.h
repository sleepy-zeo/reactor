#ifndef REACTOR_DATA_BUFFER_H_
#define REACTOR_DATA_BUFFER_H_

#include <vector>

/**
 * 1024字节
 *
 * +----------------+------------------+------------------+
 * |  unused bytes  |  readable bytes  |  writable bytes  |
 * |                |     (CONTENT)    |                  |
 * +----------------+------------------+------------------+
 * |                |                  |                  |
 * 0     <=    readerIndex   <=   writerIndex    <=     size
 *
 * readerIndex有数据，writeIndex无数据
 */
class Buffer {

public:
    explicit Buffer(int initSize = INIT_SIZE);
    ~Buffer() = default;

    char* read(int len);
    bool write(char* data, int len);

    int readableBytes();
    int writableBytes();
    int unusedBytes();

    std::vector<char>* getBuffer();

    void retrieve(size_t n);
    void retrieveAll();

    bool isEmpty();
    void reset();

    size_t readFd(int fd, int* savedErrno);

private:
    void ensureSpace(int len);
    void makeSpace(int len);
    bool updateWriteIndex(int len);
    bool updateReadIndex(int len);

    static const int INIT_SIZE = 1024;

    std::vector<char> m_buffer;
    int m_readIndex;
    int m_writeIndex;
};

#endif  // REACTOR_DATA_BUFFER_H_
