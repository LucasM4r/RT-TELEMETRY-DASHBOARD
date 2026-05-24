export interface ExternalWeather {
    city: string;
    temp: number;
    condition?: number;
}

export interface CoreUsage {
    id: number;
    usage: number;
}

export interface ProcessEntry {
    pid: number;
    name: string;
    cpu: number;
    ram: number;
    status: "running" | "sleeping" | "idle";
}

export interface SystemMetrics {
    externalWeather?: ExternalWeather;

    cpuUsage: number;
    cpuCores: CoreUsage[];
    cpuTemp: number;
    cpuFreq: number;
    cpuHistory: number[];

    ramUsed: number;
    ramTotal: number;
    ramCached: number;
    ramUsage: number;
    ramHistory: number[];

    gpuUsage: number;
    gpuTemp: number;
    gpuVramUsed: number;
    gpuVramTotal: number;
    gpuHistory: number[];

    netDown: number;
    netUp: number;
    netDownHistory: number[];
    netUpHistory: number[];
    netTotalDown: number;
    netTotalUp: number;
    netLatency: number;

    diskRead: number;
    diskWrite: number;
    diskReadHistory: number[];
    diskWriteHistory: number[];
    diskUsed: number;
    diskTotal: number;
    diskTemp: number;

    processes: ProcessEntry[];

    uptime: number;

    alert_enabled: boolean;
    alert_active: boolean;
}