#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

int XXX = 0;
int createDatafile(size_t n) {
  int fd;
  ssize_t res;
  char tmp[1024];

  ++XXX;

  char buf[128];
  buf[0] = 'T';
  buf[1] = ((XXX / 1000) % 10) + '0';
  buf[2] = ((XXX /  100) % 10) + '0';
  buf[3] = ((XXX /   10) % 10) + '0';
  buf[4] = ((XXX /    1) % 10) + '0';
  buf[5] = '\0';

  unlink(buf);
  fd = open(buf, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

  if (fd < 0) {
    std::cerr << "cannot open mmap file " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  memset(tmp, 0, sizeof(tmp));

  while (0 < n) {
    size_t l = sizeof(tmp);

    if (n < l) {
      l = n;
    }

    res = write(fd, tmp, l);

    if (res < 0) {
      std::cerr << "cannot write mmap file " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }

    n -= res;
  }

  lseek(fd, 0, SEEK_SET);

  return fd;
}


int main(int argc, char* argv[]) {
  size_t gb = 1024 * 1024 * 1024;

  if (argc != 4) {
    std::cerr << "usage: " << argv[0] << " <memory-in-gb> <mmap-in-gb> <sleeptime>\n";
    exit(EXIT_FAILURE);
  }

  size_t mem = atoi(argv[1]);
  size_t map = atoi(argv[2]);
  size_t pause = atoi(argv[3]);

  std::cout << "Memory: " << mem << " GB\n"
	    << "MMAP:   " << map << " GB\n";

  std::cout << "Starting memory allocation ... " << std::flush;

  std::vector<void*> memories;

  for (size_t i = 0; i < mem; ++i) {
    void* res = (void*) malloc(gb);

    if (res == nullptr) {
      std::cerr << "cannot allocate memory\n";
      exit(EXIT_FAILURE);
    } else {
      memset(res, 'A', gb);
    }

    memories.emplace_back(res);

    std::cout << i << " " << std::flush;
  }

  std::cout << std::endl;

  std::cout << "Sleeping for " << pause << "secs ... " << std::flush;
  sleep(pause);
  std::cout << std::endl;

  std::cout << "Starting MAP allocation ... " << std::flush;

  std::vector<void*> mmaps;

  for (size_t i = 0; i < map; ++i) {
    int fd = createDatafile(gb);
    void* res = mmap(nullptr, gb, PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);

    if (res == nullptr) {
      std::cerr << "cannot memory map file\n";
      exit(EXIT_FAILURE);
    } else {
      memset(res, 'B', gb);
    }

    mmaps.emplace_back(res);

    std::cout << i << " " << std::flush;
  }

  std::cout << std::endl;

  std::cout << "Sleeping for " << pause << "secs ... " << std::flush;
  sleep(pause);
  std::cout << std::endl;

  std::cout << "Freeing MAP ... " << std::flush;

  for (auto p : mmaps) {
    munmap(p, gb);
  }

  std::cout << "done" << std::endl;

  std::cout << "Sleeping for " << pause << "secs ... " << std::flush;
  sleep(pause);
  std::cout << std::endl;

  std::cout << "Freeing MEMORY ... " << std::flush;

  for (auto p : memories) {
    free(p);
  }

  std::cout << "done" << std::endl;

  std::cout << "Sleeping for " << pause << "secs ... " << std::flush;
  sleep(pause);
  std::cout << std::endl;
}
