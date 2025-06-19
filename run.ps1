# Set DISPLAY for Xming and run docker-compose
$env:DISPLAY = "host.docker.internal:0.0"
docker-compose up --build
