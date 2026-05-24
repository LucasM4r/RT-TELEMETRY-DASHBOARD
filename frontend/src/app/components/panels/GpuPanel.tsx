import { Microchip } from "lucide-react";

import { GaugeRing } from "../dashboard/GaugeRing";
import { MiniBar } from "../dashboard/MiniBar";
import { SparklineChart } from "../dashboard/SparklineChart";
import { StatCard } from "../dashboard/StatCard";

type GpuPanelProps = {
    gpuUsage: number;
    gpuTemp: number;
    gpuVramUsed: number;
    gpuHistory: number[];

    gpuModel?: string;
    vramTotal: number;

    gpuColor: string;
    gpuTempColor: string;
    accentColor: string;
};

export function GpuPanel({
    gpuUsage,
    gpuTemp,
    gpuVramUsed,
    gpuHistory,
    gpuModel,
    vramTotal,
    gpuColor,
    gpuTempColor,
    accentColor,
}: GpuPanelProps) {
    return (
        <StatCard
            title="GPU"
            icon={<Microchip size={14} />}
            accentColor={accentColor}
            badge={gpuModel || "Detectando GPU..."}
        >
            <div className="h-full flex flex-col justify-between gap-5">

                {/* TOP */}
                <div className="flex items-start gap-5">
                    <GaugeRing
                        value={gpuUsage}
                        color={gpuColor}
                        label={`${gpuUsage.toFixed(0)}%`}
                        sublabel="USAGE"
                        size={110}
                        strokeWidth={6}
                    />

                    <div className="flex-1 grid gap-3">
                        {[
                            {
                                label: "Temp",
                                value: `${gpuTemp.toFixed(0)}°C`,
                                color: gpuTempColor,
                            },
                            {
                                label: "VRAM Used",
                                value: `${gpuVramUsed.toFixed(1)} GB`,
                                color: accentColor,
                            },
                            {
                                label: "VRAM Total",
                                value: `${vramTotal.toFixed(1)} GB`,
                                color: "#e5e7eb",
                            },
                        ].map((item) => (
                            <div
                                key={item.label}
                                className="flex justify-between items-center"
                            >
                                <span
                                    className="text-[11px]"
                                    style={{ color: "#6b7280" }}
                                >
                                    {item.label}
                                </span>

                                <span
                                    className="text-[11px] font-medium"
                                    style={{ color: item.color }}
                                >
                                    {item.value}
                                </span>
                            </div>
                        ))}
                    </div>
                </div>

                {/* VRAM */}
                <div className="grid gap-2">
                    <div
                        className="flex justify-between text-[10px]"
                        style={{ color: "#6b7280" }}
                    >
                        <span>VRAM Usage</span>

                        <span>
                            {gpuVramUsed.toFixed(1)} /
                            {vramTotal.toFixed(1)} GB
                        </span>
                    </div>

                    <MiniBar
                        value={(gpuVramUsed / vramTotal) * 100}
                        color={accentColor}
                        height={6}
                    />
                </div>

                {/* CHART */}
                <div className="pt-2 border-t border-[#161625]">
                    <div
                        className="flex justify-between mb-2 text-[10px]"
                        style={{ color: "#4b5563" }}
                    >
                        <span>History (60s)</span>

                        <span style={{ color: accentColor }}>
                            {gpuUsage.toFixed(1)}%
                        </span>
                    </div>

                    <SparklineChart
                        data={gpuHistory}
                        color={accentColor}
                        gradientId="gpuGrad"
                        showTooltip
                    />
                </div>
            </div>
        </StatCard>
    );
}