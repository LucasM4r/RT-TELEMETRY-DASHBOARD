import { Settings, Activity } from "lucide-react";
import { StatCard } from "../dashboard/StatCard";

type Control = {
    label: string;
    key: string;
    unit: string;
    min: number;
    max: number;
    color: string;
};

type ThresholdPanelProps = {
    controls: readonly Control[];
    limits: Record<string, number>;
    updateLimit: (key: string, value: number) => void;
};

export function ThresholdPanel({
    controls,
    limits,
    updateLimit,
}: ThresholdPanelProps) {
    return (
        <StatCard
            title="Thresholds"
            icon={<Settings size={12} />}
            accentColor="#4b5563"
        >
            <div className="flex-1 flex flex-col justify-between py-1 min-h-0">
                {controls.map((ctrl) => (
                    <div key={ctrl.key} className="flex flex-col gap-1">
                        <div className="flex justify-between items-center leading-none">
                            <label
                                style={{
                                    color: "#9ca3af",
                                    fontSize: "10px",
                                    fontWeight: 500,
                                }}
                            >
                                {ctrl.label}
                            </label>

                            <span
                                style={{
                                    color: ctrl.color,
                                    fontSize: "11px",
                                    fontWeight: "bold",
                                }}
                            >
                                {limits[ctrl.key]}
                                {ctrl.unit}
                            </span>
                        </div>

                        <div className="relative flex items-center h-3">
                            <input
                                type="range"
                                min={ctrl.min}
                                max={ctrl.max}
                                value={limits[ctrl.key]}
                                onChange={(e) =>
                                    updateLimit(ctrl.key, parseInt(e.target.value))
                                }
                                className="compact-slider"
                                style={{
                                    width: "100%",
                                    height: "3px",
                                    background: "#1e1e3a",
                                    borderRadius: "2px",
                                    appearance: "none",
                                    outline: "none",
                                    cursor: "pointer",
                                }}
                            />

                            <style>{`
                input.compact-slider::-webkit-slider-thumb {
                  appearance: none;
                  width: 10px;
                  height: 10px;
                  background: ${ctrl.color};
                  border-radius: 50%;
                  border: 1px solid #080810;
                }
              `}</style>
                        </div>
                    </div>
                ))}

                <div className="mt-1 pt-1 border-t border-[#1e1e3a] flex items-center justify-center gap-1 opacity-50">
                    <Activity size={8} style={{ color: "#00ff9d" }} />
                    <span style={{ color: "#6b7280", fontSize: "8px" }}>
                        CONTROL LINK
                    </span>
                </div>
            </div>
        </StatCard>
    );
}