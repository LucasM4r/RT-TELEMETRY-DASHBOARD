import { MemoryStick } from "lucide-react";

import { GaugeRing } from "../dashboard/GaugeRing";
import { MiniBar } from "../dashboard/MiniBar";
import { SparklineChart } from "../dashboard/SparklineChart";
import { StatCard } from "../dashboard/StatCard";

type RamPanelProps = {
    ramUsage: number;
    ramUsed: number;
    ramCached: number;
    ramHistory: number[];
    ramTotal: number;
    ramColor: string;
    accentColor: string;
};

export function RamPanel({
    ramUsage,
    ramUsed,
    ramCached,
    ramHistory,
    ramTotal,
    ramColor,
    accentColor,
}: RamPanelProps) {
    return (
        <StatCard
            title="Memory"
            icon={<MemoryStick size={14} />}
            accentColor={accentColor}
        >
            <div className="h-full flex flex-col justify-between gap-5">

                {/* TOP */}
                <div className="flex items-start gap-5">
                    <GaugeRing
                        value={ramUsage}
                        color={ramColor}
                        label={`${ramUsage.toFixed(0)}%`}
                        sublabel="USAGE"
                        size={110}
                        strokeWidth={6}
                    />

                    <div className="flex-1 grid gap-3">
                        {[
                            {
                                label: "Used",
                                value: `${ramUsed.toFixed(1)} GB`,
                                color: accentColor,
                            },
                            {
                                label: "Available",
                                value: `${(ramTotal - ramUsed).toFixed(1)} GB`,
                                color: "#e5e7eb",
                            },
                            {
                                label: "Total",
                                value: `${ramTotal.toFixed(1)} GB`,
                                color: "#9ca3af",
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

                {/* BREAKDOWN */}
                <div className="grid gap-3">
                    {[
                        {
                            label: "In Use",
                            value: (ramUsed / ramTotal) * 100,
                            color: accentColor,
                        },
                        {
                            label: "Cache",
                            value: (ramCached / ramTotal) * 100,
                            color: "#3b82f6",
                        },
                        {
                            label: "Standby",
                            value:
                                100 -
                                ((ramUsed + ramCached) / ramTotal) * 100,
                            color: "#6b7280",
                        },
                    ].map((row) => (
                        <div
                            key={row.label}
                            className="flex items-center gap-3"
                        >
                            <span
                                className="w-12 text-[10px]"
                                style={{ color: "#4b5563" }}
                            >
                                {row.label}
                            </span>

                            <div className="flex-1">
                                <MiniBar
                                    value={row.value}
                                    color={row.color}
                                    height={5}
                                />
                            </div>

                            <span
                                className="w-8 text-right text-[10px]"
                                style={{ color: "#6b7280" }}
                            >
                                {row.value.toFixed(0)}%
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
                            {ramUsed.toFixed(1)} GB
                        </span>
                    </div>

                    <SparklineChart
                        data={ramHistory}
                        color={accentColor}
                        gradientId="ramGrad"
                        showTooltip
                        unit="%"
                    />
                </div>
            </div>
        </StatCard>
    );
}