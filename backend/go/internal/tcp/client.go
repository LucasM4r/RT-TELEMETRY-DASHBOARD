// Package tcp provides simple TCP client utilities.
package tcp

import (
	"bufio"
	"errors"
	"fmt"
	"log"
	"net"
	"time"
)

var ErrNotConnected = errors.New("client is not connected")

// Client represents a TCP client.
type Client struct {
	Host string
	Port string

	conn net.Conn

	// Optional callback invoked after a successful connection.
	OnConnect func(c *Client) error
}

type ReconnectConfig struct {
	BaseDelay     time.Duration
	MaxDelay      time.Duration
	BackoffFactor float64
}

// NewClient creates a new TCP client instance.
func NewClient(host, port string, onConnect func(c *Client) error) *Client {

	if onConnect == nil {
		onConnect = func(c *Client) error { return nil }
	}

	return &Client{
		Host:      host,
		Port:      port,
		OnConnect: onConnect,
	}
}

// Connect establishes a TCP connection to the configured server.
func (c *Client) Connect() error {
	addr := net.JoinHostPort(c.Host, c.Port)

	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return fmt.Errorf("connect to %s: %w", addr, err)
	}

	c.conn = conn

	return nil
}

// IsConnected reports whether the client has an active connection.
func (c *Client) IsConnected() bool {
	return c.conn != nil
}

// Send writes a command to the connection.
func (c *Client) Send(cmd string) error {
	if !c.IsConnected() {
		return ErrNotConnected
	}

	_, err := c.conn.Write([]byte(cmd + "\n"))
	if err != nil {
		return fmt.Errorf("write command: %w", err)
	}

	return nil
}

// Close closes the client connection.
func (c *Client) Close() error {
	if c.conn == nil {
		return nil
	}

	if err := c.conn.Close(); err != nil {
		return fmt.Errorf("close connection: %w", err)
	}

	c.conn = nil

	return nil
}

// Listen continuously reads lines from the connection.
func (c *Client) Listen(out chan<- []byte) error {
	if !c.IsConnected() {
		return ErrNotConnected
	}

	scanner := bufio.NewScanner(c.conn)

	for scanner.Scan() {
		data := append([]byte(nil), scanner.Bytes()...)
		out <- data
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("listen connection: %w", err)
	}

	return nil
}

// Fetch sends a command and waits for a single line response.
func (c *Client) Fetch(cmd string) ([]byte, error) {
	if !c.IsConnected() {
		return nil, ErrNotConnected
	}

	if err := c.Send(cmd); err != nil {
		return nil, err
	}

	reader := bufio.NewReader(c.conn)

	data, err := reader.ReadBytes('\n')
	if err != nil {
		return nil, fmt.Errorf("read response: %w", err)
	}

	return data, nil
}

func (c *Client) Start(out chan<- []byte, reconnectConfig *ReconnectConfig) error {
	if reconnectConfig == nil {
		reconnectConfig = DefaultReconnectConfig()
	}

	currentDelay := reconnectConfig.BaseDelay
	for {

		log.Println("[TCP][INFO] Attempting to connect")
		if err := c.Connect(); err != nil {
			log.Printf("[TCP][ERROR] Connection failed: %v", err)

			time.Sleep(currentDelay)

			newDelay := time.Duration(float64(currentDelay) * reconnectConfig.BackoffFactor)

			currentDelay = newDelay
			if currentDelay > reconnectConfig.MaxDelay {
				currentDelay = reconnectConfig.MaxDelay
			}

			continue
		}

		currentDelay = reconnectConfig.BaseDelay

		if c.OnConnect != nil {
			if err := c.OnConnect(c); err != nil {
				log.Printf("[TCP][ERROR] OnConnect callback failed: %v", err)
				c.Close()
				time.Sleep(currentDelay)
				continue
			}
		}
		log.Println("[TCP][INFO] Connected successfully")

		if err := c.Listen(out); err != nil {
			log.Printf("[TCP][ERROR] Listen error: %v", err)
		}

		c.Close()
		log.Println("[TCP][INFO] Connection closed, retrying...")
		time.Sleep(currentDelay)
	}
}

func (r *ReconnectConfig) ApplyParameters(baseDelay, maxDelay time.Duration, backoffFactor float64) {
	r.BaseDelay = baseDelay
	r.MaxDelay = maxDelay
	r.BackoffFactor = backoffFactor
}

func DefaultReconnectConfig() *ReconnectConfig {
	return &ReconnectConfig{
		BaseDelay:     1 * time.Second,
		MaxDelay:      30 * time.Second,
		BackoffFactor: 2.0,
	}
}
