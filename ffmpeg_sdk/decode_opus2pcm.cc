#include <version.h>
#include <cstdio>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
}
int main(int argc, char* argv[]) {
  std::cout << argv[0] << " Version " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
  AVCodecContext* prase;
  return 0;
}