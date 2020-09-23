#ifndef REACTOR_MULTI_REACTORS_MULTI_THREADS_UTILITIES_H_
#define REACTOR_MULTI_REACTORS_MULTI_THREADS_UTILITIES_H_

#include <fcntl.h>
#include <sys/socket.h>

namespace net {
namespace mrmt {

void setReuseAddr(int socket_fd, bool on);

void setReusePort(int socket_fd, bool on);

void setNoBlock(int socket_fd);

void setCloseOnExec(int socket_fd);

}  // namespace mrmt
}  // namespace net

#endif  // REACTOR_MULTI_REACTORS_MULTI_THREADS_UTILITIES_H_
