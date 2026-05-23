package bridge

import (
	"os"

	"github.com/LucasM4r/rt-telemetry-backend/go/internal/tcp"
)

var defaultClient = NewCClientFromEnv()

type CClient struct {
	host string
	port string
}

func Client() *CClient {
	return defaultClient
}

func NewCClient(host, port string) *CClient {
	return &CClient{
		host: host,
		port: port,
	}
}

func NewCClientFromEnv() *CClient {
	host := getEnv("C_SERVER_HOST", "localhost")
	port := getEnv("C_SERVER_PORT", "5000")
	return NewCClient(host, port)
}

func (c *CClient) Call(cmd string) ([]byte, error) {
	client := tcp.NewClient(c.host, c.port)

	if err := client.Connect(); err != nil {
		return nil, err
	}
	defer client.Close()

	return client.Fetch(cmd)
}

func getEnv(key, fallback string) string {
	if value := os.Getenv(key); value != "" {
		return value
	}
	return fallback
}
