package api

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/routes"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

type Server struct {
	mux *http.ServeMux
}

func NewServer(bridgeClient *bridge.CClient) *Server {
	mux := http.NewServeMux()
	routes.SetupRoutes(mux, bridgeClient)
	return &Server{
		mux: mux,
	}
}

func (s *Server) Router() *http.ServeMux {
	return s.mux
}
