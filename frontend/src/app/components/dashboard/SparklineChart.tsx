import { useMemo } from "react";

interface SparklineChartProps {
  data: number[];
  color: string;
  gradientId?: string;
  height?: string | number;
  showTooltip?: boolean;
  unit?: string;
  yDomain?: [number, number];
}

export function SparklineChart({
  data,
  color,
  height = "100%",
  yDomain,
}: SparklineChartProps) {
  const width = 300;

  const svgHeight =
    typeof height === "number" ? height : 60;

  const cssHeight = height;

  const padY = 4;

  const [yMin, yMax] = useMemo(() => {
    if (yDomain) return yDomain;

    const min = Math.min(...data);
    const max = Math.max(...data);

    if (min === max) {
      return [0, max + 1];
    }

    return [min, max];
  }, [data, yDomain]);

  const points = useMemo(() => {
    if (data.length < 2) {
      return { line: "", area: "" };
    }

    const xStep = width / (data.length - 1);

    const toY = (v: number) => {
      const clamped = Math.max(yMin, Math.min(yMax, v));

      return (
        padY +
        ((yMax - clamped) / (yMax - yMin)) *
        (svgHeight - padY * 2)
      );
    };

    const coords = data.map(
      (v, i) => `${i * xStep},${toY(v)}`
    );

    const line = `M ${coords.join(" L ")}`;

    const area = `
      ${line}
      L ${(data.length - 1) * xStep},${svgHeight}
      L 0,${svgHeight}
      Z
    `;

    return { line, area };
  }, [data, svgHeight, yMin, yMax]);

  const gradId = `sg-${color.replace(
    /[^a-zA-Z0-9]/g,
    ""
  )}`;

  return (
    <svg
      width="100%"
      height={cssHeight}
      viewBox={`0 0 ${width} ${svgHeight}`}
      preserveAspectRatio="none"
      style={{
        display: "block",
        overflow: "visible",
      }}
    >
      <defs>
        <linearGradient
          id={gradId}
          x1="0"
          y1="0"
          x2="0"
          y2="1"
        >
          <stop
            offset="0%"
            stopColor={color}
            stopOpacity={0.25}
          />
          <stop
            offset="100%"
            stopColor={color}
            stopOpacity={0.02}
          />
        </linearGradient>
      </defs>

      <path
        d={points.area}
        fill={`url(#${gradId})`}
      />

      <path
        d={points.line}
        fill="none"
        stroke={color}
        strokeWidth="2"
        strokeLinecap="round"
        strokeLinejoin="round"
      />
    </svg>
  );
}