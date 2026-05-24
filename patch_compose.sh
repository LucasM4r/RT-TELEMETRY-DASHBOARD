sed -i '/container_name: telemetry-server/a \    network_mode: "host"' docker-compose.yaml
sed -i 's/ports:/#ports:/g' docker-compose.yaml
