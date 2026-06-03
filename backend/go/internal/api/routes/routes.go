package routes

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/handlers"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

func SetupRoutes(mux *http.ServeMux, bridgeClient *bridge.CClient) {
	api := handlers.NewAPI(bridgeClient)

	mux.HandleFunc("/api/health", api.Health)
	mux.HandleFunc("/api/info", api.GetInfo)
	mux.HandleFunc("/api/weather", api.GetWeather)

	mux.HandleFunc("GET /api/config", api.GetConfig)
	mux.HandleFunc("PATCH /api/config/{target}", api.UpdateConfigLimit)

	mux.HandleFunc("POST /api/alert/{target}", api.PostAlert)
}
