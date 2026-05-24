interface GaugeRingProps {
  value: number; // 0-100
  size?: number;
  strokeWidth?: number;
  color: string;
  trackColor?: string;
  label?: string;
  sublabel?: string;
}

export function GaugeRing({
  value,
  size = 120,
  strokeWidth = 8,
  color,
  trackColor = "#1e1e2e",
  label,
  sublabel,
}: GaugeRingProps) {

  // Value Clamp
  const normalizedValue = Math.min(Math.max(value, 0), 100);

  // Geometry
  const radius = (size - strokeWidth) / 2;
  const circumference = 2 * Math.PI * radius;

  // Gauge de 270°
  const arcLength = circumference * 0.75;

  // Progress
  const progress = normalizedValue / 100;

  // Offset
  const dashOffset = arcLength * (1 - progress);

  const cx = size / 2;
  const cy = size / 2;

  return (
    <div
      className="relative flex items-center justify-center"
      style={{
        width: size,
        height: size,
      }}
    >
      <svg
        width={size}
        height={size}
        style={{
          transform: "rotate(135deg) scaleX(-1)",
          overflow: "visible",
        }}
      >
        {/* TRACK */}
        <circle
          cx={cx}
          cy={cy}
          r={radius}
          fill="none"
          stroke={trackColor}
          strokeWidth={strokeWidth}
          strokeDasharray={`${arcLength} ${circumference}`}
          strokeLinecap="round"
          opacity={0.9}
        />

        {/* PROGRESS */}
        <circle
          cx={cx}
          cy={cy}
          r={radius}
          fill="none"
          stroke={color}
          strokeWidth={strokeWidth}
          strokeDasharray={`${arcLength} ${circumference}`}
          strokeDashoffset={dashOffset}
          strokeLinecap="round"
          style={{
            transition: "stroke-dashoffset 0.45s ease",
            transformOrigin: "50% 50%",
            filter: `
              drop-shadow(0 0 3px ${color})
              drop-shadow(0 0 2px ${color}55)
            `,
          }}
        />
      </svg>

      {/* LABELS */}
      <div className="absolute inset-0 flex flex-col items-center justify-center pointer-events-none">
        {label && (
          <span
            style={{
              color,
              fontSize: "16px",
              fontWeight: 700,
              lineHeight: 1,
              letterSpacing: "-0.03em",
              textShadow: `0 0 8px ${color}55`,
            }}
          >
            {label}
          </span>
        )}

        {sublabel && (
          <span
            style={{
              color: "#6b7280",
              fontSize: "10px",
              marginTop: "4px",
              letterSpacing: "0.08em",
            }}
          >
            {sublabel}
          </span>
        )}
      </div>
    </div>
  );
}