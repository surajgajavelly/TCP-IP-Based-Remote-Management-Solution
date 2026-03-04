# Sentinel Proc: TCP/IP Remote Management Solution (V1.0)
A high-performance, concurrent remote diagnostic agent designed for Linux-based systems. This solution allows system administrators to monitor real-time process telemetry—including memory consumption, CPU load, and network resources—across a network using a custom-engineered binary protocol.

Instead of relying on heavy shell-based tools, this project interfaces directly with the **Linux Kernel via the ProcFS (`/proc`) virtual filesystem** using low-level system calls, ensuring a minimal resource footprint on the target system.

## V1.0 Core Features
**Concurrent Server Architecture**: Implemented a multi-process model using the `fork()` API, enabling the server to handle multiple Network Operations Center (NOC) clients simultaneously without blocking.

**Direct ProcFS Parsing**: Manual extraction of kernel-level metrics using raw system calls (`open`, `read`, `readlink`):

**VmRSS Tracking**: Monitors actual physical memory (Resident Set Size) from `/proc/[pid]/status`.

**Differential CPU Sampling**: Calculates CPU load delta using `utime` and `stime` ticks over a sample window from `/proc/[pid]/stat`.

**FD & Socket Resolution**: Counts open file descriptors and identifies active network sockets via symbolic link resolution in `/proc/[pid]/fd`.

**Persistent Session History**: Manages a sliding-window command history (last 50 commands) with state persistence to a hidden `.remote_history` file.

**Remote Process Control**: Integrated process termination capability using the `SIGKILL` signal via the `kill()` system call.

**Robust Buffer Safety**: Utilizes boundary-checked string formatting (`snprintf`) across all diagnostic functions to mitigate buffer overflow vulnerabilities.

---

## Project Structure
This project follows a clean, professional, and scalable directory structure:
```
.
├── bin/                # Compiled executables (Server/Client)
├── include/            # Header files (API Definitions & Macros)
│   ├── common.h        # Shared constants (MAX_BUF, MAX_HISTORY)
│   ├── history.h       # History module interface
│   ├── network_utils.h # Socket abstraction layer
│   └── proc_utils.h    # Linux Internals / ProcFS interface
├── src/                # Implementation files
│   ├── client_main.c   # Interactive diagnostic terminal
│   ├── history.c       # Circular-buffer history logic
│   ├── network_utils.c # TCP connection & protocol handling
│   ├── proc_utils.c    # Low-level kernel data extraction
│   └── server_main.c   # Multi-process agent core
├── .remote_history     # Hidden persistence file
├── Makefile            # Automated build system
└── test_suite.sh       # Automated integration & regression test script
```
---
## 🛠️ Technology Stack & Workflow

| Category | Tool/Standard |
| :--- | :--- |
| Language | C (C11) |
| IPC | POSIX Sockets (TCP/IP) |
| Concurrency | Multi-processing (fork) |
| VFS Access | Linux ProcFS (/proc) |
| Build System | GNU Make |
| Testing | Custom Shell Integration Suite |
| Version Control | Git (Semantic Tagging v1.0) |

---

## How to Build
This is the easiest and most reliable way to build.

1.  **Build the project:**

    The project uses a centralized Makefile for streamlined compilation:
    ```bash
    make clean && make
    ```
2.  **Launch the Agent (Server)**

    Start the diagnostic server on the target machine (specify a port, e.g., 8080):
    ```bash
    ./bin/server 8080
    ```
3. **Connect the Console (Client)**

    On the monitoring machine, connect to the agent's IP and port:
    ```bash
    ./bin/client 127.0.0.1 8080
    ```

---

## Roadmap (V2.0 Updates)
Planned enhancements to increase security and system depth:

**Transport Layer Security**: Implement OpenSSL/TLS to encrypt the command channel.

**Containerization**: Add Multi-stage Dockerfiles for zero-install deployment.

**Advanced Metrics**: Support for `/proc/net/dev` to monitor real-time network bandwidth.

**User Interface**: Implement Up/Down arrow key history navigation using termios raw mode.

**Graceful Shutdown**: Implement advanced signal handling for the server to clean up child processes on `SIGINT`.

## Contributing
Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'feat: Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License
Distributed under the MIT License. See `LICENSE` for more information.