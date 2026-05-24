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

	client := tcp.NewClient(
		config.GetEnv("C_SERVER_HOST", "localhost"),
		config.GetEnv("C_SERVER_PORT", "5000"),
	)

	if err := client.Connect(); err != nil {
		log.Fatalf("TCP connect error: %v", err)
	}
	defer client.Close()

	if err := client.Send("STREAM"); err != nil {
		log.Fatalf("TCP send error: %v", err)
	}

	out := make(chan []byte, 256)

	go func() {
		if err := client.Listen(out); err != nil {
			log.Printf("TCP listen error: %v", err)
		}
	}()

	go bridge.ForwardMetrics(h, out)

	wsServer := websocket.NewServer(h)

	go func() {
		log.Println("[WS] listening on :3000")
		log.Fatal(wsServer.ListenAndServe(":3000"))
	}()

	apiServer := api.NewServer()

	log.Println("[API] listening on :3001")
	log.Fatal(http.ListenAndServe(":3001", middleware.Cors(apiServer.Router())))
}
