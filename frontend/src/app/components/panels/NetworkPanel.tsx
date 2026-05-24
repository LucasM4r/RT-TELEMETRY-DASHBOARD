import { ArrowDown, ArrowUp, Network } from "lucide-react";

import { SparklineChart } from "../dashboard/SparklineChart";
import { StatCard } from "../dashboard/StatCard";

type NetworkPanelProps = {
    netDown: number;
    netUp: number;
    netTotalDown: number;
    netTotalUp: number;
    netDownHistory: number[];
    netUpHistory: number[];
    accentColor: string;
};

export function NetworkPanel({
    netDown,
    netUp,
    netTotalDown,
    netTotalUp,
    netDownHistory,
    netUpHistory,
    accentColor,
}: NetworkPanelProps) {
    return (
        <StatCard
            title="Network"
            icon={<Network size={14} />}
            accentColor={accentColor}
        >
            <div className="flex flex-col h-full gap-4">
                {/* SPEED BOXES */}
                <div className="grid grid-cols-2 gap-3 bg-[#0d0d1a]/50 p-3 rounded-lg border border-white/5">
                    {/* DOWNLOAD */}
                    <div className="space-y-1">
                        <div className="flex items-center gap-1 text-[#00d4ff] text-[10px] font-bold uppercase tracking-tighter">
                            <ArrowDown size={10} />
                            Download
                        </div>

                        <div className="text-2xl font-bold text-gray-100 tabular-nums">
                            {netDown.toFixed(1)}

                            <span className="text-[10px] ml-1 text-gray-500">
                                Mbps
                            </span>
                        </div>

                        <div className="text-[9px] text-gray-600 italic">
                            Total: {netTotalDown.toFixed(1)}GB
                        </div>
                    </div>

                    {/* UPLOAD */}
                    <div className="space-y-1">
                        <div className="flex items-center gap-1 text-orange-400 text-[10px] font-bold uppercase tracking-tighter">
                            <ArrowUp size={10} />
                            Upload
                        </div>

                        <div className="text-2xl font-bold text-gray-100 tabular-nums">
                            {netUp.toFixed(1)}

                            <span className="text-[10px] ml-1 text-gray-500">
                                Mbps
                            </span>
                        </div>

                        <div className="text-[9px] text-gray-600 italic">
                            Total: {netTotalUp.toFixed(1)}GB
                        </div>
                    </div>
                </div>

                {/* TRAFFIC CHARTS */}
                <div className="flex-1 flex flex-col justify-around gap-4 min-h-[160px]">
                    {/* RX */}
                    <div className="relative">
                        <div className="text-[8px] text-blue-400/50 absolute -top-2 left-0 font-bold uppercase">
                            Rx Traffic Stream
                        </div>

                        <SparklineChart
                            data={netDownHistory}
                            color="#00d4ff"
                        />
                    </div>

                    {/* TX */}
                    <div className="relative">
                        <div className="text-[8px] text-orange-400/50 absolute -top-2 left-0 font-bold uppercase">
                            Tx Traffic Stream
                        </div>

                        <SparklineChart
                            data={netUpHistory}
                            color="#f59e0b"
                        />
                    </div>
                </div>
            </div>
        </StatCard>
    );
}