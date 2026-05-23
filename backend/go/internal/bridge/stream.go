package bridge

import (
	"encoding/json"
	"log"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/hub"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/tcp"
)

// ForwardMetrics converts raw TCP metrics into WSMessage and forwards to the Hub broadcast channel.
func ForwardMetrics(h *hub.Hub, in <-chan []byte) {
	for raw := range in {
		var metrics tcp.Metrics
		if err := json.Unmarshal(raw, &metrics); err != nil {
			log.Printf("[Bridge] Unmarshal error: %v, raw data: %s", err, string(raw))
			continue
		}

		msg := hub.WSMessage{
			Type:    "metrics",
			Payload: metrics,
		}

		b, _ := json.Marshal(msg)
		h.Broadcast <- b
	}
}
