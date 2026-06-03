package handlers

import (
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/bridge"
)

type API struct {
	BridgeClient *bridge.CClient
}

func NewAPI(b *bridge.CClient) *API {
	return &API{
		BridgeClient: b,
	}
}
