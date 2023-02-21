#!/bin/bash

echo "Running migrations..."
./manage.py migrate

echo "Starting server"
./manage.py runserver 0.0.0.0:8000