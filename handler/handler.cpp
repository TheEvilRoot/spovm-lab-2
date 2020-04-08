#include <unistd.h>
#include <csignal>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <string>

using namespace std;

int fPrintAllowed = 0;
int fPrintDone = 1;

int getRandomInt(int min, int max) {
  return min + (rand() % static_cast<int>(max - min + 1));
}

char * getRandomString(size_t size) {
  char *string = new char[size + 1];

  for (size_t i = 0; i < size; i++) {
    if (getRandomInt(0, 3) > 1) {
      string[i] = static_cast<char>(getRandomInt('A', 'Z') % 255);
    } else {
      string[i] = static_cast<char>(getRandomInt('a', 'z') % 255);
    }
  }

  string[size] = '\0';
  return string;
}

void setPrintAllowed(int) {
  fPrintAllowed = 1;
}

void setDone(int) {
  fPrintDone = 1;
}

int main(int argc, char *argv[]) {
  srand(time(nullptr));
  char *uniqueString = getRandomString(10);

  // Setup signals
  struct sigaction allowPrintSignal{};
  struct sigaction doneSignal{};

  allowPrintSignal.sa_handler = setPrintAllowed;
  sigaction(SIGUSR1, &allowPrintSignal, nullptr);

  doneSignal.sa_handler = setDone;
  sigaction(SIGUSR2, &doneSignal, nullptr);

  // If it is the first process, acquire print
  if (std::stoi(std::string(argv[0])) == 0) {
    kill(getppid(), SIGUSR2);
  }

  fPrintDone = 0;
  while (!fPrintDone) {
    usleep(100000);
    if (fPrintAllowed) {
      fPrintAllowed = 0;

      fprintf(stderr, "\n\r");
      for (int i = 0; uniqueString[i] != 0; i++) {
        if (fPrintDone) {
          fPrintAllowed = 0;
          kill(getppid(), SIGUSR2);
          return 0;
        }

        fprintf(stderr, "%c", uniqueString[i]);
        usleep(100000);
      }

      fPrintAllowed = 0;
      kill(getppid(), SIGUSR2);
    }
  }

  return 0;
}
