#ifndef REACTOR_SINGLE_REACTOR_SINGLE_THREAD_UTILITIES_H_
#define REACTOR_SINGLE_REACTOR_SINGLE_THREAD_UTILITIES_H_

#include <fcntl.h>
#include <sys/socket.h>

namespace net {
namespace srst {

void setReuseAddr(int socket_fd, bool on);

void setReusePort(int socket_fd, bool on);

void setNoBlock(int socket_fd);

void setCloseOnExec(int socket_fd);

}  // namespace srst
}  // namespace net

#endif  // REACTOR_SINGLE_REACTOR_SINGLE_THREAD_UTILITIES_H_
