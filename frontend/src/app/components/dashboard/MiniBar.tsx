interface MiniBarProps {
  value: number; // 0–100
  color: string;
  height?: number;
  showLabel?: boolean;
}

export function MiniBar({ value, color, height = 6, showLabel = false }: MiniBarProps) {
  return (
    <div className="flex items-center gap-2 w-full">
      <div
        className="flex-1 rounded-full overflow-hidden"
        style={{ height, background: "#1e1e2e" }}
      >
        <div
          style={{
            width: `${Math.min(100, value)}%`,
            height: "100%",
            background: color,
            borderRadius: "999px",
            boxShadow: `0 0 6px ${color}66`,
            transition: "width 0.5s ease",
          }}
        />
      </div>
      {showLabel && (
        <span style={{ color: "#9ca3af", fontSize: "11px", minWidth: "32px", textAlign: "right" }}>
          {value.toFixed(0)}%
        </span>
      )}
    </div>
  );
}
