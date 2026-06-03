package handlers

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

// PostAlert handles POST requests to activate and deactivate alerts
func (a *API) PostAlert(w http.ResponseWriter, r *http.Request) {
	target := r.PathValue("target")

	cmd, err := bridge.BuildActivateAlertCommand(target)

	if err != nil {
		http.Error(w, "Invalid target", http.StatusBadRequest)
		return
	}

	data, err := a.BridgeClient.Call(cmd)

	if err != nil {
		http.Error(w, "Failed to reach C server", http.StatusInternalServerError)
		return
	}

	writeJSON(w, data)
}
