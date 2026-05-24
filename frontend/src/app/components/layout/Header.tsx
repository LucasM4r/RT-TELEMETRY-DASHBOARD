import { Activity } from "lucide-react";

type HeaderProps = {
    alertEnabled: boolean;
    onToggleAlarm: () => void;
};

export function Header({
    alertEnabled,
    onToggleAlarm,
}: HeaderProps) {
    return (
        <header
            style={{
                background: "linear-gradient(180deg, #0d0d1a 0%, #080810 100%)",
                borderBottom: "1px solid #1e1e3a",
            }}
            className="px-6 py-3 flex items-center justify-between"
        >
            <div className="flex items-center gap-3">
                <div
                    style={{
                        width: 1,
                        height: 20,
                        background: "#1e1e3a",
                        margin: "0 8px",
                    }}
                />

                <Activity size={16} style={{ color: "#00d4ff" }} />

                <span
                    style={{
                        color: "#e0e0e0",
                        fontSize: "13px",
                        letterSpacing: "0.1em",
                    }}
                >
                    SYSTEM MONITOR
                </span>

                <span
                    style={{
                        background: "#00d4ff18",
                        color: "#00d4ff",
                        border: "1px solid #00d4ff40",
                        borderRadius: "4px",
                        padding: "1px 8px",
                        fontSize: "10px",
                        letterSpacing: "0.08em",
                    }}
                >
                    LIVE
                </span>
            </div>

            <div className="flex items-center gap-4">
                <button
                    onClick={onToggleAlarm}
                    style={{
                        background: alertEnabled ? "#ef444422" : "#1e1e3a",
                        color: alertEnabled ? "#ef4444" : "#6b7280",
                        border: `1px solid ${alertEnabled ? "#ef444455" : "#374151"
                            }`,
                        borderRadius: "4px",
                        padding: "2px 10px",
                        fontSize: "10px",
                        fontWeight: "bold",
                        cursor: "pointer",
                        display: "flex",
                        alignItems: "center",
                        gap: "6px",
                        transition: "all 0.2s ease",
                    }}
                >
                    <div
                        style={{
                            width: 6,
                            height: 6,
                            borderRadius: "50%",
                            background: alertEnabled ? "#ef4444" : "#4b5563",
                            boxShadow: alertEnabled
                                ? "0 0 8px #ef4444"
                                : "none",
                        }}
                    />

                    {alertEnabled
                        ? "ALARM ACTIVE"
                        : "ALARM MUTED"}
                </button>
            </div>
        </header>
    );
}