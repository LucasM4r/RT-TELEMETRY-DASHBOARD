import { Cpu } from "lucide-react";

import { GaugeRing } from "../dashboard/GaugeRing";
import { MiniBar } from "../dashboard/MiniBar";
import { SparklineChart } from "../dashboard/SparklineChart";
import { StatCard } from "../dashboard/StatCard";

type CpuCore = {
    id: number;
    usage: number;
};

type CpuPanelProps = {
    cpuUsage: number;
    cpuFreq: number;
    cpuTemp: number;
    cpuHistory: number[];
    cpuCores: CpuCore[];

    cpuDisplayName?: string;

    coreCount: number;
    threadCount: number;

    cpuColor: string;
    cpuTempColor: string;

    accentColor: string;
};

export function CpuPanel({
    cpuUsage,
    cpuFreq,
    cpuTemp,
    cpuHistory,
    cpuCores,
    cpuDisplayName,
    coreCount,
    threadCount,
    cpuColor,
    cpuTempColor,
    accentColor,
}: CpuPanelProps) {
    return (
        <StatCard
            title="CPU"
            icon={<Cpu size={14} />}
            accentColor={accentColor}
            badge={cpuDisplayName}
        >
            <div className="h-full flex flex-col justify-between gap-5">

                {/* TOP */}
                <div className="flex items-start gap-5">
                    <GaugeRing
                        value={cpuUsage}
                        color={cpuColor}
                        label={`${cpuUsage.toFixed(0)}%`}
                        sublabel="USAGE"
                        size={110}
                        strokeWidth={6}
                    />

                    <div className="flex-1 grid gap-3">
                        {[
                            {
                                label: "Frequency",
                                value: `${cpuFreq.toFixed(2)} GHz`,
                                color: accentColor,
                            },
                            {
                                label: "Temp",
                                value: `${cpuTemp.toFixed(0)}°C`,
                                color: cpuTempColor,
                            },
                            {
                                label: "Cores",
                                value: `${coreCount}C / ${threadCount}T`,
                                color: "#e5e7eb",
                            },
                        ].map((item) => (
                            <div
                                key={item.label}
                                className="flex items-center justify-between"
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

                {/* CORES */}
                <div className="grid grid-cols-2 gap-x-4 gap-y-2">
                    {cpuCores.map((core) => (
                        <div
                            key={core.id}
                            className="flex items-center gap-2"
                        >
                            <span
                                className="text-[9px] w-4"
                                style={{ color: "#4b5563" }}
                            >
                                C{core.id}
                            </span>

                            <div className="flex-1">
                                <MiniBar
                                    value={core.usage}
                                    color={accentColor}
                                    height={4}
                                />
                            </div>

                            <span
                                className="text-[9px] w-7 text-right"
                                style={{ color: "#6b7280" }}
                            >
                                {core.usage.toFixed(0)}%
                            </span>
                        </div>
                    ))}
                </div>

                {/* CHART */}
                <div className="pt-2 border-t border-[#161625]">
                    <div
                        className="flex justify-between mb-2 text-[10px]"
                        style={{ color: "#4b5563" }}
                    >
                        <span>History (60s)</span>

                        <span style={{ color: accentColor }}>
                            {cpuUsage.toFixed(1)}%
                        </span>
                    </div>

                    <SparklineChart
                        data={cpuHistory}
                        color={accentColor}
                        gradientId="cpuGrad"
                        showTooltip
                    />
                </div>
            </div>
        </StatCard>
    );
}