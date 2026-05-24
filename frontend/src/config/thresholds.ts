import { COLORS } from "./colors";

export const THRESHOLD_CONTROLS = [
    {
        label: "CPU Usage Limit",
        key: "cpu",
        unit: "%",
        min: 10,
        max: 100,
        color: COLORS.cpu,
    },
    {
        label: "RAM Usage Limit",
        key: "ram",
        unit: "%",
        min: 10,
        max: 100,
        color: COLORS.ram,
    },
    {
        label: "CPU Temp Limit",
        key: "cpuTemp",
        unit: "°C",
        min: 30,
        max: 105,
        color: COLORS.temp,
    },
    {
        label: "GPU Temp Limit",
        key: "gpuTemp",
        unit: "°C",
        min: 30,
        max: 105,
        color: COLORS.gpu,
    },
    {
        label: "Storage Limit",
        key: "storage",
        unit: "%",
        min: 10,
        max: 100,
        color: COLORS.disk,
    },
] as const;