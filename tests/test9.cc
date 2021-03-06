#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpServer.h"
#include <cstdio>
#include <unistd.h>

void onConnection(const imitate_muduo::TcpConnectionPtr &conn) {
  if (conn->connected()) {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(), conn->peerAddress().toHostPort().c_str());
  } else {
    printf("onConnection(): connection [%s] is down\n", conn->name().c_str());
  }
}

void onMessage(const imitate_muduo::TcpConnectionPtr &conn,
               imitate_muduo::Buffer *buf,
               imitate_muduo::Timestamp recieveTime) {
  time_t time = std::chrono::system_clock::to_time_t(recieveTime);
  printf("onMessage(): recieved %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(), conn->name().c_str(), ctime(&time));
  conn->send(buf->retrieveAsString());
}

int main(int argc, char *argv[]) {
  printf("main(): pid = %d\n", getpid());
  imitate_muduo::InetAddress listenAddr(9981);
  imitate_muduo::EventLoop loop;
  imitate_muduo::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  if (argc > 1)
    server.setThreadNum(atoi(argv[1]));
  server.start();
  loop.loop();
}