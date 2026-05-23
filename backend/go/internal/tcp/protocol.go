package tcp

import "encoding/json"

// Decode converts JSON data into any struct type
func Decode[T any](data []byte) (T, error) {
	var result T

	if err := json.Unmarshal(data, &result); err != nil {
		return result, err
	}

	return result, nil
}
