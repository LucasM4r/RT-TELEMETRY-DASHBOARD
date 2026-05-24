export function getTempColor(temp: number) {
    if (temp < 60) return "#00ff9d";
    if (temp < 75) return "#f59e0b";
    return "#ef4444";
}

export function getUsageColor(pct: number) {
    if (pct < 50) return "#00ff9d";
    if (pct < 80) return "#f59e0b";
    return "#ef4444";
}