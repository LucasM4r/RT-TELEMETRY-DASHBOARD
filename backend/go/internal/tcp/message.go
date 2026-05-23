package tcp

// Metrics represents realtime system metrics.
type Metrics struct {
	CPUUsage float64   `json:"cpu"`
	CPUFreq  float64   `json:"cpu_freq"`
	CPUTemp  float64   `json:"cpu_temp"`
	CPUCores []float64 `json:"cpu_cores"`

	MemUsed   float64 `json:"mem_used"`
	MemCached float64 `json:"mem_cached"`
	MemUsage  float64 `json:"mem_usage"`

	GPUTemp      float64 `json:"gpu_temp"`
	GPUVRAMUsed  float64 `json:"gpu_vram_used"`
	GPUVRAMUsage float64 `json:"gpu_vram_usage"`

	NetDown      float64 `json:"net_down"`
	NetUp        float64 `json:"net_up"`
	NetTotalDown float64 `json:"net_total_down"`
	NetTotalUp   float64 `json:"net_total_up"`

	StorageRead  float64 `json:"storage_read"`
	StorageWrite float64 `json:"storage_write"`
	StorageTemp  float64 `json:"storage_temp"`
	StorageUsed  float64 `json:"storage_used"`
	StorageUsage float64 `json:"storage_usage"`

	CPUAlertLimit     float64 `json:"cpu_alert_limit"`
	MemAlertLimit     float64 `json:"mem_alert_limit"`
	CPUTempAlertLimit float64 `json:"cpu_temp_alert_limit"`
	GPUTempAlertLimit float64 `json:"gpu_temp_alert_limit"`
	StorageAlertLimit float64 `json:"storage_alert_limit"`

	AlertActive  int `json:"alert_active"`
	AlertEnabled int `json:"alert_enabled"`
}

// Info represents static system information.
type Info struct {
	CPUModel   string `json:"cpu_model"`
	CPUCores   int    `json:"cpu_cores"`
	CPUThreads int    `json:"cpu_threads"`

	MemTotal float64 `json:"mem_total"`

	GPUModel    string  `json:"gpu_model"`
	GPUMemTotal float64 `json:"gpu_mem_total"`

	StorageModel string  `json:"storage_model"`
	StorageTotal float64 `json:"storage_total"`
}

// Config represents alert configuration limits.
type Config struct {
	CPUAlertLimit     float64 `json:"cpu_usage_limit"`
	MemAlertLimit     float64 `json:"mem_usage_limit"`
	CPUTempAlertLimit float64 `json:"cpu_temp_limit"`
	GPUTempAlertLimit float64 `json:"gpu_temp_limit"`
	StorageAlertLimit float64 `json:"storage_usage_limit"`
}
