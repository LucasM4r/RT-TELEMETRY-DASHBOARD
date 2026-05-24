import { Thermometer } from "lucide-react";

import { MiniBar } from "../dashboard/MiniBar";
import { StatCard } from "../dashboard/StatCard";

type TemperatureItem = {
    label: string;
    value: number;
    max: number;
};

type TemperaturePanelProps = {
    items: TemperatureItem[];

    accentColor: string;

    getTempColor: (temp: number) => string;
};

export function TemperaturePanel({
    items,
    accentColor,
    getTempColor,
}: TemperaturePanelProps) {
    return (
        <StatCard
            title="Temperatures"
            icon={<Thermometer size={14} />}
            accentColor={accentColor}
        >
            <div className="grid grid-cols-2 gap-3">
                {items.map((t) => {
                    const color = getTempColor(t.value);

                    const min = 0;
                    const pct = ((t.value - min) / (t.max - min)) * 100;

                    return (
                        <div
                            key={t.label}
                            className="flex flex-col gap-1.5 rounded-lg p-3"
                            style={{
                                background: "#0d0d1a",
                                border: `1px solid ${color}22`,
                            }}
                        >
                            <div className="flex items-center justify-between">
                                <span
                                    style={{
                                        color: "#6b7280",
                                        fontSize: "10px",
                                    }}
                                >
                                    {t.label}
                                </span>

                                <span
                                    style={{
                                        color,
                                        fontSize: "13px",
                                        fontWeight: 600,
                                    }}
                                >
                                    {t.value.toFixed(0)}°C
                                </span>
                            </div>

                            <MiniBar
                                value={pct > 100 ? 100 : pct}
                                color={color}
                                height={4}
                            />
                        </div>
                    );
                })}
            </div>
        </StatCard>
    );
}