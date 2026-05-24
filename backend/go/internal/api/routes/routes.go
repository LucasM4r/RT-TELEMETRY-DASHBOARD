package routes

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/handlers"
)

// SetupRoutes registers the API routes and their corresponding handlers.
func SetupRoutes(mux *http.ServeMux) {
	mux.HandleFunc("/api/health", handlers.Health)
	mux.HandleFunc("/api/info", handlers.GetInfo)
	mux.HandleFunc("/api/weather", handlers.GetWeather)
	mux.HandleFunc("/api/config", handlers.GetConfig)
	mux.HandleFunc("POST /api/config/{target}/{value}", handlers.UpdateConfigLimit)
	mux.HandleFunc("POST /api/alert/{target}", handlers.PostAlert)

}
