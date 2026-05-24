package websocket

import (
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/hub"
)

// readLoop continuously reads messages from the client's WebSocket connection and broadcasts them to the hub.
func readLoop(c *hub.Client, h *hub.Hub) {
	defer func() {
		h.Unregister <- c
		_ = c.Conn.Close()
	}()

	for {
		msg, err := readFrame(c.Conn)
		if err != nil {
			return
		}

		if len(msg) > 0 {
			h.Broadcast <- []byte(msg)
		}
	}
}

// writeLoop continuously listens for messages on the client's send channel and writes them to the client's WebSocket connection.
func writeLoop(c *hub.Client, h *hub.Hub) {
	defer func() {
		h.Unregister <- c
		_ = c.Conn.Close()
	}()

	for msg := range c.Send {
		if err := writeFrame(c.Conn, string(msg)); err != nil {
			return
		}
	}
}
