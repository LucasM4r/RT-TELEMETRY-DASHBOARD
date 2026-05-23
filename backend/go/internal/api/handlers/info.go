package handlers

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

// GetInfo handles GET requests to fetch general information from the C server.
func GetInfo(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
		return
	}

	data, err := bridge.Client().Call("GET_INFO")
	if err != nil {
		http.Error(w, "failed to fetch info from C server", http.StatusInternalServerError)
		return
	}

	writeJSON(w, data)
}
