import { HardDrive } from "lucide-react";

import { GaugeRing } from "../dashboard/GaugeRing";
import { SparklineChart } from "../dashboard/SparklineChart";
import { StatCard } from "../dashboard/StatCard";

type DiskPanelProps = {
    diskUsed: number;
    diskTemp: number;
    diskRead: number;
    diskWrite: number;
    diskReadHistory: number[];

    storageTotal: number;
    storageModel?: string;

    accentColor: string;
};

export function DiskPanel({
    diskUsed,
    diskTemp,
    diskRead,
    diskWrite,
    diskReadHistory,

    storageTotal,
    storageModel,

    accentColor,
}: DiskPanelProps) {
    const usagePct = (diskUsed / storageTotal) * 100;

    return (
        <StatCard
            title="Storage"
            icon={<HardDrive size={14} />}
            accentColor={accentColor}
            badge={storageModel}
        >
            <div className="flex flex-col h-full gap-5 min-h-0">

                {/* TOP */}
                <div className="flex items-center gap-6 shrink-0">
                    <GaugeRing
                        value={usagePct}
                        color={accentColor}
                        label={`${usagePct.toFixed(0)}%`}
                        sublabel="USED"
                        size={110}
                    />

                    <div className="flex-1 space-y-2 text-[11px] text-gray-400">
                        <div className="flex justify-between border-b border-white/5 pb-1">
                            <span>Used</span>

                            <span className="text-orange-500 font-bold">
                                {diskUsed} GB
                            </span>
                        </div>

                        <div className="flex justify-between border-b border-white/5 pb-1">
                            <span>Available</span>

                            <span className="text-gray-200">
                                {(storageTotal - diskUsed).toFixed(1)} GB
                            </span>
                        </div>

                        <div className="flex justify-between">
                            <span>Temp</span>

                            <span className="text-gray-200">
                                {diskTemp.toFixed(0)}°C
                            </span>
                        </div>
                    </div>
                </div>

                {/* IO BOX */}
                <div className="grid grid-cols-2 gap-4 bg-[#0d0d1a]/50 p-3 rounded-lg border border-white/5 shrink-0">
                    <div className="text-center border-r border-white/5">
                        <div className="text-[9px] text-gray-500 uppercase font-bold mb-1">
                            Disk Read
                        </div>

                        <div className="text-xl font-bold text-gray-100">
                            {diskRead}

                            <span className="text-[9px] ml-1 text-gray-500 font-normal">
                                MB/s
                            </span>
                        </div>
                    </div>

                    <div className="text-center">
                        <div className="text-[9px] text-gray-500 uppercase font-bold mb-1">
                            Disk Write
                        </div>

                        <div className="text-xl font-bold text-gray-100">
                            {diskWrite}

                            <span className="text-[9px] ml-1 text-gray-500 font-normal">
                                MB/s
                            </span>
                        </div>
                    </div>
                </div>

                {/* CHART */}
                <div className="flex-1 min-h-0 flex flex-col justify-end">
                    <div className="text-[9px] text-gray-600 mb-2 font-bold uppercase tracking-widest">
                        I/O Activity Monitor
                    </div>

                    <div className="flex-1 min-h-0">
                        <SparklineChart
                            data={diskReadHistory}
                            color={accentColor}
                            height="100%"
                        />
                    </div>
                </div>
            </div>
        </StatCard>
    );
}