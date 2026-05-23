package bridge

import "fmt"

func BuildSetLimitCommand(target string, value string) (string, error) {
	switch target {
	case "cpu":
		return "SET_CPU_ALERT_LIMIT:" + value, nil
	case "mem":
		return "SET_MEM_ALERT_LIMIT:" + value, nil
	case "cpu_temp":
		return "SET_CPU_TEMP_ALERT_LIMIT:" + value, nil
	case "gpu_temp":
		return "SET_GPU_TEMP_ALERT_LIMIT:" + value, nil
	case "storage":
		return "SET_STORAGE_ALERT_LIMIT:" + value, nil
	default:
		return "", fmt.Errorf("invalid target")
	}
}
