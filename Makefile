DOCKER_COMPOSE = docker compose

run:
	$(DOCKER_COMPOSE) up --build -d

stop:
	$(DOCKER_COMPOSE) down

clean:
	$(DOCKER_COMPOSE) down --rmi all --volumes

logs:
	$(DOCKER_COMPOSE) logs -f

get-all:
	echo "GET_ALL"| nc localhost 5000

get-cpu:
	echo "GET_CPU"| nc localhost 5000

get-mem:
	echo "GET_MEM"| nc localhost 5000

get-alert:
	echo "GET_ALERT" | nc localhost 5000

get-status:
	echo "GET_STATUS" | nc localhost 5000

stop-alert:
	echo "STOP_ALERT" | nc localhost 5000

start-alert:
	echo "START_ALERT" | nc localhost 5000
	
restart-server:
	$(DOCKER_COMPOSE) restart server-c