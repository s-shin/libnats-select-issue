#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "nats/nats.h"

void NatsTest(const std::string &url) {
  natsStatus status;

  std::shared_ptr<natsConnection> conn;
  {
    natsConnection *nc = nullptr;
    status = natsConnection_ConnectTo(&nc, url.c_str());
    if (status != NATS_OK) {
      std::cerr << "ERROR: natsConnection_ConnectTo: "
                << natsStatus_GetText(status) << std::endl;
      return;
    }
    conn.reset(nc, natsConnection_Destroy);
  }

  std::shared_ptr<natsSubscription> sub;
  {
    natsSubscription *ns = nullptr;
    natsConnection_Subscribe(
        &ns, conn.get(), "foo",
        [](natsConnection *, natsSubscription *, natsMsg *msg, void *) {
          auto data = natsMsg_GetData(msg);
          auto len = natsMsg_GetDataLength(msg);
          std::cout << natsMsg_GetSubject(msg)
                    << " <= " << std::string(data, data + len) << std::endl;
          natsMsg_Destroy(msg);
        },
        nullptr);
    sub.reset(ns, natsSubscription_Destroy);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  natsConnection_PublishString(conn.get(), "foo", "bar");

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  natsSubscription_Unsubscribe(sub.get());
  natsConnection_Close(conn.get());
}

int main(int argc, char **argv) {
  int num_files = 0;
  std::string nats_url = NATS_DEFAULT_URL;

  if (argc < 1 || 3 < argc) {
    std::cout << "Usage: " << argv[0] << " <num-open-files> [<nats-url>]"
              << std::endl;
    return 1;
  }
  if (argc > 1) {
    num_files = std::atoi(argv[1]);
    if (num_files < 0) {
      std::cerr << "ERROR: invalid argv[1]" << std::endl;
      return 1;
    }
  }
  if (argc > 2) {
    nats_url.assign(argv[2]);
  }

  std::vector<FILE *> fps;
  for (int i = 1; i <= num_files; ++i) {
    auto path = argv[0];
    auto fp = fopen(path, "r");
    if (fp == nullptr) {
      std::cerr << "ERROR: fopen(" << path << "): " << strerror(errno)
                << std::endl;
      for (auto fp : fps) {
        fclose(fp);
      }
      return 1;
    }
    fps.emplace_back(fp);
  }

  if (fps.empty()) {
    std::cout << "no files opened" << std::endl;
  } else {
    std::cout << fps.size() << " files opened (min fd: " << fileno(fps.front())
              << ", max fd: " << fileno(fps.back()) << ")" << std::endl;
  }

  std::cout << "nats url: " << nats_url << std::endl;
  NatsTest(nats_url);

  std::cout << "done" << std::endl;

  for (auto fp : fps) {
    fclose(fp);
  }
  return 0;
}
