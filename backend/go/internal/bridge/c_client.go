package bridge

import (
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/config"
	"github.com/LucasM4r/rt-telemetry-backend/go/internal/tcp"
)

type CClient struct {
	host string
	port string
}

func NewCClient(host, port string) *CClient {
	return &CClient{
		host: host,
		port: port,
	}
}

func NewCClientFromEnv() *CClient {
	return NewCClient(
		config.GetEnv("C_SERVER_HOST", "localhost"),
		config.GetEnv("C_SERVER_PORT", "5000"),
	)
}

func (c *CClient) Call(cmd string) ([]byte, error) {
	client := tcp.NewClient(c.host, c.port, nil)

	if err := client.Connect(); err != nil {
		return nil, err
	}
	defer client.Close()

	return client.Fetch(cmd)
}
