#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define PIPE_NAME "\\\\.\\pipe\\MessagePipe"
#define MESSAGE_SIZE 100

void parent_process() {
    HANDLE hPipe;
    char message[] = "Hello from parent process!";
    DWORD bytesWritten;

    // Create a named pipe for the parent to write
    hPipe = CreateNamedPipe(
        PIPE_NAME,                // Pipe name
        PIPE_ACCESS_OUTBOUND,     // Write access
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,                         // Max instances
        sizeof(message),           // Output buffer size
        sizeof(message),           // Input buffer size
        0,                         // Timeout (default)
        NULL                       // Default security
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Error creating named pipe. Error code: %ld\n", GetLastError());
        exit(1);
    }

    printf("Parent Process: Waiting for child to connect...\n");

    // Connect to the pipe
    if (ConnectNamedPipe(hPipe, NULL) == FALSE) {
        printf("Error connecting to pipe. Error code: %ld\n", GetLastError());
        exit(1);
    }

    // Write the message to the pipe
    if (!WriteFile(hPipe, message, sizeof(message), &bytesWritten, NULL)) {
        printf("Error writing to pipe. Error code: %ld\n", GetLastError());
        exit(1);
    }

    printf("Parent Process: Sent message: '%s'\n", message);

    // Close the pipe
    CloseHandle(hPipe);
}

void child_process() {
    HANDLE hPipe;
    char buffer[MESSAGE_SIZE];
    DWORD bytesRead;

    // Open the named pipe for reading
    hPipe = CreateFile(
        PIPE_NAME,                 // Pipe name
        GENERIC_READ,              // Read access
        0,                         // No sharing
        NULL,                      // Default security
        OPEN_EXISTING,             // Open the existing pipe
        0,                         // Default attributes
        NULL                       // No template file
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Error opening named pipe. Error code: %ld\n", GetLastError());
        exit(1);
    }

    printf("Child Process: Waiting for message from parent...\n");

    // Read the message from the pipe
    if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        printf("Error reading from pipe. Error code: %ld\n", GetLastError());
        exit(1);
    }

    printf("Child Process: Received message: '%s'\n", buffer);

    // Close the pipe
    CloseHandle(hPipe);
}

int main() {
    // Create a child process using CreateProcess instead of fork
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create the child process
    if (!CreateProcess(
            NULL,                  // Application name (NULL means using command line)
            "child_process.exe",    // Command line (we will create a separate .exe file for child)
            NULL,                  // Process security attributes
            NULL,                  // Thread security attributes
            FALSE,                 // Handle inheritance
            0,                     // Creation flags
            NULL,                  // Environment block
            NULL,                  // Current directory
            &si,                   // Startup info
            &pi                    // Process information
        )) {
        printf("CreateProcess failed. Error code: %ld\n", GetLastError());
        return 1;
    }

    // Parent process code
    parent_process();

    // Wait for the child process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
