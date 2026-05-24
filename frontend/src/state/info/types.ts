export interface SystemInfo {
    cpu_model: string;
    cpu_cores: number;
    cpu_threads: number;
    mem_total: number; // GB
    gpu_model: string;
    gpu_mem_total: number; // GB
    storage_model: string;
    storage_total: number; // GB
}