# RT-TELEMETRY-DASHBOARD

A telemetry system for hardware metrics collection, processing, and visualization. Built with a C-based collector, a Go backend, and a React frontend.

---

## System Architecture

The architecture is decoupled into three main layers: data collection, processing/routing, and visualization.

```mermaid
---
config:
  themeVariables:
    lineColor: '#888'
  theme: dark
  layout: dagre
---
flowchart TB
 subgraph Hardware["Hardware / OS"]
        Kernel[/"Linux Kernel: /proc & /sys"/]
        CPU["CPU"]
        GPU["GPU"]
        RAM["RAM"]
        Disk["Disk"]
        Net["Net"]
  end
 subgraph C_Server["Server Container"]
    direction TB
        NetThread["Network Threads"]
        Mutex["Mutex"]
        MonThreads["Monitoring Threads"]
  end
 subgraph Backend["Backend Container"]
        Go["Golang Middleware"]
  end
 subgraph Client["Frontend Container"]
        React["React Frontend"]
  end
    Kernel --- CPU & GPU & RAM & Disk & Net
    NetThread <---> Mutex
    MonThreads <---> Mutex
    Kernel -- Parsing --> MonThreads
    React -- HTTP (API REST) --> Go
    Go -. WebSocket .-> React
    Go <== TCP Socket ==> NetThread

    style Kernel fill:#2d2d2d,stroke:#fff,color:#fff
    style Mutex fill:#ff4444,color:#fff
    style Go fill:#2d2d2d,stroke:#68a063,color:#fff
    style React fill:#2d2d2d,stroke:#00d8ff,color:#fff
    style Hardware fill:#1a1a1a,stroke:#555,stroke-width:2px,color:#fff
    style C_Server fill:#1a1a1a,stroke:#ff4444,stroke-width:2px,color:#fff
    style Backend fill:#1a1a1a,stroke:#68a063,stroke-width:2px,color:#fff
    style Client fill:#1a1a1a,stroke:#00d8ff,stroke-width:2px,color:#fff
```

### Components
1. **Server Container (C):** Collects hardware and OS metrics directly from /proc and /sys, transmitting structured data over a TCP socket.
2. **Backend Container (Go):** Connects to the C server via TCP, manages the internal event hub, and exposes data to the client via WebSockets and a REST API.

3. **Frontend Container (React):** Consumes the WebSocket streams to render telemetry charts in real time.

### Tech Stack

- **Collector:** C
- **Backend:** Go
- **Frontend:** React
- **Communication:** TCP, WebSocket, REST API
- **Infrastructure:** Docke, Docker Compose

---

## Key Features

- Hardware telemetry extraction (CPU, GPU, RAM, Disk, Network).
- Kernel-level data parsing (/proc and /sys).
- Asynchronous data pipeline using TCP and WebSockets.
- Containerized environment for isolated deployment.

## User Interface

The web interface provides a real-time overview of the monitored metrics:

![Interface](docs/interface.png)

---

## Getting Started

### Prerequisites

- Docker and Docker Compose installed.

- Read permissions on the host system to allow the monitoring container to parse /proc and /sys files..

### Installation & Run

1. **Clone the repository:**
```bash
git clone https://github.com/LucasM4r/RT-TELEMETRY-DASHBOARD.git
cd RT-Telemetry-Dashboard
```
2. **Boot the environment using the Makefile:**
```bash
make run
```

Alternatively, using standard Docker Compose:

```bash
docker compose up --build
```

[![DOI](https://zenodo.org/badge/1232322663.svg)](https://doi.org/10.5281/zenodo.20364557)
