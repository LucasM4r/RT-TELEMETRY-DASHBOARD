package websocket

import (
	"log"
	"net"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/hub"
)

// Server represents the WebSocket server that listens for incoming connections and manages client interactions.
type Server struct {
	hub *hub.Hub
}

// NewServer initializes a new Server instance with the provided Hub.
func NewServer(h *hub.Hub) *Server {
	return &Server{
		hub: h,
	}
}

// ListenAndServe starts the WebSocket server and listens for incoming connections on the specified address.
func (s *Server) ListenAndServe(addr string) error {
	ln, err := net.Listen("tcp", addr)
	if err != nil {
		return err
	}
	defer ln.Close()

	log.Println("[WS] listening on", addr)

	for {
		conn, err := ln.Accept()
		if err != nil {
			continue
		}

		go HandleWS(conn, s.hub)
	}
}
