import { useEffect, useState } from "react";
import { API_URL } from "../../../config/api";
import type { ExternalWeather } from "../../../state/metrics/types";

export function useWeather() {
    const [weather, setWeather] = useState<ExternalWeather>({
        city: "Detecting...",
        temp: 0,
    });

    useEffect(() => {
        const fetchWeather = async () => {
            try {
                const res = await fetch(`${API_URL}/api/weather`);
                if (!res.ok) return;

                setWeather(await res.json());
            } catch (err) {
                console.error("Error fetching weather:", err);
            }
        };

        fetchWeather();

        const id = setInterval(fetchWeather, 10 * 60 * 1000);

        return () => clearInterval(id);
    }, []);

    return weather;
}