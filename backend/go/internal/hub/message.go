package hub

// WSMessage represents a message sent over the WebSocket connection.
type WSMessage struct {
	Type    string      `json:"type"`
	Payload interface{} `json:"payload"`
}
