package websocket

import (
	"crypto/sha1"
	"encoding/base64"
	"fmt"
	"net"
	"strings"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/hub"
)

// websocketGUID is a constant defined by the WebSocket protocol for generating the Sec-WebSocket-Accept header.
const websocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// HandleWS performs the WebSocket handshake and registers the client with the hub.
func HandleWS(conn net.Conn, h *hub.Hub) error {
	req := make([]byte, 4096)
	n, err := conn.Read(req)

	if err != nil {
		_ = conn.Close()
		return err
	}

	// Parse the HTTP headers from the request
	headers := parseHeaders(string(req[:n]))

	if !strings.EqualFold(headers["upgrade"], "websocket") {
		return fmt.Errorf("not websocket request")
	}

	key := headers["sec-websocket-key"]
	if key == "" {
		return fmt.Errorf("missing Sec-WebSocket-Key")
	}

	// Generate the Sec-WebSocket-Accept value
	accept := generateAccept(key)

	// Add the necessary headers to complete the handshake
	resp :=
		"HTTP/1.1 101 Switching Protocols\r\n" +
			"Upgrade: websocket\r\n" +
			"Connection: Upgrade\r\n" +
			"Sec-WebSocket-Accept: " + accept + "\r\n\r\n"

	if _, err := conn.Write([]byte(resp)); err != nil {
		return err
	}

	client := &hub.Client{
		Conn: conn,
		Send: make(chan []byte, 256),
	}

	// Register the client with the hub
	h.Register <- client

	// Start the read and write loops for the client
	go readLoop(client, h)
	go writeLoop(client, h)

	return nil
}

// generateAccept computes the Sec-WebSocket-Accept value based on the client's Sec-WebSocket-Key.
func generateAccept(key string) string {
	h := sha1.New()
	h.Write([]byte(key + websocketGUID))
	return base64.StdEncoding.EncodeToString(h.Sum(nil))
}

// parseHeaders takes the raw HTTP request string and extracts the headers into a map for easy access.
func parseHeaders(raw string) map[string]string {
	lines := strings.Split(raw, "\r\n")
	headers := make(map[string]string)

	// Skip the first line (HTTP request line) and parse the headers
	for _, l := range lines {
		parts := strings.SplitN(l, ":", 2)
		if len(parts) != 2 {
			continue
		}
		// Store headers in a case-insensitive manner
		headers[strings.ToLower(strings.TrimSpace(parts[0]))] = strings.TrimSpace(parts[1])
	}

	return headers
}
