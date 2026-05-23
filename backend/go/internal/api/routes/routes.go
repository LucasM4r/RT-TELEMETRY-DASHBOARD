package routes

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/handlers"
)

// SetupRoutes registers the API routes and their corresponding handlers.
func SetupRoutes(mux *http.ServeMux) {
	mux.HandleFunc("/api/health", handlers.Health)
	mux.HandleFunc("/api/info", handlers.GetInfo)
	mux.HandleFunc("GET /api/config", handlers.GetConfig)
	mux.HandleFunc("POST /config/{target}/{value}", handlers.UpdateConfigLimit)
}
