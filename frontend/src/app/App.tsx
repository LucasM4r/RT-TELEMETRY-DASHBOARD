import { useSystemMetrics } from "./hooks/useSystemMetrics/useSystemMetrics";
import { useSystemInfo } from "./hooks/useSystemInfo";
import { useSystemConfig } from "./hooks/useSystemConfig";

import { Header } from "./components/layout/Header";
import { EmergencyOverlay } from "./components/layout/EmergencyOverlay";

import { CpuPanel } from "./components/panels/CpuPanel";
import { RamPanel } from "./components/panels/RamPanel";
import { GpuPanel } from "./components/panels/GpuPanel";
import { NetworkPanel } from "./components/panels/NetworkPanel";
import { DiskPanel } from "./components/panels/DiskPanel";
import { TemperaturePanel } from "./components/panels/TemperaturePanel";
import { ThresholdPanel } from "./components/panels/ThresholdPanel";

import { COLORS } from "@/config/colors";
import { THRESHOLD_CONTROLS } from "@/config/thresholds";

import { getTempColor, getUsageColor } from "@/utils/colors";

export default function App() {
  const m = useSystemMetrics();

  const { info, loading } = useSystemInfo();
  const { limits, updateLimit } = useSystemConfig();

  const cpuDisplayName = loading ? "Detecting CPU..." : info?.cpu_model;
  const coreCount = info?.cpu_cores || 0;
  const threadCount = info?.cpu_threads || 0;

  const ramTotal = info?.mem_total || 32;
  const vramTotal = info?.gpu_mem_total || 8;

  const storageTotal = info?.storage_total || 1000;

  const usageColors = {
    CPU: getUsageColor(m.cpuUsage),
    RAM: getUsageColor(m.ramUsage),
    GPU: getUsageColor(m.gpuUsage),
  }

  const tempColors = {
    CPU: getTempColor(m.cpuTemp),
    GPU: getTempColor(m.gpuTemp),
  }

  const isEmergency = m.alert_enabled && m.alert_active;
  return (
    <div
      className="min-h-screen w-full relative"
      style={{ background: "#080810", margin: 0, padding: 0 }}
    >
      <EmergencyOverlay active={isEmergency} />
      {/* Header */}
      <Header
        alertEnabled={m.alert_enabled}
        onToggleAlarm={() => m.toggleAlarm(m.alert_enabled)}
      />

      <div className="p-4 grid gap-4" style={{ gridTemplateColumns: "1fr 1fr 1fr", gridTemplateRows: "auto" }}>
        {/* ── CPU Panel ── */}
        <CpuPanel
          cpuUsage={m.cpuUsage}
          cpuFreq={m.cpuFreq}
          cpuTemp={m.cpuTemp}
          cpuHistory={m.cpuHistory}
          cpuCores={m.cpuCores}
          cpuDisplayName={cpuDisplayName}
          coreCount={coreCount}
          threadCount={threadCount}
          cpuColor={usageColors.CPU}
          cpuTempColor={tempColors.CPU}
          accentColor={COLORS.cpu}
        />
        {/* ── RAM Panel ── */}
        <RamPanel
          ramUsage={m.ramUsage}
          ramUsed={m.ramUsed}
          ramCached={m.ramCached}
          ramHistory={m.ramHistory}
          ramTotal={ramTotal}
          ramColor={usageColors.RAM}
          accentColor={COLORS.ram}
        />

        {/* ── GPU Panel ── */}
        <GpuPanel
          gpuUsage={m.gpuUsage}
          gpuTemp={m.gpuTemp}
          gpuVramUsed={m.gpuVramUsed}
          gpuHistory={m.gpuHistory}
          gpuModel={info?.gpu_model}
          vramTotal={vramTotal}
          gpuColor={usageColors.GPU}
          gpuTempColor={tempColors.GPU}
          accentColor={COLORS.gpu}
        />
        {/* ── Network Panel ── */}
        <NetworkPanel
          netDown={m.netDown}
          netUp={m.netUp}
          netTotalDown={m.netTotalDown}
          netTotalUp={m.netTotalUp}
          netDownHistory={m.netDownHistory}
          netUpHistory={m.netUpHistory}
          accentColor={COLORS.net}
        />

        {/* ── Disk Panel ── */}
        <DiskPanel
          diskUsed={m.diskUsed}
          diskTemp={m.diskTemp}
          diskRead={m.diskRead}
          diskWrite={m.diskWrite}
          diskReadHistory={m.diskReadHistory}
          storageTotal={storageTotal}
          storageModel={info?.storage_model}
          accentColor={COLORS.disk}
        />
        {/* ── Temperature ── */}
        <div className="flex flex-col gap-4">
          <TemperaturePanel
            accentColor={COLORS.temp}
            getTempColor={getTempColor}
            items={[
              {
                label: "CPU",
                value: m.cpuTemp,
                max: 100,
              },
              {
                label: "GPU",
                value: m.gpuTemp,
                max: 90,
              },
              {
                label: "SSD",
                value: m.diskTemp,
                max: 70,
              },
              {
                label: "City",
                value: m.externalWeather?.temp || 0,
                max: 50,
              },
            ]}
          />
          {/*Sliders*/}
          <ThresholdPanel
            controls={THRESHOLD_CONTROLS}
            limits={limits}
            updateLimit={updateLimit}
          />
        </div>
      </div>

      <style>{`
        @keyframes pulse {
          0%, 100% { opacity: 1; }
          50% { opacity: 0.3; }
        }
        @keyframes pulse-border {
          0% { opacity: 0.3; }
          50% { opacity: 1; }
          100% { opacity: 0.3; }
        }
      `}</style>
    </div>
  );
}