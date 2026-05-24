import { API_URL } from "@/config/api";
import { sleep } from "@/utils/sleep";
import type { SystemInfo } from "@/state/info/types";

export async function fetchSystemInfo(): Promise<SystemInfo> {
    const maxAttempts = 5;

    for (let attempt = 0; attempt < maxAttempts; attempt++) {
        try {
            const res = await fetch(`${API_URL}/api/info`);

            if (!res.ok) {
                throw new Error("Failed to load system info");
            }

            return await res.json();
        } catch (err) {
            const backoff = Math.min(300 * 2 ** attempt, 5000);
            await sleep(backoff);
        }
    }

    throw new Error("Failed after multiple retries");
}