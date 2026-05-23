package handlers

import (
	"net/http"
	"strconv"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

// GetConfig handles GET requests to fetch the current configuration from the C server.
func GetConfig(w http.ResponseWriter, r *http.Request) {
	data, err := bridge.Client().Call("GET_CONFIG")
	if err != nil {
		http.Error(w, "Failed to reach C server", http.StatusInternalServerError)
		return
	}

	writeJSON(w, data)
}

// UpdateConfigLimit handles POST requests to update alert limits for various metrics.
func UpdateConfigLimit(w http.ResponseWriter, r *http.Request) {
	target := r.PathValue("target")
	value := r.PathValue("value")

	if valueInt, err := strconv.Atoi(value); err != nil || valueInt < 0 || valueInt > 100 {
		http.Error(w, "Value must be an integer between 0 and 100", http.StatusBadRequest)
		return
	}

	cmd, err := bridge.BuildSetLimitCommand(target, value)
	if err != nil {
		http.Error(w, "Invalid target", http.StatusBadRequest)
		return
	}

	data, err := bridge.Client().Call(cmd)
	if err != nil {
		http.Error(w, "Failed to reach C server", http.StatusInternalServerError)
		return
	}

	writeJSON(w, data)
}
