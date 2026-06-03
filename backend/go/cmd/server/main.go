package main

import (
	"log"
	"net/http"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/api/middleware"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/config"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/hub"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/tcp"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/websocket"
)

func main() {

	h := hub.NewHub()
	go h.Run()

	cClient := bridge.NewCClientFromEnv()

	client := tcp.NewClient(
		config.GetEnv("C_SERVER_HOST", "localhost"),
		config.GetEnv("C_SERVER_PORT", "5000"),
		func(c *tcp.Client) error {
			return c.Send("STREAM")
		},
	)

	out := make(chan []byte, 256)
	defer close(out)

	go func() {
		if err := client.Start(out, nil); err != nil {
			log.Printf("[TCP][ERROR] Client stopped with error: %v", err)
		}
	}()

	go bridge.ForwardMetrics(h, out)

	wsServer := websocket.NewServer(h)

	go func() {
		log.Println("[WS] listening on :3000")
		log.Fatal(wsServer.ListenAndServe(":3000"))
	}()

	apiServer := api.NewServer(cClient)

	log.Println("[API] listening on :3001")
	log.Fatal(http.ListenAndServe(":3001", middleware.Cors(apiServer.Router())))
}
