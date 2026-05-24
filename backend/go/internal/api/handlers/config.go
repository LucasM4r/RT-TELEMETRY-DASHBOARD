package handlers

import (
	"encoding/json"
	"net/http"
	"strconv"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

type UpdateLimitRequest struct {
	Value int `json:"value"`
}

func GetConfig(w http.ResponseWriter, r *http.Request) {
	data, err := bridge.Client().Call("GET_CONFIG")
	if err != nil {
		http.Error(w, "Failed to reach C server", http.StatusInternalServerError)
		return
	}

	writeJSON(w, data)
}

func UpdateConfigLimit(w http.ResponseWriter, r *http.Request) {
	target := r.PathValue("target")

	var body UpdateLimitRequest

	if err := json.NewDecoder(r.Body).Decode(&body); err != nil {
		http.Error(w, "Invalid JSON body", http.StatusBadRequest)
		return
	}

	if body.Value < 0 || body.Value > 100 {
		http.Error(w, "Value must be between 0 and 100", http.StatusBadRequest)
		return
	}

	cmd, err := bridge.BuildSetLimitCommand(
		target,
		strconv.Itoa(body.Value),
	)

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
