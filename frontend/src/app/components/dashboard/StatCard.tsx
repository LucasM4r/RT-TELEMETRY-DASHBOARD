import type { ReactNode } from "react";

interface StatCardProps {
  title: string;
  icon: ReactNode;
  accentColor: string;
  children: ReactNode;
  className?: string;
  badge?: string;
}

export function StatCard({
  title,
  icon,
  accentColor,
  children,
  className = "",
  badge,
}: StatCardProps) {
  return (
    <div
      className={`
        rounded-xl
        p-4
        flex
        flex-col
        min-h-0
        h-full
        ${className}
      `}
      style={{
        background:
          "linear-gradient(135deg, #12121f 0%, #0e0e1a 100%)",

        border: `1px solid ${accentColor}22`,

        boxShadow: `
          0 0 20px ${accentColor}0a,
          inset 0 1px 0 ${accentColor}10
        `,

        position: "relative",
        overflow: "hidden",
      }}
    >
      {/* Glow top line */}
      <div
        style={{
          position: "absolute",
          top: 0,
          left: "10%",
          right: "10%",
          height: "1px",

          background: `
            linear-gradient(
              90deg,
              transparent,
              ${accentColor}60,
              transparent
            )
          `,
        }}
      />

      {/* HEADER */}
      <div
        className="
          flex
          items-center
          justify-between
          mb-3
          shrink-0
        "
      >
        <div className="flex items-center gap-2">
          <span style={{ color: accentColor }}>
            {icon}
          </span>

          <span
            style={{
              color: "#9ca3af",
              fontSize: "12px",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            {title}
          </span>
        </div>

        {badge && (
          <span
            style={{
              background: `${accentColor}20`,
              color: accentColor,
              border: `1px solid ${accentColor}40`,
              borderRadius: "4px",
              padding: "1px 7px",
              fontSize: "10px",
            }}
          >
            {badge}
          </span>
        )}
      </div>

      {/* CONTENT */}
      <div
        className="
          flex-1
          min-h-0
          flex
          flex-col
        "
      >
        {children}
      </div>
    </div>
  );
}