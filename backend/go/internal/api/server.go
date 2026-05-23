package api

import (
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/routes"
)

type Server struct {
	mux *http.ServeMux
}

func NewServer() *Server {
	mux := http.NewServeMux()
	routes.SetupRoutes(mux)
	return &Server{
		mux: mux,
	}
}

func (s *Server) Router() *http.ServeMux {
	return s.mux
}
