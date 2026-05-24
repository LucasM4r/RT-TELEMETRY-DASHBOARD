import type { SystemMetrics } from "./types";

export function mapMetrics(
    prev: SystemMetrics,
    latest: any
): SystemMetrics {
    const cpuUsage = latest.cpu ?? 0;
    const ramUsage = latest.mem_usage ?? 0;
    const gpuUsage = latest.gpu_vram_usage ?? 0;

    const netDown = latest.net_down ?? 0;
    const netUp = latest.net_up ?? 0;

    const diskRead = latest.storage_read ?? 0;
    const diskWrite = latest.storage_write ?? 0;

    return {
        ...prev,

        cpuUsage,
        cpuTemp: latest.cpu_temp ?? 0,
        cpuFreq: latest.cpu_freq ?? 0,
        cpuHistory: [...prev.cpuHistory.slice(1), cpuUsage],

        cpuCores: latest.cpu_cores
            ? latest.cpu_cores.map((usage: number, i: number) => ({
                id: i,
                usage,
            }))
            : prev.cpuCores,

        ramUsed: latest.mem_used ?? 0,
        ramCached: latest.mem_cached ?? 0,
        ramUsage,
        ramHistory: [...prev.ramHistory.slice(1), ramUsage],

        gpuUsage,
        gpuTemp: latest.gpu_temp ?? 0,
        gpuVramUsed: latest.gpu_vram_used ?? 0,
        gpuHistory: [...prev.gpuHistory.slice(1), gpuUsage],

        netDown,
        netUp,
        netDownHistory: [
            ...prev.netDownHistory.slice(1),
            (netDown / 150) * 100,
        ],
        netUpHistory: [
            ...prev.netUpHistory.slice(1),
            (netUp / 50) * 100,
        ],

        netTotalDown: prev.netTotalDown + netDown / 8 / 1024,
        netTotalUp: prev.netTotalUp + netUp / 8 / 1024,

        diskRead,
        diskWrite,
        diskTemp: latest.storage_temp ?? 0,
        diskUsed: latest.storage_used ?? 0,

        diskReadHistory: [
            ...prev.diskReadHistory.slice(1),
            (diskRead / 400) * 100,
        ],
        diskWriteHistory: [
            ...prev.diskWriteHistory.slice(1),
            (diskWrite / 200) * 100,
        ],

        alert_enabled: latest.alert_enabled ?? false,
        alert_active: !!latest.alert_active,

        uptime: prev.uptime + 1,
    };
}