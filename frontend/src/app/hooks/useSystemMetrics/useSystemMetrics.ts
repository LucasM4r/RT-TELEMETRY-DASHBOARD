import { useState } from "react";
import { initMetrics } from "../../../state/metrics/initMetrics";
import { useWeather } from "./useWeather";
import { useSystemWebSocket } from "./useSystemWebSocket";
import { API_URL } from "../../../config/api";

export function useSystemMetrics() {
    const [metrics, setMetrics] = useState(initMetrics());

    const weather = useWeather();
    useSystemWebSocket(setMetrics);

    const toggleAlarm = async (enabled: boolean) => {
        const endpoint = enabled
            ? "/api/alert/stop"
            : "/api/alert/enable";

        try {
            await fetch(`${API_URL}${endpoint}`, {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
            });
        } catch (err) {
            console.error("Error toggling alarm:", err);
        }
    };

    return {
        ...metrics,
        externalWeather: weather,
        toggleAlarm,
    };
}