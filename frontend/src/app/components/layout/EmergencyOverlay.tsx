type EmergencyOverlayProps = {
    active: boolean;
};

export function EmergencyOverlay({
    active,
}: EmergencyOverlayProps) {
    if (!active) return null;

    return (
        <>
            {/* Red border glow */}
            <div
                className="fixed inset-0 pointer-events-none z-[100]"
                style={{
                    boxShadow: "inset 0 0 60px rgba(239, 68, 68, 0.4)",
                    animation: "pulse-border 2s ease-in-out infinite",
                }}
            />

            {/* Critical banner */}
            <div
                className="fixed top-0 left-1/2 -translate-x-1/2 px-6 py-1 z-[101]"
                style={{
                    background: "#ef4444",
                    color: "#ffffff",
                    fontSize: "11px",
                    fontWeight: "bold",
                    letterSpacing: "0.12em",
                    boxShadow: "0 0 16px rgba(239, 68, 68, 0.5)",
                    animation: "pulse 1.2s ease-in-out infinite",
                }}
            >
                SYSTEM CRITICAL
            </div>
        </>
    );
}