import { useEffect } from "react";
import { WS_URL } from "@/config/api";
import { mapMetrics } from "../../../state/metrics/mapMetrics";
import type { SystemMetrics } from "../../../state/metrics/types";

export function useSystemWebSocket(
    setMetrics: React.Dispatch<React.SetStateAction<SystemMetrics>>
) {
    useEffect(() => {
        let ws: WebSocket | undefined;
        let timeout: any;
        let attempts = 0;

        const connect = () => {
            const backoff = Math.min(500 * 2 ** attempts, 10000);

            ws = new WebSocket(WS_URL);

            ws.onmessage = (event) => {
                const msg = JSON.parse(event.data);
                if (msg.type !== "metrics") return;

                setMetrics((prev) =>
                    mapMetrics(prev, msg.payload)
                );
            };

            ws.onclose = () => {
                attempts++;
                timeout = setTimeout(connect, backoff);
            };
        };

        connect();

        return () => {
            clearTimeout(timeout);
            ws?.close();
        };
    }, [setMetrics]);
}