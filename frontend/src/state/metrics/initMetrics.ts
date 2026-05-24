import type { SystemMetrics } from "./types";
import { HISTORY_LENGTH } from "@/config/api";

const initHistory = () =>
    Array(HISTORY_LENGTH).fill(0);

export function initMetrics(): SystemMetrics {
    return {
        cpuUsage: 0,
        cpuCores: [],
        cpuTemp: 0,
        cpuFreq: 0,
        cpuHistory: initHistory(),

        ramUsed: 0,
        ramTotal: 32,
        ramCached: 0,
        ramUsage: 0,
        ramHistory: initHistory(),

        gpuUsage: 0,
        gpuTemp: 0,
        gpuVramUsed: 0,
        gpuVramTotal: 8,
        gpuHistory: initHistory(),

        netDown: 0,
        netUp: 0,
        netDownHistory: initHistory(),
        netUpHistory: initHistory(),
        netTotalDown: 0,
        netTotalUp: 0,
        netLatency: 0,

        diskRead: 0,
        diskWrite: 0,
        diskReadHistory: initHistory(),
        diskWriteHistory: initHistory(),
        diskUsed: 0,
        diskTotal: 1000,
        diskTemp: 0,

        processes: [],

        uptime: 0,

        alert_enabled: false,
        alert_active: false,
    };
}