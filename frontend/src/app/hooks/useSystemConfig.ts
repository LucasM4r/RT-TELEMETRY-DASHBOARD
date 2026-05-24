import { useState, useEffect } from "react";

const API_URL =
    import.meta.env.VITE_API_URL ||
    `http://${window.location.hostname}:3001`;

type Limits = {
    cpu: number;
    ram: number;
    cpuTemp: number;
    gpuTemp: number;
    storage: number;
};

const DEFAULT_LIMITS: Limits = {
    cpu: 90,
    ram: 95,
    cpuTemp: 85,
    gpuTemp: 85,
    storage: 98,
};

const metricMap: Record<keyof Limits, string> = {
    cpu: "cpu",
    ram: "mem",
    cpuTemp: "cpu_temp",
    gpuTemp: "gpu_temp",
    storage: "storage",
};

export function useSystemConfig() {
    const [limits, setLimits] = useState<Limits>(DEFAULT_LIMITS);

    useEffect(() => {
        const fetchConfig = async () => {
            try {
                const response = await fetch(`${API_URL}/api/config`);

                if (!response.ok) {
                    throw new Error("Failed to load config");
                }

                const data = await response.json();

                setLimits({
                    cpu: data.cpu_alert_limit ?? data.cpu_usage_limit ?? 90,
                    ram: data.mem_alert_limit ?? data.mem_usage_limit ?? 95,
                    cpuTemp:
                        data.cpu_temp_alert_limit ??
                        data.cpu_temp_limit ??
                        85,
                    gpuTemp:
                        data.gpu_temp_alert_limit ??
                        data.gpu_temp_limit ??
                        85,
                    storage:
                        data.storage_alert_limit ??
                        data.storage_usage_limit ??
                        98,
                });
            } catch (error) {
                console.error(
                    "[useSystemConfig] Failed to fetch config:",
                    error
                );
            }
        };

        fetchConfig();
    }, []);

    const updateLimit = async (
        metric: keyof Limits,
        value: number
    ) => {
        try {
            const target = metricMap[metric];

            const response = await fetch(
                `${API_URL}/api/config/${target}`,
                {
                    method: "PATCH",
                    headers: {
                        "Content-Type": "application/json",
                    },
                    body: JSON.stringify({
                        value,
                    }),
                }
            );

            if (!response.ok) {
                throw new Error("Failed to update config");
            }

            setLimits((prev) => ({
                ...prev,
                [metric]: value,
            }));
        } catch (error) {
            console.warn(
                "[useSystemConfig] Failed to update config:",
                error
            );
        }
    };

    return {
        limits,
        updateLimit,
    };
}