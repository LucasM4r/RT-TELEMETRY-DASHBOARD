package routes

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/handlers"
)

func SetupRoutes(mux *http.ServeMux) {
	mux.HandleFunc("/api/health", handlers.Health)
	mux.HandleFunc("/api/info", handlers.GetInfo)
	mux.HandleFunc("/api/weather", handlers.GetWeather)

	mux.HandleFunc("GET /api/config", handlers.GetConfig)
	mux.HandleFunc("PATCH /api/config/{target}", handlers.UpdateConfigLimit)

	mux.HandleFunc("POST /api/alert/{target}", handlers.PostAlert)
}
