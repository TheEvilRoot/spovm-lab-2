#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

using namespace std;

int currentPrintingPid = 0;

int flag = 0;
int fSomeonePrinting = 0;
int fPrintDone = 1;

std::vector<pid_t> processes;

void addProcess() {
  pid_t newPid = fork();
  if (newPid == 0) {
    const char *argument = std::to_string(processes.size()).c_str();
    execlp("./handler", argument, nullptr);
  } else processes.push_back(newPid);
}

void processPrint() {
  if (fPrintDone && !processes.empty()) {
    fPrintDone = 0;
    if (currentPrintingPid >= processes.size() - 1) {
      currentPrintingPid = 0;
    } else if (!flag) {
      currentPrintingPid++;
    }
    flag = 0;
    kill(processes[currentPrintingPid], SIGUSR1);
  }
}

void terminateLast() {
  if (!processes.empty()) {
    kill(processes.back(), SIGUSR2);
    waitpid(processes.back(), 0, 0);

    processes.pop_back();

    if (currentPrintingPid >= processes.size()) {
      currentPrintingPid = 0;
      flag = 1;
      fPrintDone = 1;
    }
  }
}

void terminateAll() {
  if (processes.back() != 0)
    while (!processes.empty()) {
      kill(processes.back(), SIGUSR2);
      waitpid(processes.back(), nullptr, 0);
      processes.pop_back();
    }
}

void setPrinting(int) {
  fSomeonePrinting = 1;
}

void setDone(int) {
  fPrintDone = 1;
}

int main(int argc, char *argv[]) {
  struct sigaction isPrintingSignal{};
  struct sigaction isDoneSignal{};

  isPrintingSignal.sa_handler = setPrinting;
  sigaction(SIGUSR1, &isPrintingSignal, nullptr);

  isDoneSignal.sa_handler = &setDone;
  sigaction(SIGUSR2, &isDoneSignal, nullptr);

  system("/bin/stty raw");

  while (true) {
    int c = getchar();
    if (c == '+')
      addProcess();
    else if (c == '-')
      terminateLast();
    else if (c == 'q')
      break;
    processPrint();
  }

  terminateAll();
  system("/bin/stty cooked");
  return 0;
}
