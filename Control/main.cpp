#include <iostream>
#include <cwchar>
#include <vector>

#include <Windows.h>
#include <conio.h>

typedef PROCESS_INFORMATION ProcessInfo;
typedef STARTUPINFOA StartUpInfo;

HANDLE allowedEvent;

ProcessInfo * createHandler() {
  StartUpInfo startUpInfo;
  ZeroMemory(&startUpInfo, sizeof(startUpInfo));
  startUpInfo.cb = sizeof(startUpInfo);

  ProcessInfo *processInfo = new ProcessInfo;
  ZeroMemory(processInfo, sizeof(processInfo));

  CreateProcessA(nullptr, LPSTR("Handler.exe"), nullptr, nullptr, false,
                 0, nullptr, nullptr, &startUpInfo,
                 processInfo);

  return processInfo;
}

DWORD getProcessExitCode(const ProcessInfo * info) {
  DWORD exitCode;
  GetExitCodeProcess(info->hProcess, &exitCode);
  return exitCode;
}

void newProcess(std::vector<ProcessInfo *> &processes) {
  processes.push_back(createHandler());
}

void deleteProcess(std::vector<ProcessInfo *> &processes) {
  if (!processes.empty()) {
    auto process = processes.back();
    processes.pop_back();
    TerminateProcess(process->hProcess, 0);
    WaitForSingleObject(process->hProcess, INFINITE);
  }

  if (processes.empty())
    SetEvent(allowedEvent);
}

void terminateProcesses(std::vector<ProcessInfo *> &processes) {
  for (auto process : processes) {
    TerminateProcess(process->hProcess, 0);
    WaitForSingleObject(process->hProcess, INFINITE);
  }
}

bool handleInput(std::vector<ProcessInfo *> &processes) {
  char c = _getch();
  switch (c) {
  case '+':
    newProcess(processes);
    break;
  case '-':
    deleteProcess(processes);
    break;
  case 'q':
    terminateProcesses(processes);
    return false;
  }
  return true;
}

void handleSelfTerminated(std::vector<ProcessInfo *> &processes) {
  std::vector<ProcessInfo *> allocator;
  for (const auto process : processes) {
    if (getProcessExitCode(process) != STILL_ACTIVE) {  
      delete process;
    } else {
      allocator.push_back(process);
    }
  }
  processes = allocator;
}

int main(const int argc, const char *argv[]) { 
  allowedEvent = CreateEventA(nullptr, false, true, "Allowed");
  std::vector<ProcessInfo *> processes;

  while (true) {
    if (!handleInput(processes))
      break;

    handleSelfTerminated(processes);
  }

  
  return 0;
}