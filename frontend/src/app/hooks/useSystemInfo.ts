import { useEffect, useState } from "react";
import { fetchSystemInfo } from "../../services/systemInfoService";
import type { SystemInfo } from "../../state/info/types";

export function useSystemInfo() {
    const [info, setInfo] = useState<SystemInfo | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState<string | null>(null);

    useEffect(() => {
        let mounted = true;

        const load = async () => {
            try {
                const data = await fetchSystemInfo();

                if (!mounted) return;

                setInfo(data);
                setError(null);
            } catch (err) {
                if (!mounted) return;

                setError("Failed to load system info");
            } finally {
                if (mounted) setLoading(false);
            }
        };

        load();

        return () => {
            mounted = false;
        };
    }, []);

    return { info, loading, error };
}